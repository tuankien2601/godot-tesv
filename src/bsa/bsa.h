#ifndef GD_BSA
#define GD_BSA

#include <cstdint>
#include <vector>
#include <map>
#include <string>
#include <filesystem>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot
{
	class BSA : public RefCounted
	{
		GDCLASS(BSA, RefCounted)
	private:
		enum ArchiveFlags
		{
			ArchiveFlag_FolderNames = 0x0001,
			ArchiveFlag_FileNames = 0x0002,
			ArchiveFlag_Compress = 0x0004,
			ArchiveFlag_RetainDir = 0x0008,
			ArchiveFlag_RetainName = 0x0010,
			ArchiveFlag_RetainFileOffset = 0x0020,
			ArchiveFlag_Xbox360 = 0x0040,
			ArchiveFlag_StartUpStr = 0x0080,
			ArchiveFlag_EmbeddedNames = 0x0100,
			ArchiveFlag_XMem = 0x0200,
		};

		enum Version
		{
			Version_TES4 = 0x67,
			Version_FO3 = 0x68,
			Version_SSE = 0x69,
		};

		enum FileFlags
		{
			FileFlag_NIF = 0x001,
			FileFlag_DDS = 0x002,
			FileFlag_XML = 0x004,
			FileFlag_WAV = 0x008,
			FileFlag_MP3 = 0x010,
			FileFlag_TXT = 0x020, // TXT, HTML, BAT, SCC
			FileFlag_SPT = 0x040,
			FileFlag_FNT = 0x080,  // TEX, FNT
			FileFlag_MISC = 0x100, // CTL and others
		};

		enum FileSizeFlags
		{
			FileSizeFlag_Compression = 0x40000000,
		};

#pragma pack(push)
#pragma pack(1)
		struct Hash
		{
			uint32_t low, high;
		};
#pragma pack(pop)

		struct Header
		{
			std::uint32_t mFormat;
			std::uint32_t mVersion;
			std::uint32_t mFoldersOffset;
			std::uint32_t mFlags;
			std::uint32_t mFolderCount;
			std::uint32_t mFileCount;
			std::uint32_t mFolderNamesLength;
			std::uint32_t mFileNamesLength;
			std::uint32_t mFileFlags;
		};

		Header mHeader;

		struct FileRecord
		{
			std::uint64_t mHash;
			std::vector<char> mName;
			std::uint32_t mSize{0u};
			std::uint32_t mOffset{std::numeric_limits<uint32_t>::max()};
		};

		struct FolderRecord
		{
			std::uint32_t mCount;
			std::int64_t mOffset;
			std::map<std::uint64_t, FileRecord> mFiles;
		};

		std::filesystem::path mFilepath;

		std::map<std::uint64_t, FolderRecord> mFolders;

		FileRecord get_file_record(const std::string &str) const;

		static std::uint64_t generate_hash(const std::filesystem::path &stem, std::string extension);

	protected:
		static void _bind_methods();

	public:
		static Ref<BSA> open(String path);
		
		PackedByteArray get_file(String path);

		PackedStringArray get_file_list();

		bool has_file(String path);
	};
}

#endif