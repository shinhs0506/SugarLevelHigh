#pragma once

#include "common.hpp"

// this class wraps all player's logic and states transitions
// separate all player logic from level manager 
class PlayerController
{
public:
	enum class PlayerState
	{
		IDLE,
		MOVE_LEFT,
		MOVE_RIGHT,
		MOVE_UP,
		MOVE_DOWN,
		PERFORM_ABILITY,
		END, // should not move to any other states from here
			 // this is set to prevent player continue to act after his turn
	};

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
	PlayerState current_state;
	PlayerState next_state;
	vec2 player_pos;

	void move_to_state(PlayerState next_state);
};
