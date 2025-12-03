#ifndef HAVOK_BEHAVIOUR_SYSTEM_H
#define HAVOK_BEHAVIOUR_SYSTEM_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/animation_tree.hpp>
#include <godot_cpp/classes/animation_node_state_machine.hpp>
#include "havok.h"

namespace godot {

	class HavokBehaviourSystem : public Node {
		GDCLASS(HavokBehaviourSystem, Node)
	public:
		void load_behavior_graph(String file_path);
		void set_behavior_graph(hkbBehaviorGraph *graph);
		hkbBehaviorGraph *get_behavior_graph() const;
		void set_animation_player(AnimationPlayer *player);
		AnimationPlayer *get_animation_player() const;
		void set_animation_tree(AnimationTree *tree);
		AnimationTree *get_animation_tree() const;
	protected:
		static void _bind_methods();
		void _ready() override;

	private:
		hkbBehaviorGraph *behavior_graph = nullptr;
		AnimationPlayer *animation_player = nullptr;
		AnimationTree *animation_tree = nullptr;

		String get_anim(String path);
		Ref<AnimationRootNode> create_animation_node(hkbGenerator *generator, Ref<AnimationNodeStateMachine> parent);
	};

}

#endif