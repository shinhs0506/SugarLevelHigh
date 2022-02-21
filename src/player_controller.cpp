#include <iostream>

#include "player_controller.hpp"
#include "tiny_ecs_registry.hpp"
#include "level_init.hpp"
#include "physics_system.hpp"
#include "ability.hpp"

PlayerController::PlayerController()
{

}

void PlayerController::reset(Entity player)
{
	this->player = player;
	this->current_state = PlayerState::IDLE;
	this->next_state = PlayerState::IDLE;
}

void PlayerController::step(float elapsed_ms)
{
	// update states
	current_state = next_state;
}

void PlayerController::on_key(int key, int, int action, int mod)
{
	Motion& player_motion = registry.motions.get(player);
	switch (current_state)
	{	
	case PlayerState::IDLE:
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			switch (key)
			{
			case GLFW_KEY_A:
				player_motion.velocity = vec2(-player_motion.speed, 0);
				move_to_state(PlayerState::MOVE_LEFT); break;
			case GLFW_KEY_D:
				player_motion.velocity = vec2(player_motion.speed, 0);
				move_to_state(PlayerState::MOVE_RIGHT); break;
			case GLFW_KEY_W:
				// TODO: player not moving for up
				player_motion.velocity = vec2(0);
				move_to_state(PlayerState::MOVE_UP); break;
			case GLFW_KEY_S:
				// TODO: player not moving for down
				player_motion.velocity = vec2(0);
				move_to_state(PlayerState::MOVE_DOWN); break;
			}
		}
		break;

	case PlayerState::MOVE_LEFT:
		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		{
			player_motion.velocity = vec2(0);
			move_to_state(PlayerState::IDLE);
		}
		break;

	case PlayerState::MOVE_RIGHT:
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			player_motion.velocity = vec2(0);
			move_to_state(PlayerState::IDLE);
		}
		break;

	case PlayerState::MOVE_UP:
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			player_motion.velocity = vec2(0);
			move_to_state(PlayerState::IDLE);
		}
		break;

	case PlayerState::MOVE_DOWN:
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			player_motion.velocity = vec2(0);
			move_to_state(PlayerState::IDLE);
		}
		break;
	}
}

void PlayerController::on_mouse_button(int button, int action, int mod, vec2 cursor_world_pos)
{
	switch (current_state)
	{
	case PlayerState::IDLE:
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
					registry.clickables.get(entity).on_click();
					move_to_state(PlayerState::PERFORM_ABILITY);
				}
			}
		}
		break;

	case PlayerState::PERFORM_ABILITY:
		// player can use right click to cancel attack preview
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
		{
			move_to_state(PlayerState::IDLE);
			break;
		}
		// use left click for buttons or perform ability only
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		{
			// manually calculate a world position with some offsets
			vec2 player_pos = registry.motions.get(player).position;

			vec2 direction = cursor_world_pos - player_pos;

			vec2 offset{ 75.f, 0.f }; // a bit before the character

			perform_attack(offset, direction);

			move_to_state(PlayerState::END);

		}
		break;

	}
}

bool PlayerController::should_end_player_turn()
{
	return current_state == PlayerState::END;
}

void PlayerController::move_to_state(PlayerState next_state)
{
	switch (next_state) {
	case PlayerState::IDLE:
		std::cout << "moving to IDLE state" << std::endl;
		assert(current_state == PlayerState::MOVE_LEFT || current_state == PlayerState::MOVE_RIGHT ||
			current_state == PlayerState::MOVE_UP || current_state == PlayerState::MOVE_DOWN ||
			current_state == PlayerState::PERFORM_ABILITY);
		break;

	case PlayerState::MOVE_LEFT:
		std::cout << "moving to MOVE_LEFT state" << std::endl;
		assert(current_state == PlayerState::IDLE);
		break;

	case PlayerState::MOVE_RIGHT:
		std::cout << "moving to MOVE_RIGHT state" << std::endl;
		assert(current_state == PlayerState::IDLE);
		break;

	case PlayerState::MOVE_UP:
		std::cout << "moving to MOVE_UP state" << std::endl;
		assert(current_state == PlayerState::IDLE);
		break;

	case PlayerState::MOVE_DOWN:
		std::cout << "moving to MOVE_DOWN state" << std::endl;
		assert(current_state == PlayerState::IDLE);
		break;

	case PlayerState::PERFORM_ABILITY:
		std::cout << "moving to PERFORM_ABILITY state" << std::endl;
		assert(current_state == PlayerState::IDLE);
		break;

	case PlayerState::END:
		std::cout << "moving to END state" << std::endl;
		assert(current_state == PlayerState::PERFORM_ABILITY);
		break;
	}
	this->next_state = next_state;
}
