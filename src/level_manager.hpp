#pragma once
#include <vector>

#include "tiny_ecs_registry.hpp"
#include "player_controller.hpp"
#include <nlohmann/json.hpp>
#include "enemy_controller.hpp"
#include "reload_manager.hpp"
#include "tutorial_controller.hpp"

// Wraps all level logis and entities
class LevelManager
{

public:
	enum class LevelState {
		PREPARE, // advance turn order  
		PLAYER_TURN, // player doesn't press anything
		ENEMY_TURN, // handled by AI system
		EVALUATION, // attack processing state
        TERMINATION, // game ending logic
	};

	// flag for completed levels
	vec4 levels_completed = { false, false, false, false };

	LevelManager();

	~LevelManager();

	void init(GLFWwindow* window);

	void get_progress();

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

	// Input callback functions, should be called within GameSystem input callbacks
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);
	void on_mouse_button(int button, int action, int mod);

	// state machine functions
	void move_to_state(LevelState level_state);

	LevelState current_state();

	static void update_healthbar_len_color(Entity entity);

private:
	int curr_level;

	Entity main_camera;
	const float CAM_MOVE_SPEED = 200;
    
	// for turn order logic
	std::vector<Entity> order_vector;
	int curr_order_ind;
	bool should_initialize_active_turn;
	int num_characters;
    Entity order_indicator;

    nlohmann::json curr_level_data_json;

	// OpenGL window handle
	GLFWwindow* window;

    // manages data load and save
    ReloadManager reload_manager;

	// controller that handles enemy's behaviors
	EnemyController enemy_controller;

	// controller that handles player's input
	PlayerController player_controller;

	// controller that handles tutorial prompts
	TutorialController tutorial_controller;
	
	LevelState current_level_state;
	LevelState next_level_state;

    // game ending logic
    bool is_level_over;

    //  button
    Entity back_button;
    Entity basic_attack_button;
    Entity advanced_attack_button;

    Entity energy_bar;

    Entity background;
  
    // remove the character from order_vector
    void remove_character(Entity entity);

    // read and initialize level data
    void init_data(int level);

    void save_level_data();
    void destroy_saved_level_data_file();

    void update_curr_level_data();
};
