#include <iostream>
#include <cfloat>

#include "enemy_controller.hpp"
#include "level_init.hpp"
#include "ability.hpp"
#include "level_manager.hpp"

EnemyController::EnemyController()
{
	current_state = CharacterState::END;
	next_state = CharacterState::END;
}

void EnemyController::start_turn(Entity enemy)
{
	this->enemy = enemy;

	Health& player_health = registry.healths.get(enemy);
	std::cout << "over turn " << player_health.damage_per_turn << std::endl;
	if (player_health.damage_per_turn == true) {
		player_health.cur_health -= 5;
		LevelManager::update_healthbar_len_color(enemy);
		createHitEffect(enemy, 200);
		if (player_health.cur_health < 0) {
			player_health.cur_health = 0;
		}
	}

	current_state = CharacterState::IDLE;
	next_state = CharacterState::IDLE;

	beginning_delay_counter_ms = DEFAULT_BEGINNING_DELAY;
}

bool EnemyController::should_end_enemy_turn()
{
	return current_state == CharacterState::END;
}

// don't have vertical movement for now
// direction: -1: left; 1: right
void EnemyController::move(Motion& motion, int direction, float distance) {
	move_counter = distance / motion.speed * 1000;
	motion.goal_velocity.x = direction * motion.speed;
	if (direction == -1) {
		move_to_state(CharacterState::MOVE_LEFT);
	}
	else {
		move_to_state(CharacterState::MOVE_RIGHT);
	}

}

float EnemyController::cal_actual_attack_range(AttackAbility& ability) {
	// add some small offset
	return ability.range + min(ability.size.x, ability.size.y) / 2 + 20;
}

bool EnemyController::within_attack_range(float dist, AttackAbility& ability) {
	return dist < cal_actual_attack_range(ability);
}

void EnemyController::make_decision() {
	Health& health = registry.healths.get(enemy);
	Energy& energy = registry.energies.get(enemy);
	Motion& motion = registry.motions.get(enemy);
	
	// find the closest player
	Entity target;
	Motion target_motion;
	float min_dist = FLT_MAX;
	for (int i = 0; i < registry.playables.size(); i++) {
		Entity player = registry.playables.entities[i];
		Motion& player_motion = registry.motions.get(player);

		float dist = distance(player_motion.position, motion.position);
		if (dist < min_dist) {
			min_dist = dist;
			target = player;
			target_motion = player_motion;
		}
	}

	// select a best attack
	AttackArsenal& arsenal = registry.attackArsenals.get(enemy);
	AttackAbility& chosen_attack = advanced_attack_available(arsenal) ? 
		arsenal.advanced_attack : arsenal.basic_attack;

	// if the enemy is low health, try to move away from character and keep distance of ranged attack
	// the magic 10 is to ensure the enemy can attack within range
	if (health.cur_health < 40.f && min_dist < cal_actual_attack_range(chosen_attack) - 10 && energy.cur_energy > 0.f) {
		float dist = cal_actual_attack_range(chosen_attack) - 10 - min_dist;
		// only move left/right now
		move(motion, motion.position.x < target_motion.position.x ? -1 : 1, dist);
		return;
	}

	// add a small amount of offset because attacks are generated with offset
	if (within_attack_range(min_dist, chosen_attack)) {
		vec2 direction = target_motion.position - motion.position;// Attacks left for now
		vec2 offset{ 75.f, 0.f }; // a bit before the character

		perform_attack(enemy, motion.position, offset, direction, chosen_attack);
		chosen_attack.current_cooldown = chosen_attack.max_cooldown;

		move_to_state(CharacterState::END);
	}
	// failed to move within range
	else if (energy.cur_energy == 0.f) {
		move_to_state(CharacterState::END);
	}
	else {
		// move distance is calculated only on x-axis
		float x_dist = abs(target_motion.position.x - motion.position.x);
		float dist = abs(x_dist - cal_actual_attack_range(chosen_attack));
		// only move left/right now
		move(motion, motion.position.x < target_motion.position.x ? 1 : -1, dist);
	}
}

void EnemyController::step(float elapsed_ms)
{
	// update state
	current_state = next_state;

	// For Level 3 damage over time hit effect
	for (uint i = 0; i < registry.hitEffects.size(); i++) {
		Entity entity = registry.hitEffects.entities[i];
		HitEffect& effect = registry.hitEffects.components[i];
		effect.ttl_ms -= elapsed_ms;
		if (effect.ttl_ms < 0) {
			removeHitEffect(entity);

			// only set dead after hit effect played 
			if (registry.healths.has(entity) && registry.healths.get(entity).cur_health < epsilon) {
				registry.healths.get(entity).dead = true;
				move_to_state(CharacterState::END);
			}
		}
	}

	// Enemy fell off the map (ie died on its turn)
	if (current_state == CharacterState::END) {
		return;
	}

	Energy& energy = registry.energies.get(enemy);
	Motion& motion = registry.motions.get(enemy);
	Health& health = registry.healths.get(enemy);

	// Check if enemy fell off the map
	if (motion.position.y > 900) {
		health.dead = true;
		move_to_state(CharacterState::END);
	}

	switch (current_state) {
	case CharacterState::IDLE:
		// A small delay before AI moves to allow for player to see AI abit easier
		// and so that AI doesnt instantly do its turn causing player to miss it
		if (beginning_delay_counter_ms > 0) {
			beginning_delay_counter_ms -= elapsed_ms;
			break;
		}
		make_decision();
		break;

	case CharacterState::MOVE_LEFT:
	case CharacterState::MOVE_RIGHT:
	case CharacterState::MOVE_UP:
	case CharacterState::MOVE_DOWN:
		move_counter -= elapsed_ms;
		// deduct energy while moving
		if (energy.cur_energy > 0.f) {
			energy.cur_energy -= min(float(5 * elapsed_ms * 0.01), energy.cur_energy);
		}

		if (move_counter < 0.f || energy.cur_energy == 0.f) {
			move_counter = 0.f;
			motion.goal_velocity.x = 0;
			move_to_state(CharacterState::IDLE);
		}
		break;

	case CharacterState::END:
		break;
	}

	if (health.dead == false) {
		updateEnergyBar(energy);
		updateHealthBar(enemy);
	}
}

void EnemyController::move_to_state(CharacterState next_state)
{
	switch (next_state) {
	case CharacterState::IDLE:
		std::cout << "moving to IDLE state" << std::endl;
		assert(current_state == CharacterState::MOVE_LEFT || current_state == CharacterState::MOVE_RIGHT ||
			current_state == CharacterState::MOVE_UP || current_state == CharacterState::MOVE_DOWN ||
			current_state == CharacterState::PERFORM_ABILITY);
		break;

	case CharacterState::MOVE_LEFT:
		std::cout << "moving to MOVE_LEFT state" << std::endl;
		assert(current_state == CharacterState::IDLE);
		break;

	case CharacterState::MOVE_RIGHT:
		std::cout << "moving to MOVE_RIGHT state" << std::endl;
		assert(current_state == CharacterState::IDLE);
		break;

	case CharacterState::MOVE_UP:
		std::cout << "moving to MOVE_UP state" << std::endl;
		assert(current_state == CharacterState::IDLE);
		break;

	case CharacterState::MOVE_DOWN:
		std::cout << "moving to MOVE_DOWN state" << std::endl;
		assert(current_state == CharacterState::IDLE);
		break;

	case CharacterState::PERFORM_ABILITY:
		std::cout << "moving to PERFORM_ABILITY state" << std::endl;
		assert(current_state == CharacterState::IDLE);
		break;

	case CharacterState::END:
		std::cout << "moving to END state" << std::endl;
		break;
	}
	this->next_state = next_state;
}
