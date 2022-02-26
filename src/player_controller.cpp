#include <iostream>

#include "player_controller.hpp"
#include "tiny_ecs_registry.hpp"
#include "level_init.hpp"
#include "physics_system.hpp"
#include "ability.hpp"
#include "components.hpp"

PlayerController::PlayerController()
{
	current_state = CharacterState::END;
	next_state = CharacterState::END;
}

void PlayerController::start_turn(Entity player)
{
	this->player = player;

	this->current_state = CharacterState::IDLE;
	this->next_state = CharacterState::IDLE;
}

void PlayerController::step(float elapsed_ms)
{
	Motion& player_motion = registry.motions.get(player);
	Energy& player_energy = registry.energies.get(player);
	if (current_state == CharacterState::MOVE_LEFT || current_state == CharacterState::MOVE_RIGHT ||
		current_state == CharacterState::MOVE_UP || current_state == CharacterState::MOVE_DOWN) {
		if (player_energy.cur_energy > 0.f) {
			player_energy.prev_energy = player_energy.cur_energy;
			player_energy.cur_energy -= min(float(5 * elapsed_ms * 0.01), player_energy.cur_energy);
		}
	}
	updateEnergyBar(player_energy);
	updateHealthBar(player);

	// update states
	current_state = next_state;

}

void PlayerController::on_key(int key, int, int action, int mod)
{
	Motion& player_motion = registry.motions.get(player);
	Energy& player_energy = registry.energies.get(player);

	if (player_energy.cur_energy > 0.f) {
		switch (current_state)
		{
		case CharacterState::IDLE:
			if (action == GLFW_PRESS || action == GLFW_REPEAT)
			{
				switch (key)
				{
				case GLFW_KEY_A:
					if (registry.motions.get(player).location != LOCATION::ON_CLIMBABLE) {
						player_motion.velocity.x = -player_motion.speed;
						move_to_state(CharacterState::MOVE_LEFT);
					}
					break;
				case GLFW_KEY_D:
					if (registry.motions.get(player).location != LOCATION::ON_CLIMBABLE) {
						player_motion.velocity.x = player_motion.speed;
						move_to_state(CharacterState::MOVE_RIGHT);
					}
					break;
				case GLFW_KEY_W:
					if (registry.motions.get(player).location == BELOW_CLIMBABLE 
						|| registry.motions.get(player).location == ON_CLIMBABLE) {
						player_motion.velocity = vec2(0, -player_motion.speed);
						move_to_state(CharacterState::MOVE_UP);
					}
					break;
					
				case GLFW_KEY_S:
					if (registry.motions.get(player).location == ABOVE_CLIMBABLE
						|| registry.motions.get(player).location == ON_CLIMBABLE) {
						player_motion.velocity = vec2(0, player_motion.speed);
						move_to_state(CharacterState::MOVE_DOWN);
					}
					break;
				}
			}
			break;

		case CharacterState::MOVE_LEFT:
			if (key == GLFW_KEY_A && action == GLFW_RELEASE)
			{
				player_motion.velocity.x = 0;
				move_to_state(CharacterState::IDLE);
			}
			break;

		case CharacterState::MOVE_RIGHT:
			if (key == GLFW_KEY_D && action == GLFW_RELEASE)
			{
				player_motion.velocity.x = 0;
				move_to_state(CharacterState::IDLE);
			}
			break;

		case CharacterState::MOVE_UP:
			if (key == GLFW_KEY_W && action == GLFW_RELEASE)
			{
				player_motion.velocity = vec2(0);
				move_to_state(CharacterState::IDLE);
			}
			break;

		case CharacterState::MOVE_DOWN:
			if (key == GLFW_KEY_S && action == GLFW_RELEASE)
			{
				player_motion.velocity = vec2(0);
				move_to_state(CharacterState::IDLE);
			}
		}
	} else {
		if (current_state == CharacterState::MOVE_LEFT || current_state == CharacterState::MOVE_RIGHT ||
			current_state == CharacterState::MOVE_UP || current_state == CharacterState::MOVE_DOWN) {
			player_motion.velocity = vec2(0);
			move_to_state(CharacterState::IDLE);
		}
	}
}

void PlayerController::on_mouse_move(vec2 cursor_pos) {
	// Update attack preview to the correct angles/positions
	// Note it's possible that the attack preview has been destroyed in PERFORM_ABILITY state
	if (current_state == CharacterState::PERFORM_ABILITY && registry.attackPreviews.size() > 0) { 
		Entity attack_preview = registry.attackPreviews.entities[0];
		Motion& attack_preview_motion = registry.motions.get(attack_preview);

		vec2 direction = cursor_pos - player_pos;
		double angle = -atan2(direction[0], direction[1]) + M_PI / 2;

		vec2 attack_preview_pos = offset_position(direction, player_pos, angle);

		attack_preview_motion.position = attack_preview_pos;
		attack_preview_motion.angle = angle;
	}
}

void PlayerController::on_mouse_button(int button, int action, int mod, vec2 cursor_world_pos)
{
	switch (current_state)
	{
	case CharacterState::IDLE:
		// click on attack action to go to PLAYER_ATTACK state
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
			Motion click_motion;
			click_motion.position = cursor_world_pos;
			click_motion.scale = { 1.f, 1.f };

			// check to see if click was on a button first
			for (uint i = 0; i < registry.clickables.size(); i++) {

				Entity entity = registry.clickables.entities[i];
				Motion motion = registry.motions.get(entity);

				if (collides(click_motion, motion)) {
					bool ability_successfully_chosen = registry.clickables.get(entity).on_click();
					if (ability_successfully_chosen) {
						// Get player position at time of choosing an ability successfully
						player_pos = registry.motions.get(player).position;
						// Create preview object
						create_preview_object(player_pos);

						move_to_state(CharacterState::PERFORM_ABILITY);
					}
				}
			}
		}
		break;

	case CharacterState::PERFORM_ABILITY:

		// player can use right click to cancel attack preview
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
		{
			destroy_preview_objects();
			move_to_state(CharacterState::IDLE);
			break;
		}
		// use left click for buttons or perform ability only
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		{
			AttackArsenal& active_arsenal = registry.attackArsenals.get(player);
			AttackAbility& chosen_attack = (active_arsenal.basic_attack.activated == true) ? active_arsenal.basic_attack : active_arsenal.advanced_attack;

		
			// manually calculate a world position with some offsets
			vec2 direction = cursor_world_pos - player_pos;
			vec2 offset{ 75.f, 0.f }; // a bit before the character

		    perform_attack(player, player_pos, offset, direction, chosen_attack); 
			chosen_attack.current_cooldown = chosen_attack.max_cooldown;

			destroy_preview_objects();
			
			// reset player's enegy
			Energy& energy = registry.energies.get(player);
			energy.cur_energy = energy.max_energy;

			move_to_state(CharacterState::END);
		}
		break;

	}
}

bool PlayerController::should_end_player_turn()
{
	return current_state == CharacterState::END;
}

void PlayerController::move_to_state(CharacterState next_state)
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
		assert(current_state == CharacterState::PERFORM_ABILITY);
		break;
	}
	this->next_state = next_state;
}

