// internal
#include "ai_system.hpp"
#include "level_init.hpp"

void AISystem::init(LevelManager* level_manager)
{
	this->level_manager = level_manager;
}


void AISystem::reset_Enemy(Energy& entity_energy, AI& entity_AI) {
	beginning_delay_counter_ms = 1000.0f;
	entity_energy.cur_energy = 100.0f;

}

void enemy_Attack(Entity entity) {

	Entity enemy = registry.activeTurns.entities[0];

	// manually calculate a world position with some offsets
	vec2 player_pos = registry.motions.get(enemy).position;

	vec2 offset{ -75.f, 0.f }; // a bit before the character
	Transform trans;
	trans.translate(player_pos);
	trans.translate(offset);

	vec2 attack_pos = trans.mat * vec3(0, 0, 1);
	createAttackObject(enemy, GEOMETRY_BUFFER_ID::SQUARE, 50.f, 200, 0, attack_pos, vec2(0, 0), vec2(100, 100), false);

}

void AISystem::end_Enemy_Turn(Energy& entity_energy, AI& entity_AI) {
	
	reset_Enemy(entity_energy, entity_AI);
	level_manager->move_to_state(LevelManager::LevelState::EVALUATION);
}

void AISystem::decision_Tree(Entity entity, AI& entity_AI) {

	Energy& entity_energy = registry.energies.get(entity);
	Motion& entity_motion = registry.motions.get(entity);
	//AI& entity_AI = registry.AIs.get(entity);

	if (level_manager->current_state() == LevelManager::LevelState::ENEMY_MOVE) {
		if (entity_energy.cur_energy > 0) {
			// Has energy left then do some sort of movement

			if ((int)entity_energy.cur_energy % 20 == 0) {
				int random_direction = (rand() > RAND_MAX / 2) ? -1 : 1;
				entity_AI.movement_direction.x *= random_direction;
			}
			// For real random direction, use above. To make enemy just move left, use line below.
			// int random_direction = -1;


			//Only left and right ai movement for now
			entity_motion.velocity = vec2(entity_motion.speed * entity_AI.movement_direction.x, 0);
			entity_energy.cur_energy -= 1.0f;
		}
		else {
			// Reset velocities
			entity_motion.velocity = vec2(0, 0);
			level_manager->move_to_state(LevelManager::LevelState::ENEMY_ATTACK);
		}
	}
	else if (level_manager->current_state() == LevelManager::LevelState::ENEMY_ATTACK) {
		enemy_Attack(entity);
		end_Enemy_Turn(entity_energy, entity_AI);
	}
}

void AISystem::step(float elapsed_ms)
{

	if (level_manager->current_state() == LevelManager::LevelState::ENEMY_MOVE ||
		level_manager->current_state() == LevelManager::LevelState::ENEMY_ATTACK) {
		Entity active_entity = registry.activeTurns.entities[0];
		AI& entity_AI = registry.AIs.get(active_entity);
		// A small delay before AI moves to allow for player to see AI abit easier
		// and so that AI doesnt instantly do its turn causing player to miss it
		if (beginning_delay_counter_ms > 0) {
			beginning_delay_counter_ms -= elapsed_ms;
			return;
		}
		decision_Tree(active_entity, entity_AI);

	}
	(void)elapsed_ms; // placeholder to silence unused warning until implemented
}