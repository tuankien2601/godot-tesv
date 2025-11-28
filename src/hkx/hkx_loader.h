#ifndef GD_HKX_LOADER
#define GD_HKX_LOADER

#include "godot_cpp/classes/animation.hpp"
#include <godot_cpp/classes/object.hpp>

namespace godot {

	class HKXLoader : public Object {
		GDCLASS(HKXLoader, Object)

	protected:
		static void _bind_methods();

	public:
		HKXLoader(){}
		~HKXLoader(){}

		static Ref<Animation> load_animation(PackedByteArray animation_data, PackedByteArray skeleton_data);
	};

}

#endif