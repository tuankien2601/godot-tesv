#ifndef GD_DDS_LOADER
#define GD_DDS_LOADER

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <godot_cpp/classes/object.hpp>

namespace godot {

	class DDSLoader : public Object {
		GDCLASS(DDSLoader, Object)

	protected:
		static void _bind_methods();

	private:
		static bool _enable_compression;
		static Image::CompressMode _compression_mode;
	public:
		DDSLoader(){}
		~DDSLoader(){}

		static Ref<ImageTexture> load(PackedByteArray input);
		static void enable_compression(bool enabled);
	};

}

#endif