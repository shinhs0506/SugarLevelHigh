#pragma once

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include "render_system.hpp"
#include "level_manager.hpp"

// Container for all our entities and game logic
class GameSystem
{
public:
	// different screen states
	enum class GameState {
		MAIN_MENU, // main menu screen
		MAIN_SETTING, // main menu settings for sound, etc.
		LEVEL_SELECTION, // select which level to start
		CHARACTER_SELECTION, // select characters to form a team for the level
		IN_LEVEL, // level content, handled by level manager
	};

	GameSystem();

	// Creates a window
	GLFWwindow* create_window();

	// starts the game
	void init(RenderSystem* renderer);

	// Releases all associated resources
	~GameSystem();

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);

	void handle_collisions();

	bool is_over();

private:
	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);
	void on_mouse_button(int button, int action, int mod);

	// OpenGL window handle
	GLFWwindow* window;

	// Game renderer
	RenderSystem* renderer;

	// Game states
	GameState game_state;
	 
	// music references
	Mix_Music* background_music;

	// Manges Level-scoped entities and logic
	LevelManager level_manager;
};
