#include "havok_behaviour_system.h"
#include <godot_cpp/classes/animation_node_animation.hpp>
#include <godot_cpp/classes/animation_node_blend_space1_d.hpp>
#include <godot_cpp/classes/animation_node_blend_space2d.hpp>
#include <godot_cpp/classes/animation_library.hpp>
#include <godot_cpp/classes/resource_loader.hpp>

using namespace godot;

void HavokBehaviourSystem::load_behavior_graph(String file_path)
{
	// Placeholder for loading behavior graph from file
	// Actual implementation would involve reading the file and parsing it into hkbBehaviorGraph
}

void HavokBehaviourSystem::set_behavior_graph(hkbBehaviorGraph *graph)
{
	behavior_graph = graph;
}

void HavokBehaviourSystem::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("load_behavior_graph", "file_path"), &HavokBehaviourSystem::load_behavior_graph);
	ClassDB::bind_method(D_METHOD("set_animation_player", "player"), &HavokBehaviourSystem::set_animation_player);
	ClassDB::bind_method(D_METHOD("get_animation_player"), &HavokBehaviourSystem::get_animation_player);
	ClassDB::bind_method(D_METHOD("set_animation_tree", "tree"), &HavokBehaviourSystem::set_animation_tree);
	ClassDB::bind_method(D_METHOD("get_animation_tree"), &HavokBehaviourSystem::get_animation_tree);
	ADD_PROPERTY(PropertyInfo(Variant::Type::OBJECT, "animation_player", PROPERTY_HINT_RESOURCE_TYPE, "AnimationPlayer"), "set_animation_player", "get_animation_player");
	ADD_PROPERTY(PropertyInfo(Variant::Type::OBJECT, "animation_tree", PROPERTY_HINT_RESOURCE_TYPE, "AnimationTree"), "set_animation_tree", "get_animation_tree");
}

String HavokBehaviourSystem::get_anim(String path)
{
	String anim_name = path.replace("hkx", "").replace("animations\\", "");

	if (animation_player->has_animation(anim_name))
	{
		return anim_name;
	}

	Ref<Animation> animation = ResourceLoader::get_singleton()->load(path);
	if (animation.is_valid())
	{
		animation_player->get_animation_library("")->add_animation(anim_name, animation);
		return anim_name;
	}
	return "";
}

Ref<AnimationRootNode> HavokBehaviourSystem::create_animation_node(hkbGenerator *generator, Ref<AnimationNodeStateMachine> parent)
{
	if (auto havokStateMachine = dynamic_cast<hkbStateMachine *>(generator))
	{
		Ref<AnimationNodeStateMachine> state_machine;
		state_machine.instantiate();

		if (parent.is_valid())
		{
			parent->add_node(havokStateMachine->m_name, state_machine);
		}

		for (auto havokState : havokStateMachine->m_states)
		{
			auto state = create_animation_node(havokState->m_generator, state_machine);
		}

		return state_machine;
	}
	else if (auto behaviourGraph = dynamic_cast<hkbBehaviorGraph *>(generator))
	{
		return create_animation_node(behaviourGraph->m_rootGenerator, parent);
	}
	else if (auto blendGenerator = dynamic_cast<hkbBlenderGenerator *>(generator))
	{
		Ref<AnimationNodeBlendSpace1D> blend_space;
		blend_space.instantiate();
		if (parent.is_valid())
		{
			parent->add_node(blendGenerator->m_name, blend_space);
		}

		blend_space->set_max_space(blendGenerator->m_maxCyclicBlendParameter);
		blend_space->set_min_space(blendGenerator->m_minCyclicBlendParameter);

		for (auto blendChild : blendGenerator->m_children)
		{
			auto blendChildNode = create_animation_node(blendChild->m_generator, Ref<AnimationNodeStateMachine>());
			blend_space->add_blend_point(blendChildNode, blendChild->m_weight);
		}

		return blend_space;
	}
	else if (auto clipGenerator = dynamic_cast<hkbClipGenerator *>(generator))
	{
		Ref<AnimationNodeAnimation> animation_node;
		animation_node.instantiate();
		animation_node->set_animation(get_anim(clipGenerator->m_animationName));
		if (parent.is_valid())
		{
			parent->add_node(generator->m_name, animation_node);
		}
		return animation_node;
	} else if (auto manualSelectorGenerator = dynamic_cast<hkbManualSelectorGenerator *>(generator)) {
		Ref<AnimationNodeStateMachine> state_machine;
		state_machine.instantiate();
		if (parent.is_valid())
		{
			parent->add_node(manualSelectorGenerator->m_name, state_machine);
		}

		for (auto generator : manualSelectorGenerator->m_generators)
		{
			auto childNode = create_animation_node(generator, state_machine);
		}

		return state_machine;
	} else if (auto modifierGenerator = dynamic_cast<hkbModifierGenerator *>(generator)) {
		return create_animation_node(modifierGenerator->m_generator, parent);
	} else if (auto positionRelativeSelectorGenerator = dynamic_cast<hkbPositionRelativeSelectorGenerator *>(generator)) {
		return create_animation_node(positionRelativeSelectorGenerator->m_blendToFixPositionGenerator, parent);	
	} else if (auto boneSwitchGenerator = dynamic_cast<BSBoneSwitchGenerator *>(generator)) {
		return create_animation_node(boneSwitchGenerator->m_pDefaultGenerator, parent);	
	} else if (auto cyclicBlendTransitionGenerator = dynamic_cast<BSCyclicBlendTransitionGenerator *>(generator)) {
		return create_animation_node(cyclicBlendTransitionGenerator->m_pBlenderGenerator, parent);	
	} else if (auto offsetAnimationGenerator = dynamic_cast<BSOffsetAnimationGenerator *>(generator)) {
		Ref<AnimationNodeStateMachine> state_machine;
		state_machine.instantiate();
		if (parent.is_valid())
		{
			parent->add_node(offsetAnimationGenerator->m_name, state_machine);
		}

		create_animation_node(offsetAnimationGenerator->m_pDefaultGenerator, state_machine);
		create_animation_node(offsetAnimationGenerator->m_pOffsetClipGenerator, state_machine);
		return state_machine;
	} else if (auto synchronizedClipGenerator = dynamic_cast<BSSynchronizedClipGenerator *>(generator)) {
		return create_animation_node(synchronizedClipGenerator->m_pClipGenerator, parent);
	} else if (auto iStateTaggingGenerator = dynamic_cast<BSiStateTaggingGenerator *>(generator)) {
		return create_animation_node(iStateTaggingGenerator->m_pDefaultGenerator, parent);
	} else {
		UtilityFunctions::print("Unsupported generator type: " + String(typeid(*generator).name()));
		return Ref<AnimationRootNode>();
	}
}

void HavokBehaviourSystem::_ready()
{
	if (animation_player && animation_tree)
	{
		Ref<AnimationLibrary> anim_library = animation_player->get_animation_library("");

		if (anim_library.is_null())
		{
			UtilityFunctions::print("Default AnimationLibrary not found. Creating one.");
			anim_library.instantiate();
			animation_player->add_animation_library("", anim_library);
		}

		auto rootGenerator = behavior_graph->m_rootGenerator;
		if (dynamic_cast<hkbStateMachine *>(rootGenerator))
		{
		}
		else
		{
		}
	}
}