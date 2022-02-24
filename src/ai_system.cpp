// internal
#include "ai_system.hpp"
#include "level_init.hpp"
#include "ability.hpp"

void AISystem::init(LevelManager* level_manager)
{
	this->level_manager = level_manager;
}


void AISystem::reset_Enemy(Energy& entity_energy, AI& entity_AI) {
	beginning_delay_counter_ms = 1000.0f;
	entity_energy.cur_energy = 100.0f;

}

void enemy_Attack(Entity enemy) {


	// TODO: Find closest player or lowest health player and choose appropriate attack based on some logic and cooldowns

	AttackArsenal& active_arsenal = registry.attackArsenals.get(enemy);
	AttackAbility& chosen_attack = (active_arsenal.basic_attack.activated == true) ? active_arsenal.basic_attack : active_arsenal.advanced_attack;

	vec2 enemy_pos = registry.motions.get(enemy).position;
	vec2 direction = vec2(-1,0);// Attacks left for now
	vec2 offset{ 75.f, 0.f }; // a bit before the character

	perform_attack(enemy, enemy_pos, offset, direction, chosen_attack);
	chosen_attack.current_cooldown = chosen_attack.max_cooldown;

	// Reduce all cooldowns by 1 that are not already 0.
	if (active_arsenal.basic_attack.current_cooldown > 0) {
		active_arsenal.basic_attack.current_cooldown -= 1;
	}
	if (active_arsenal.advanced_attack.current_cooldown > 0) {
		active_arsenal.advanced_attack.current_cooldown -= 1;
	}
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

	updateHealthBar(entity);
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