#include "bsa.h"
#include <algorithm>
#include <cctype>
#include <cassert>
#include <fstream>
#include <lz4frame.h>

using namespace godot;

Ref<BSA> BSA::open(String bsaFile)
{
	std::filesystem::path path(bsaFile.utf8().get_data());
	assert(std::filesystem::exists(path));
	std::ifstream input(path, std::ios_base::binary);

	// Total archive size
	std::streamoff fsize = 0;
	if (input.seekg(0, std::ios_base::end))
	{
		fsize = input.tellg();
		input.seekg(0);
	}

	assert(fsize >= 36);

	Ref<BSA> bsa = memnew(BSA);
	bsa->mFilepath = path;

	input.read(reinterpret_cast<char *>(&bsa->mHeader), sizeof(BSA::Header));

	auto &header = bsa->mHeader;

	assert(header.mFormat == static_cast<std::uint32_t>(0x415342));
	assert(header.mVersion == Version_TES4 || header.mVersion == Version_FO3 || header.mVersion == Version_SSE);

	if (header.mVersion == Version_TES4)
		header.mFlags &= (~ArchiveFlag_EmbeddedNames);

	input.seekg(header.mFoldersOffset);

	struct FlatFolderRecord
	{
		std::uint64_t mHash;
		std::string mName;
		std::uint32_t mCount;
		std::int64_t mOffset;
	};

	std::vector<std::pair<FlatFolderRecord, std::vector<FileRecord>>> folders;
	folders.resize(header.mFolderCount);
	for (auto &[folder, filelist] : folders)
	{
		input.read(reinterpret_cast<char *>(&folder.mHash), 8);
		input.read(reinterpret_cast<char *>(&folder.mCount), 4);
		if (header.mVersion == Version_SSE) // SSE
		{
			std::uint32_t unknown = 0;
			input.read(reinterpret_cast<char *>(&unknown), 4);
			input.read(reinterpret_cast<char *>(&folder.mOffset), 8);
		}
		else
		{
			input.read(reinterpret_cast<char *>(&folder.mOffset), 4);
		}
	}

	assert(!input.bad());

	// file record blocks
	if ((header.mFlags & ArchiveFlag_FolderNames) == 0)
		header.mFolderCount = 1; // TODO: not tested - unit test necessary

	for (auto &[folder, filelist] : folders)
	{
		if ((header.mFlags & ArchiveFlag_FolderNames) != 0)
		{
			uint8_t size = 0;
			input.read(reinterpret_cast<char *>(&size), 1);
			if (size > 0)
			{
				folder.mName.resize(size);
				input.getline(folder.mName.data(), size, '\0');
				assert(input.gcount() == static_cast<std::streamsize>(size));
				assert(header.mFolderNamesLength >= size);
				folder.mName.resize(size - 1);
			}
			header.mFolderNamesLength -= size;
		}

		filelist.resize(folder.mCount);
		for (auto &file : filelist)
		{
			input.read(reinterpret_cast<char *>(&file.mHash), 8);
			input.read(reinterpret_cast<char *>(&file.mSize), 4);
			input.read(reinterpret_cast<char *>(&file.mOffset), 4);
		}
	}

	if (header.mFolderNamesLength != 0)
		input.ignore(header.mFolderNamesLength);

	assert(!input.bad());

	if ((header.mFlags & ArchiveFlag_FileNames) != 0)
	{
		for (auto &[folder, filelist] : folders)
		{
			for (auto &file : filelist)
			{
				auto &name = file.mName;
				name.resize(256);
				input.getline(name.data(), 256, '\0');
				assert(input.gcount() > 1);
				assert(header.mFileNamesLength >= input.gcount());
				name.resize(input.gcount());
				assert(name.back() == '\0');
				header.mFileNamesLength -= static_cast<std::uint32_t>(input.gcount());
				file.mName.insert(file.mName.begin(), folder.mName.begin(), folder.mName.end());
				file.mName.insert(file.mName.begin() + folder.mName.size(), '\\');
			}
		}
	}

	if (header.mFileNamesLength != 0)
		input.ignore(header.mFileNamesLength);

	assert(!input.bad());

	for (auto &[folder, filelist] : folders)
	{
		std::map<std::uint64_t, FileRecord> fileMap;
		for (const auto &file : filelist)
			fileMap[file.mHash] = std::move(file);
		auto &folderMap = bsa->mFolders[folder.mHash];
		folderMap = FolderRecord{folder.mCount, folder.mOffset, std::move(fileMap)};
	}

	return bsa;
}

BSA::FileRecord BSA::get_file_record(const std::string &str) const
{
	for (const auto c : str)
	{
		assert(((static_cast<unsigned>(c) >> 7U) & 1U) == 0U);
	}

#ifdef _WIN32
	const auto &path = str;
#else
	// Force-convert the path into something UNIX can handle first
	// to make sure std::filesystem::path doesn't think the entire path is the filename on Linux
	// and subsequently purge it to determine the file folder.
	std::string path = str;
	std::replace(path.begin(), path.end(), '\\', '/');
#endif

	const auto p = std::filesystem::path{path}; // Purposefully damage Unicode strings.
	const auto stem = p.stem();
	const auto ext = p.extension().string(); // Purposefully damage Unicode strings.

	std::uint64_t folderHash = generate_hash(p.parent_path(), {});

	auto it = mFolders.find(folderHash);
	if (it == mFolders.end())
		return FileRecord();

	std::uint64_t fileHash = generate_hash(stem, ext);
	auto iter = it->second.mFiles.find(fileHash);
	if (iter == it->second.mFiles.end())
		return FileRecord();

	return iter->second;
}

bool BSA::has_file(String path)
{
	FileRecord fileRec = get_file_record(path.utf8().get_data());
	return fileRec.mOffset != std::numeric_limits<uint32_t>::max();
}

PackedByteArray BSA::get_file(String path)
{
	FileRecord fileRecord = get_file_record(path.utf8().get_data());
	PackedByteArray output;
	
	if (fileRecord.mOffset == std::numeric_limits<uint32_t>::max()) return output;

	size_t size = fileRecord.mSize & (~FileSizeFlag_Compression);
	size_t resultSize = size;
	std::ifstream streamPtr(mFilepath, std::ios_base::binary);
	streamPtr.seekg(fileRecord.mOffset);
	bool compressed = (fileRecord.mSize != size) == ((mHeader.mFlags & ArchiveFlag_Compress) == 0);
	if ((mHeader.mFlags & ArchiveFlag_EmbeddedNames) != 0)
	{
		// Skip over the embedded file name
		uint8_t length = 0;
		streamPtr.read(reinterpret_cast<char *>(&length), 1);
		streamPtr.ignore(length);
		size -= length + sizeof(uint8_t);
	}
	if (compressed)
	{
		streamPtr.read(reinterpret_cast<char *>(&resultSize), sizeof(uint32_t));
		size -= sizeof(uint32_t);
	}
	 
	output.resize(resultSize);

	if (compressed)
	{
		PackedByteArray buffer;
		buffer.resize(size);
		streamPtr.read(reinterpret_cast<char*>(buffer.ptrw()), size);

		if (mHeader.mVersion != Version_SSE)
		{
			output = buffer.decompress(resultSize, 1);
		}
		else
		{
			LZ4F_decompressionContext_t context = nullptr;
			LZ4F_createDecompressionContext(&context, LZ4F_VERSION);
			LZ4F_decompressOptions_t options = {};
			LZ4F_errorCode_t errorCode = LZ4F_decompress(
				context, output.ptrw(), &resultSize, buffer.ptr(), &size, &options);
			assert(!LZ4F_isError(errorCode));
			errorCode = LZ4F_freeDecompressionContext(context);
			assert(!LZ4F_isError(errorCode));
		}
	}
	else
	{
		streamPtr.read(reinterpret_cast<char*>(output.ptrw()), size);
	}

	return output;
}

inline void to_lowercase(std::string &str)
{
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
}

std::uint64_t BSA::generate_hash(const std::filesystem::path &stem, std::string extension)
{
	auto str = stem.u8string();
	size_t len = str.length();
	if (len == 0)
		return 0;
	std::replace(str.begin(), str.end(), '/', '\\');
	to_lowercase(str);
	uint64_t result = str[len - 1];
	if (len >= 3)
		result |= str[len - 2] << 8;
	result |= len << 16;
	result |= static_cast<uint32_t>(str[0] << 24);
	if (len >= 4)
	{
		uint32_t hash = 0;
		for (size_t i = 1; i <= len - 3; ++i)
			hash = hash * 0x1003f + str[i];
		result += static_cast<uint64_t>(hash) << 32;
	}
	if (extension.empty())
		return result;
	to_lowercase(extension);
	if (extension == ".kf")
		result |= 0x80;
	else if (extension == ".nif")
		result |= 0x8000;
	else if (extension == ".dds")
		result |= 0x8080;
	else if (extension == ".wav")
		result |= 0x80000000;
	uint32_t hash = 0;
	for (const auto &c : extension)
		hash = hash * 0x1003f + c;
	result += static_cast<uint64_t>(hash) << 32;
	return result;
}

PackedStringArray BSA::get_file_list()
{
	PackedStringArray output;
	for (auto item : mFolders)
	{
		auto folder = item.second;
		for (auto file_item : folder.mFiles)
		{
			auto file = file_item.second;
			String file_name(file.mName.data());
			output.append(file_name);
		}
	}
	return output;
}

void BSA::_bind_methods() {
	ClassDB::bind_static_method("BSA", D_METHOD("open", "path"), &BSA::open);
	ClassDB::bind_method(D_METHOD("has_file", "path"), &BSA::has_file);
	ClassDB::bind_method(D_METHOD("get_file", "path"), &BSA::get_file);
	ClassDB::bind_method(D_METHOD("get_file_list"), &BSA::get_file_list);
}