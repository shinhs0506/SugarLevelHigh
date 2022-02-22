#pragma once
#include <vector>

#include "tiny_ecs_registry.hpp"
#include "player_controller.hpp"
#include "common.hpp"

// Wraps all level logis and entities
class LevelManager
{

public:
	enum class LevelState {
		PREPARE, // advance turn order  
		PLAYER_TURN, // player doesn't press anything
		ENEMY_MOVE, // enemy move state
		ENEMY_ATTACK, // enemy attack state
		EVALUATION, // attack processing state
        TERMINATION, // game ending logic
	};

	LevelManager();

	~LevelManager();

	void init(GLFWwindow* window);

	// Load all needed entities for the level
	void load_level(int level);

	// Restart the level
	void restart_level();

	// Abandon the level and directly go to results screen
	void abandon_level();

	// end the turn. should only called by active turn character
	void end_turn(Entity ender);

	// Should be called from GameSystem to step level content
	bool step(float elapsed_ms);
	void handle_collisions();

	// Whether this level ended
    bool is_over();


	void update_camera(vec2 velocity);

	// Input callback functions, should be called within GameSystem input callbacks
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);
	void on_mouse_button(int button, int action, int mod);

	// state machine functions
	void move_to_state(LevelState level_state);

	LevelState current_state();

private:
	int curr_level;

	Entity main_camera;
	const float CAM_MOVE_SPEED = 100;
    
	// for turn order logic
	std::vector<Entity> order_vector;
	int curr_order_ind;
	bool should_initialize_active_turn;
	int num_characters;

	std::vector<Entity> terrain_vector;
    std::vector<Entity> level_entity_vector;

	// OpenGL window handle
	GLFWwindow* window;

	// controller that handles player's input
	PlayerController player_controller;
	
	LevelState current_level_state;
	LevelState next_level_state;

    // game ending logic
    bool is_level_over;
  
    // remove the character from order_vector
    void remove_character(Entity entity);

    // read and initialize level data
    void init_data(int level);
};
