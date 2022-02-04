#pragma once

#include <vector>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include "level_manager.hpp"

class AISystem
{
public:
	void step(float elapsed_ms);
	void init(LevelManager* level_manager);

private:
	LevelManager* level_manager;
	// A small delay before AI moves to allow for player to see AI abit easier
	// and so that AI doesnt instantly do its turn causing player to miss it
	float beginning_delay_counter_ms = 1000.0f; // 1000ms = 1s

	void decision_Tree(Entity entity, AI& entity_AI);
	void end_Enemy_Turn(Energy& entity_energy, AI& entity_AI);
	void reset_Enemy(Energy& entity_energy, AI& entity_AI);
};