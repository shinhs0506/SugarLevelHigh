#pragma once
#include <vector>

#include "tiny_ecs.hpp"
#include "components.hpp"

class ECSRegistry
{
	// Callbacks to remove a particular or all entities in the system
	std::vector<ContainerInterface*> registry_list;

public:
	// Manually created list of all components this game has
	// TODO: A1 add a LightUp component
	ComponentContainer<Playable> playables;
	ComponentContainer<Enemy> enemies;
	ComponentContainer<Health> healths;
	ComponentContainer<Energy> energies;
	ComponentContainer<Initiative> initiatives;
    ComponentContainer<ActiveTurn> activeTurns;
	ComponentContainer<AttackAbility> attackAbilities;
	ComponentContainer<AttackObject> attackObjects;
	ComponentContainer<Projectile> projectiles;
	ComponentContainer<Terrain> terrains;
	ComponentContainer<Motion> motions;
	ComponentContainer<Collision> collisions;
	ComponentContainer<Mesh*> meshPtrs;
	ComponentContainer<RenderRequest> renderRequests;
	ComponentContainer<ScreenState> screenStates;
	ComponentContainer<DebugComponent> debugComponents;
	ComponentContainer<vec3> colors;
	ComponentContainer<Camera> cameras;
	ComponentContainer<Clickable> clickables;
	ComponentContainer<Overlay> overlays;
	ComponentContainer<HitEffect> hitEffects;
	ComponentContainer<AI> AIs;
  ComponentContainer<Background> backgrounds;

	// constructor that adds all containers for looping over them
	// IMPORTANT: Don't forget to add any newly added containers!
	ECSRegistry()
	{
		// TODO: A1 add a LightUp component
		registry_list.push_back(&playables);
		registry_list.push_back(&enemies);
		registry_list.push_back(&healths);
		registry_list.push_back(&energies);
		registry_list.push_back(&initiatives);
        registry_list.push_back(&activeTurns);
		registry_list.push_back(&attackAbilities);
		registry_list.push_back(&attackObjects);
		registry_list.push_back(&projectiles);
		registry_list.push_back(&terrains);
		registry_list.push_back(&motions);
		registry_list.push_back(&collisions);
		registry_list.push_back(&meshPtrs);
		registry_list.push_back(&renderRequests);
		registry_list.push_back(&screenStates);
		registry_list.push_back(&debugComponents);
		registry_list.push_back(&colors);
		registry_list.push_back(&cameras);
		registry_list.push_back(&clickables);
		registry_list.push_back(&overlays);
		registry_list.push_back(&hitEffects);
		registry_list.push_back(&AIs);
		registry_list.push_back(&backgrounds);
	}

	void clear_all_components() {
		for (ContainerInterface* reg : registry_list)
			reg->clear();
	}

	void list_all_components() {
		printf("Debug info on all registry entries:\n");
		for (ContainerInterface* reg : registry_list)
			if (reg->size() > 0)
				printf("%4d components of type %s\n", (int)reg->size(), typeid(*reg).name());
	}

	void list_all_components_of(Entity e) {
		printf("Debug info on components of entity %u:\n", (unsigned int)e);
		for (ContainerInterface* reg : registry_list)
			if (reg->has(e))
				printf("type %s\n", typeid(*reg).name());
	}

	void remove_all_components_of(Entity e) {
		for (ContainerInterface* reg : registry_list)
			reg->remove(e);
	}
};

extern ECSRegistry registry;
