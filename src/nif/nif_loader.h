#ifndef GD_NIF_LOADER
#define GD_NIF_LOADER

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/object.hpp>

namespace godot {

	class NifLoader : public Object {
		GDCLASS(NifLoader, Object)


	protected:
		static void _bind_methods();

	public:
		static Node3D* load(PackedByteArray data);
		static Node3D* load_skinned(PackedByteArray mesh, PackedByteArray skeleton);
		static void load_skeleton(Node3D *mesh, PackedByteArray skeleton);
	};
}

#endif