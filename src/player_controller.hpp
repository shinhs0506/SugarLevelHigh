#pragma once

#include "common.hpp"
#include "components.hpp"

// this class wraps all player's logic and states transitions
// separate all player logic from level manager 
class PlayerController
{
public:
	PlayerController();

	// reset the states to START
	void reset(Entity player);

	// callback functions by LevelManager that are only called during player's turn
	void step(float elapsed_ms);

	void on_key(int key, int, int action, int mod);

	void on_mouse_move(vec2 cursor_pos);

	void on_mouse_button(int button, int action, int mod, vec2 cursor_world_pos);

	bool should_end_player_turn();

private:
	Entity player;
	CharacterState current_state;
	CharacterState next_state;
	vec2 player_pos;

	void move_to_state(CharacterState next_state);
};
