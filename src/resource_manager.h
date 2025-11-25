#ifndef TESV_RESOURCE_MANAGER
#define TESV_RESOURCE_MANAGER

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <filesystem>

namespace godot {
	class BSA;
	class TESVResourceManager : public Object {
		GDCLASS(TESVResourceManager, Object)


	protected:
		static std::filesystem::path _gameDir;
		static std::filesystem::path _dataDir;
		static Vector<Ref<BSA>> _archives;
		static void _bind_methods();

	public:
		static bool set_game_dir(String path);
		static bool add_archive(String path);
		static PackedStringArray get_file_list();
		static PackedByteArray get_file_raw(String path);
		static bool file_exists(String path);
		static Ref<Texture2D> load_texture(String path);
		static Node3D* load_mesh(String path);
	};

}

#endif