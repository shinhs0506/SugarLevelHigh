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
	const float ENEMY_MOVEMENT_SPEED = 100.0f;
	LevelManager* level_manager;

	void decisionTree(Entity entity, AI& entity_AI);
	void endEnemyTurn(Energy& entity_energy, AI& entity_AI);
};