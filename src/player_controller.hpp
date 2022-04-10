#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>


#include "common.hpp"
#include "components.hpp"

// this class wraps all player's logic and states transitions
// separate all player logic from level manager 
class PlayerController
{
public:
	PlayerController();
	~PlayerController();

	// reset the states to START
	void start_turn(Entity player, int curr_level);

	// callback functions by LevelManager that are only called during player's turn
	void step(float elapsed_ms);

	void on_key(int key, int, int action, int mod);

	void on_mouse_move(vec2 cursor_pos);

	void on_mouse_button(int button, int action, int mod, vec2 cursor_world_pos);

	bool should_end_player_turn();

	bool has_player_moved_right();

	bool should_camera_snap = true;
	bool cooldown_logic_enabled = true;

	Entity advanced_attack_button;
	Entity heal_button;

private:
	Entity player;
	CharacterState current_state;
	CharacterState next_state;
	vec2 player_pos;

	// prevent camera from snapping too quickly
	const float DEFAULT_BEGINNING_DELAY = 200.0f;
	float beginning_delay_counter_ms = DEFAULT_BEGINNING_DELAY; 
	bool has_camera_snapped = false;

	Mix_Chunk* melee_attack_sound;
	Mix_Chunk* advanced_attack_sound;
	Mix_Chunk* heal_ability_sound;

	void move_to_state(CharacterState next_state);
};
