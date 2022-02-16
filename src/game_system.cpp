// Header
#include "game_system.hpp"

// stlib
#include <cassert>
#include <sstream>

#include "SDL_shape.h"
#include "ability.hpp"
#include "common.hpp"
#include "physics_system.hpp"
#include "level_init.hpp"
#include "tiny_ecs_registry.hpp"

#include <iostream>

const char* GAME_TITLE = "Sugar Level: High";

// Create the Game
GameSystem::GameSystem() {
}

GameSystem::~GameSystem() {
	// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);

	Mix_CloseAudio();

	// Destroy all created components
	registry.clear_all_components();

	// Close the window
	glfwDestroyWindow(window);
}

// Debugging
namespace {
	void glfw_err_cb(int error, const char *desc) {
		fprintf(stderr, "%d: %s", error, desc);
	}
}

// World initialization
// Note, this has a lot of OpenGL specific things, could be moved to the renderer
GLFWwindow* GameSystem::create_window() {
	///////////////////////////////////////
	// Initialize GLFW
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW");
		return nullptr;
	}

	//-------------------------------------------------------------------------
	// If you are on Linux or Windows, you can change these 2 numbers to 4 and 3 and
	// enable the glDebugMessageCallback to have OpenGL catch your mistakes for you.
	// GLFW / OGL Initialization
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);

	// Create the main window (for rendering, keyboard, and mouse input)
	window = glfwCreateWindow(window_width_px, window_height_px, GAME_TITLE, nullptr, nullptr);
	if (window == nullptr) {
		fprintf(stderr, "Failed to glfwCreateWindow");
		return nullptr;
	}

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((GameSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((GameSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	auto mouse_btn_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2) { ((GameSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_button(_0, _1, _2); };
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);
	glfwSetMouseButtonCallback(window, mouse_btn_redirect);

	//////////////////////////////////////
	// Loading music and sounds with SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Failed to initialize SDL Audio");
		return nullptr;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		fprintf(stderr, "Failed to open audio device");
		return nullptr;
	}

	background_music = Mix_LoadMUS(audio_path("music.wav").c_str());

	if (background_music == nullptr) {
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
			audio_path("music.wav").c_str());
		return nullptr;
	}

	return window;
}

void GameSystem::init(RenderSystem* renderer_arg) {
	this->renderer = renderer_arg;

	// Playing background music indefinitely
	Mix_PlayMusic(background_music, -1); 
	fprintf(stderr, "Loaded music\n");

	// set the title of the game
	glfwSetWindowTitle(window, GAME_TITLE);

	//// init a camera that is shared across all scenes
	//// camera offsets are the same as the window size
    vec2 offset = vec2(window_width_px / 2, window_height_px / 2);
	//// init position at center of the window, which is the same as offset
	//// also set x, y limit to the same as offset so the camera is not really movable
	//// need to modify limits in each level to match the map
    createCamera(offset, offset, offset, offset);
	
    // start with main menu
    this->current_game_state = GameState::MAIN_MENU;
    this->next_game_state = GameState::MAIN_MENU;
    move_to_state(GameState::MAIN_MENU);
}

void GameSystem::destroy_entities() {
    registry.remove_all_components_of(level_selection_button);
    registry.remove_all_components_of(help_button);
    registry.remove_all_components_of(exit_button);
}

bool GameSystem::is_over() {
    switch (current_game_state) {
        case GameState::IN_LEVEL: 
        {
            bool is_level_over = level_manager.is_over();
            if (is_level_over) {
                // TODO: move to the main menu state
                level_manager.abandon_level();
                move_to_state(GameState::MAIN_MENU);
            }
        }
        break;
        case GameState::MAIN_MENU:
        {
            // TODO: set WindowShouldClose to true if exit button is closed
            bool did_player_exit = menu_manager.is_over();
            if (did_player_exit) {
                glfwSetWindowShouldClose(window, GL_TRUE);
            }
        }
        break;
    }

	return bool(glfwWindowShouldClose(window));
}

// Update our game world
bool GameSystem::step(float elapsed_ms_since_last_update) {
	// Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
	    registry.remove_all_components_of(registry.debugComponents.entities.back());

    this->current_game_state = this->next_game_state;

	switch (current_game_state) {
	case GameState::IN_LEVEL:
		level_manager.step(elapsed_ms_since_last_update);
		break;
    case GameState::MAIN_MENU:
        menu_manager.step(elapsed_ms_since_last_update);
        break;
	default: 
		fprintf(stderr, "Fatal: entered invalid game state: %i", current_game_state);
		exit(1);
	}

	return true;
}

// On key callback
void GameSystem::on_key(int key, int, int action, int mod) {
	switch (current_game_state) {
	case GameState::IN_LEVEL:
		level_manager.on_key(key, 0, action, mod);
		break;
    case GameState::MAIN_MENU:
        menu_manager.on_key(key, 0, action, mod);
        break;
	default:
		fprintf(stderr, "Fatal: entered invalid game state: %i", current_game_state);
		exit(1);
	}
}

void GameSystem::on_mouse_move(vec2 mouse_position) {
	switch (current_game_state) {
	case GameState::IN_LEVEL:
		level_manager.on_mouse_move(mouse_position);
		break;
    case GameState::MAIN_MENU:
        menu_manager.on_mouse_move(mouse_position);
        break;
	default:
		fprintf(stderr, "Fatal: entered invalid game state: %i", current_game_state);
		exit(1);
	}
}

void GameSystem::on_mouse_button(int button, int action, int mod) {

	switch (current_game_state) {
	case GameState::IN_LEVEL:
		level_manager.on_mouse_button(button, action, mod);
		break;
    case GameState::MAIN_MENU:
    {
        menu_manager.on_mouse_button(button, action, mod);
    }
    break;
	default:
		fprintf(stderr, "Fatal: entered invalid game state: %i", current_game_state);
		exit(1);
	}
}

void GameSystem::handle_collisions() {

	switch (current_game_state) {
	case GameState::IN_LEVEL:
		level_manager.handle_collisions();
		break;
    case GameState::MAIN_MENU:
        menu_manager.handle_collisions();
        break;
	default:
		fprintf(stderr, "Fatal: entered invalid game state: %i", current_game_state);
		exit(1);
	}

	// Remove all collisions from this simulation step
	registry.collisions.clear();
}

void GameSystem::move_to_state(GameState next_game_state) {
    //registry.clear_all_components();

    
    if (next_game_state == GameState::IN_LEVEL) {
        menu_manager.destroy();
        level_manager.init(window, window_width_px, window_height_px);
        level_manager.load_level(0);
    } else if (next_game_state == GameState::MAIN_MENU) {
        menu_manager.init(window, this);
    }

    this->next_game_state = next_game_state;      
}

bool GameSystem::is_in_level() {
    return this->current_game_state == GameState::IN_LEVEL;
}
