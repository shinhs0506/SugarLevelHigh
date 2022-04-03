#include <iostream>

#include "player_controller.hpp"
#include "tiny_ecs_registry.hpp"
#include "level_init.hpp"
#include "physics_system.hpp"
#include "ability.hpp"
#include "components.hpp"
#include "level_manager.hpp"
#include "camera_manager.hpp"

PlayerController::PlayerController()
{
	current_state = CharacterState::END;
	next_state = CharacterState::END;


	SDL_Init(SDL_INIT_AUDIO);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	melee_attack_sound = Mix_LoadWAV(audio_path("melee_attack.wav").c_str());
	advanced_attack_sound = Mix_LoadWAV(audio_path("advanced_attack.wav").c_str());
	heal_ability_sound = Mix_LoadWAV(audio_path("healing_ability.wav").c_str());
}

PlayerController::~PlayerController()
{
	if (melee_attack_sound != nullptr)
		Mix_FreeChunk(melee_attack_sound);
	if (advanced_attack_sound != nullptr)
		Mix_FreeChunk(advanced_attack_sound);
	Mix_CloseAudio();
}

void PlayerController::start_turn(Entity player, int curr_level)
{
	this->player = player;

	// For level 3 damage over turn 
	Health& player_health = registry.healths.get(player);
	std::cout << "over turn " << player_health.damage_per_turn << std::endl;
	if (player_health.damage_per_turn == true) {
		player_health.cur_health -= 5;
		LevelManager::update_healthbar_len_color(player);
		createHitEffect(player, 200);
		if (player_health.cur_health < 0) {
			player_health.cur_health = 0;
		}
	}

	this->current_state = CharacterState::IDLE;
	this->next_state = CharacterState::IDLE;

	Entity advanced_attack_clickable;
	if (curr_level == 0) {
		advanced_attack_clickable = registry.clickables.entities[2];
	}
	else {
		advanced_attack_clickable = registry.clickables.entities[3];
		Entity healing_clickable = registry.clickables.entities[0];

		if (registry.abilityButtons.size() > 0) {
			if (registry.buffArsenals.get(player).heal.current_cooldown != 0) {
				registry.clickables.get(healing_clickable).on_cooldown = true;
			}
			else {
				registry.clickables.get(healing_clickable).on_cooldown = false;
			}
		}
	}

	if (registry.attackArsenals.get(player).advanced_attack.current_cooldown != 0) {
		registry.clickables.get(advanced_attack_clickable).on_cooldown = true;
	}
	else {
		registry.clickables.get(advanced_attack_clickable).on_cooldown = false;
	}

	Motion& player_motion = registry.motions.get(player);
	Motion& camera_motion = registry.motions.get(registry.cameras.entities[0]);
	camera_motion.scale = { window_width_px , window_height_px };
	if (!collides(camera_motion, player_motion)) {
		update_camera_pos(player_motion.position);
	}
}

void PlayerController::step(float elapsed_ms)
{
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

	Motion& player_motion = registry.motions.get(player);
	Energy& player_energy = registry.energies.get(player);
	if (current_state == CharacterState::MOVE_LEFT || current_state == CharacterState::MOVE_RIGHT ||
		current_state == CharacterState::MOVE_UP || current_state == CharacterState::MOVE_DOWN) {
		if (player_energy.cur_energy > 0.f) {
			player_energy.prev_energy = player_energy.cur_energy;
			player_energy.cur_energy -= min(float(5 * elapsed_ms * 0.01), player_energy.cur_energy);
		}
	}

    if (current_state == CharacterState::PERFORM_ABILITY_AUTO) {
        perform_buff_ability(player);
		// Play healing sound
		Mix_PlayChannel(-1, heal_ability_sound, 0);
        move_to_state(CharacterState::END);
    }

	updateEnergyBar(player_energy);
	updateHealthBar(player);

	if (player_motion.position.y > 1100) {
		Health& player_health = registry.healths.get(player);
		player_health.dead = true;
		move_to_state(CharacterState::END);
	}
	
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
					player_motion.goal_velocity.x = -player_motion.speed;
					if (registry.motions.get(player).location == LOCATION::ON_CLIMBABLE) {
						player_motion.location = LOCATION::NORMAL;
						player_motion.is_falling = true;
					}
					move_to_state(CharacterState::MOVE_LEFT);
					break;
				case GLFW_KEY_D:
					player_motion.goal_velocity.x = player_motion.speed;
					if (registry.motions.get(player).location == LOCATION::ON_CLIMBABLE) {
						player_motion.location = LOCATION::NORMAL;
						player_motion.is_falling = true;
					}
					move_to_state(CharacterState::MOVE_RIGHT);
					break;
				case GLFW_KEY_W:
					if (registry.motions.get(player).location == BELOW_CLIMBABLE 
						|| registry.motions.get(player).location == ON_CLIMBABLE) {
						player_motion.goal_velocity = vec2(0, -player_motion.speed);
						move_to_state(CharacterState::MOVE_UP);
					}
					break;
					
				case GLFW_KEY_S:
					if (registry.motions.get(player).location == ABOVE_CLIMBABLE
						|| registry.motions.get(player).location == ON_CLIMBABLE) {
						player_motion.goal_velocity = vec2(0, player_motion.speed);
						move_to_state(CharacterState::MOVE_DOWN);
					}
					break;
				}
			}
			break;

		case CharacterState::MOVE_LEFT:
			if (key == GLFW_KEY_A && action == GLFW_RELEASE)
			{
				player_motion.goal_velocity.x = 0;
				move_to_state(CharacterState::IDLE);
			}
			break;

		case CharacterState::MOVE_RIGHT:
			if (key == GLFW_KEY_D && action == GLFW_RELEASE)
			{
				player_motion.goal_velocity.x = 0;
				move_to_state(CharacterState::IDLE);
			}
			break;

		case CharacterState::MOVE_UP:
			if (key == GLFW_KEY_W && action == GLFW_RELEASE)
			{
				player_motion.goal_velocity = vec2(0);
				move_to_state(CharacterState::IDLE);
			}
			break;

		case CharacterState::MOVE_DOWN:
			if (key == GLFW_KEY_S && action == GLFW_RELEASE)
			{
				player_motion.goal_velocity = vec2(0);
				move_to_state(CharacterState::IDLE);
			}
		}
	} else {
		if (current_state == CharacterState::MOVE_LEFT || current_state == CharacterState::MOVE_RIGHT ||
			current_state == CharacterState::MOVE_UP || current_state == CharacterState::MOVE_DOWN) {
			player_motion.goal_velocity = vec2(0);
			move_to_state(CharacterState::IDLE);
		}
	}
}

void PlayerController::on_mouse_move(vec2 cursor_pos) {
	// Update attack preview to the correct angles/positions
	// Note it's possible that the attack preview has been destroyed in PERFORM_ABILITY state
	if (current_state == CharacterState::PERFORM_ABILITY_MANUAL && registry.attackPreviews.size() > 0) { 
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
			for (uint i = 0; i < registry.playerButtons.size(); i++) {

				Entity entity = registry.playerButtons.entities[i];
				Motion motion = registry.motions.get(entity);

				if (collides(click_motion, motion)) {

					bool ability_successfully_chosen = registry.clickables.get(entity).on_click();
					if (ability_successfully_chosen) {
                        if (registry.abilityButtons.has(entity)) {
                            move_to_state(CharacterState::PERFORM_ABILITY_AUTO);    
                            break;
                        }

						// Get player position at time of choosing an ability successfully
						player_pos = registry.motions.get(player).position;
						// Create preview object
						create_preview_object(player_pos);

						move_to_state(CharacterState::PERFORM_ABILITY_MANUAL);
                        break;
					}
				}
			}
		}
		break;

	case CharacterState::PERFORM_ABILITY_MANUAL:

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

			
			if (active_arsenal.basic_attack.activated == true) {
				// Melee/basic Audio
				Mix_PlayChannel(-1, melee_attack_sound, 0);
			}
			else {
				// Projectile/advanced Audio
				Mix_PlayChannel(-1, advanced_attack_sound, 0);
			}

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

bool PlayerController::has_player_moved_right()
{
	return current_state == CharacterState::MOVE_RIGHT;
}

void PlayerController::move_to_state(CharacterState next_state)
{
	switch (next_state) {
	case CharacterState::IDLE:
		std::cout << "moving to IDLE state" << std::endl;
		assert(current_state == CharacterState::MOVE_LEFT || current_state == CharacterState::MOVE_RIGHT ||
			current_state == CharacterState::MOVE_UP || current_state == CharacterState::MOVE_DOWN ||
			current_state == CharacterState::PERFORM_ABILITY_MANUAL);
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

    case CharacterState::PERFORM_ABILITY_AUTO:
		std::cout << "moving to PERFORM_ABILITY_AUTO state" << std::endl;
		assert(current_state == CharacterState::IDLE);
		break;

	case CharacterState::PERFORM_ABILITY_MANUAL:
		std::cout << "moving to PERFORM_ABILITY_MANUAL state" << std::endl;
		assert(current_state == CharacterState::IDLE);
		break;

	case CharacterState::END:
		std::cout << "moving to END state" << std::endl;

		assert(current_state == CharacterState::PERFORM_ABILITY_MANUAL || 
                current_state == CharacterState::PERFORM_ABILITY_AUTO ||
				current_state == CharacterState::IDLE ||
				current_state == CharacterState::MOVE_LEFT||
				current_state == CharacterState::MOVE_RIGHT);
		break;
	}
	this->next_state = next_state;
}

