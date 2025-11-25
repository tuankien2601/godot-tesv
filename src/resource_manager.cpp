#include "resource_manager.h"
#include "bsa/bsa.h"
#include "nif/nif_loader.h"
#include "dds/dds_loader.h"
#include "resource_cache.h"
#include <filesystem>
#include <fstream>

using namespace godot;
namespace fs = std::filesystem;

fs::path TESVResourceManager::_gameDir;
fs::path TESVResourceManager::_dataDir;
Vector<Ref<BSA>> TESVResourceManager::_archives;

bool TESVResourceManager::set_game_dir(String path)
{
	fs::path root_dir(path.utf8().get_data());

	fs::path data_dir = root_dir / "data";

	if (fs::exists(data_dir) && fs::is_directory(data_dir))
	{
		for (const auto &entry : fs::directory_iterator(data_dir))
		{
			if (entry.path().extension() == ".bsa") add_archive(entry.path().c_str());
		}

		_gameDir = root_dir;
		_dataDir = data_dir;

		return true;
	}

	return false;
}

bool TESVResourceManager::add_archive(String path)
{
	auto bsa = BSA::open(path);
	if (bsa.is_valid())
	{
		_archives.append(bsa);
		return true;
	}

	return false;
}

PackedStringArray TESVResourceManager::get_file_list()
{
	PackedStringArray file_list;
	for (auto bsa : _archives)
	{
		file_list.append_array(bsa->get_file_list());
	}

	return file_list;
}

bool TESVResourceManager::file_exists(String path)
{
	fs::path target = _dataDir / path.utf8().get_data();

	if (fs::exists(target)) return true;

	for (auto bsa : _archives)
	{
		if (bsa->has_file(path)) return true;
	}

	return false;
}

PackedByteArray TESVResourceManager::get_file_raw(String path)
{
	PackedByteArray output;
	fs::path target = _dataDir / path.utf8().get_data();
	std::ifstream input(target, std::ios_base::binary);
	if (input.is_open())
	{
		std::streamoff fsize = 0;
		if (input.seekg(0, std::ios_base::end))
		{
			fsize = input.tellg();
			input.seekg(0);
		}
		output.resize(fsize);
		input.read(reinterpret_cast<char*>(output.ptrw()), fsize);

		return output;
	}

	for (auto bsa : _archives)
	{
		output = bsa->get_file(path);
		if (output.size() > 0) break;
	}

	return output;
}

Ref<Texture2D> TESVResourceManager::load_texture(String path)
{
	auto texture = TESVResourceCache::get_texture(path);
	if (texture.is_null())
	{
		texture = DDSLoader::load(get_file_raw(path));
		TESVResourceCache::add_texture(path, texture);
	}

	return texture;
}

Node3D* TESVResourceManager::load_mesh(String path)
{
	return NifLoader::load(get_file_raw(path));
}

void TESVResourceManager::_bind_methods() {
	ClassDB::bind_static_method("TESVResourceManager", D_METHOD("set_game_dir", "path"), &TESVResourceManager::set_game_dir);
	ClassDB::bind_static_method("TESVResourceManager", D_METHOD("add_archive", "path"), &TESVResourceManager::add_archive);
	ClassDB::bind_static_method("TESVResourceManager", D_METHOD("get_file_list"), &TESVResourceManager::get_file_list);
	ClassDB::bind_static_method("TESVResourceManager", D_METHOD("get_file_raw", "path"), &TESVResourceManager::get_file_raw);
	ClassDB::bind_static_method("TESVResourceManager", D_METHOD("file_exists", "path"), &TESVResourceManager::file_exists);
	ClassDB::bind_static_method("TESVResourceManager", D_METHOD("load_texture", "path"), &TESVResourceManager::load_texture);
	ClassDB::bind_static_method("TESVResourceManager", D_METHOD("load_mesh", "path"), &TESVResourceManager::load_mesh);
}