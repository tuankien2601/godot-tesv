#ifndef TESV_RESOURCE_CACHE
#define TESV_RESOURCE_CACHE

#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace std {
// Specialize std::hash for godot::StringName
template <> struct hash<godot::StringName> {
  std::size_t operator()(const godot::StringName &sn) const noexcept {
    // StringName has a built-in hash method that returns a uint32_t/uint64_t.
    // Cast or convert it to std::size_t as required by std::hash.
    return static_cast<std::size_t>(sn.hash());
  }
};
} // namespace std

namespace godot {
class TESVResourceCache : public Object {
  GDCLASS(TESVResourceCache, Object)

protected:
  static std::unordered_map<StringName, Ref<ImageTexture>> _texture_cache;
  static size_t _capacity;
  static size_t _used;
  static void _bind_methods();

public:
  static void set_cache_size(size_t size);
  static size_t total_mem_used();
  static void free_mem();
  static bool add_texture(String path, Ref<ImageTexture> texture);
  static Ref<ImageTexture> get_texture(String path);
};

} // namespace godot

#endif