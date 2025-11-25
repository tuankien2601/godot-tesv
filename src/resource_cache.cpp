#include "resource_cache.h"

using namespace godot;

std::unordered_map<StringName, Ref<ImageTexture>> TESVResourceCache::_texture_cache;
size_t TESVResourceCache::_capacity = 2000000000;
size_t TESVResourceCache::_used;

void TESVResourceCache::set_cache_size(size_t size)
{
	_capacity = size;
}

size_t TESVResourceCache::total_mem_used()
{
	return _used;
}

void TESVResourceCache::free_mem()
{
	_texture_cache.clear();
	_used = 0;
}

bool TESVResourceCache::add_texture(String path, Ref<ImageTexture> texture)
{
	auto size = texture->get_image()->get_data_size();
	if (_used + size > _capacity)
	{
		for (auto it = _texture_cache.begin(); it != _texture_cache.end(); it++)
		{
			if (it->second->get_reference_count() > 1) continue;
			_texture_cache.erase(it);
			_used -= it->second->get_image()->get_data_size();
			if (_used + size < _capacity) break;
		}
	}

	if (_used + size > _capacity) return false;

	_texture_cache[path] = texture;
	return true;
}

Ref<ImageTexture> TESVResourceCache::get_texture(String path)
{
	auto it = _texture_cache.find(path);
	if (it != _texture_cache.end()) {
		return it->second;
	}

	return Ref<ImageTexture>();
}

void TESVResourceCache::_bind_methods() {
	ClassDB::bind_static_method("TESVResourceCache", D_METHOD("set_cache_size", "size"), &TESVResourceCache::set_cache_size);
	ClassDB::bind_static_method("TESVResourceCache", D_METHOD("total_mem_used"), &TESVResourceCache::total_mem_used);
	ClassDB::bind_static_method("TESVResourceCache", D_METHOD("free_mem"), &TESVResourceCache::free_mem);
}