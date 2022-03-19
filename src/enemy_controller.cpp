#include <iostream>
#include <cfloat>

#include "enemy_controller.hpp"
#include "level_init.hpp"
#include "ability.hpp"



EnemyController::EnemyController()
{
	current_state = CharacterState::END;
	next_state = CharacterState::END;
}

void EnemyController::start_turn(Entity enemy)
{
	current_state = CharacterState::IDLE;
	next_state = CharacterState::IDLE;
	this->enemy = enemy;

	beginning_delay_counter_ms = DEFAULT_BEGINNING_DELAY;
}

bool EnemyController::should_end_enemy_turn()
{
	return current_state == CharacterState::END;
}

// don't have vertical movement for now
void EnemyController::move(Motion& motion, DIRECTION direction, float distance) {
	move_counter = distance / motion.speed * 1000;

	if (direction == DIRECTION_LEFT) {
		move_to_state(CharacterState::MOVE_LEFT);
		motion.goal_velocity.x = -1 * motion.speed;
	} 
	else if (direction == DIRECTION_RIGHT) {
		move_to_state(CharacterState::MOVE_RIGHT);
		motion.goal_velocity.x = 1 * motion.speed;
	}
	else if (direction == DIRECTION_UP) {
		move_to_state(CharacterState::MOVE_UP);
		motion.goal_velocity.y = -1 * motion.speed;
	}
	else if (direction == DIRECTION_DOWN) {
		move_to_state(CharacterState::MOVE_DOWN);
		motion.goal_velocity.y = 1 * motion.speed;
	}
}

float EnemyController::cal_actual_attack_range(AttackAbility& ability) {
	// add some small offset
	return ability.range + max(ability.size.x, ability.size.y);
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
		float dist = max(cal_actual_attack_range(chosen_attack) - min_dist - 10, 0.f);
		// only move left/right now
		move(motion, motion.position.x < target_motion.position.x ? DIRECTION_LEFT : DIRECTION_RIGHT, dist);
		return;
	}

	// add a small amount of offset because attacks are generated with offset
	if (within_attack_range(min_dist, chosen_attack) && motion.location != ON_CLIMBABLE) {
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
	// try to climb ladder when possible and the target is not at the same level
	// target above enemy
	else if (motion.position.y - target_motion.position.y > 0 &&
		(motion.location == BELOW_CLIMBABLE || motion.location == ON_CLIMBABLE)) {
		move(motion, DIRECTION_UP, 10); // some arbitrary distance
	}
	// target below enemy
	// disable climing down for possible visual bugs
	//else if (target_motion.position.y - motion.position.y > 0 &&
	//	(motion.location == ABOVE_CLIMBABLE || motion.location == ON_CLIMBABLE)) {
	//	move(motion, DIRECTION_DOWN, 10); // some arbitrary distance
	//} 
	// move horizontally
	else {
		// move distance is calculated only on x-axis
		float x_dist = abs(target_motion.position.x - motion.position.x);
		float dist = abs(x_dist - cal_actual_attack_range(chosen_attack));
		// only move left/right now
		move(motion, motion.position.x < target_motion.position.x ? DIRECTION_RIGHT : DIRECTION_LEFT, dist);
	}
}

void EnemyController::step(float elapsed_ms)
{
	// update state
	current_state = next_state;

	Energy& energy = registry.energies.get(enemy);
	Motion& motion = registry.motions.get(enemy);

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
			//motion.goal_velocity.y = 0;
			move_to_state(CharacterState::IDLE);
		}
		break;

	case CharacterState::END:
		break;
	}

	updateEnergyBar(energy);
	updateHealthBar(enemy);
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
