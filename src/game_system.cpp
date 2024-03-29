// Header
#include "game_system.hpp"

// stlib
#include <cassert>
#include <sstream>
#include <iostream>

#include "physics_system.hpp"
#include "level_init.hpp"
#include "camera_manager.hpp"

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

	/*
	* Music by Eric Matyas
	* www.soundimage.org
	*/
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

    init_camera();
	
    // start with main menu
    this->current_game_state = GameState::MAIN_MENU;
    this->next_game_state = GameState::MAIN_MENU;
    menu_manager.init(window, this);
}

bool GameSystem::is_over() {
    switch (current_game_state) {
        case GameState::IN_LEVEL: 
        {
            bool is_level_over = level_manager.is_over();
            if (is_level_over) {
				for (uint i = 0; i < level_manager.prompts.size(); i++) {
					removePrompt(level_manager.prompts[i]);
				}
                move_to_state(GameState::LEVEL_SELECTION);
            }
        }
        break;
        case GameState::HELP:
        {
            bool did_exit_help = help_manager.is_over();
            if (did_exit_help) {
                move_to_state(GameState::MAIN_MENU);
            }
        }
        break;
		case GameState::LEVEL_SELECTION:
		{
			bool did_exit_level_menu = level_menu_manager.is_over();
			if (did_exit_level_menu) {
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
		case GameState::CONFIG:
		{
			bool did_exit_config = config_manager.is_over();
			if (did_exit_config) {
				move_to_state(GameState::MAIN_MENU);
			}
		}
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
    case GameState::HELP:
        help_manager.step(elapsed_ms_since_last_update);
        break;
	case GameState::LEVEL_SELECTION:
		level_menu_manager.step(elapsed_ms_since_last_update);
		break;
    case GameState::MAIN_MENU:
        menu_manager.step(elapsed_ms_since_last_update);
        break;
	case GameState::CONFIG:
		config_manager.step(elapsed_ms_since_last_update);
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
    case GameState::HELP:
        help_manager.on_key(key, 0, action, mod);
        break;
	case GameState::LEVEL_SELECTION:
		level_menu_manager.on_key(key, 0, action, mod);
		break;
    case GameState::MAIN_MENU:
        menu_manager.on_key(key, 0, action, mod);
        break;
	case GameState::CONFIG:
		config_manager.on_key(key, 0, action, mod);
		break;
	default:
		fprintf(stderr, "Fatal: entered invalid game state: %i", current_game_state);
		exit(1);
	}
}

void GameSystem::on_mouse_move(vec2 mouse_position) {
	double cursor_window_x, cursor_window_y;
	glfwGetCursorPos(window, &cursor_window_x, &cursor_window_y);
	vec2 cursor_window_pos = { cursor_window_x, cursor_window_y };

	Entity camera = registry.cameras.entities[0];
	vec2 camera_pos = registry.motions.get(camera).position;
	vec2 camera_offset = registry.cameras.get(camera).offset;

	float cursor_x = cursor_window_pos.x * x_resolution_scale + camera_pos.x - camera_offset.x;
	float cursor_y = cursor_window_pos.y * y_resolution_scale + camera_pos.y - camera_offset.y;
	vec2 cursor_world_pos = vec2(cursor_x, cursor_y);

	switch (current_game_state) {
	case GameState::IN_LEVEL:
		level_manager.on_mouse_move(cursor_world_pos);
		break;
	case GameState::HELP:
		help_manager.on_mouse_move(cursor_world_pos);
		break;
	case GameState::LEVEL_SELECTION:
		level_menu_manager.on_mouse_move(cursor_world_pos);
		break;
    case GameState::MAIN_MENU:
        menu_manager.on_mouse_move(cursor_world_pos);
        break;
	case GameState::CONFIG:
		config_manager.on_mouse_move(cursor_world_pos);
		break;
	default:
		fprintf(stderr, "Fatal: entered invalid game state: %i", current_game_state);
		exit(1);
	}
}

void GameSystem::on_mouse_button(int button, int action, int mod) {

	switch (current_game_state) {
	case GameState::IN_LEVEL:
		level_manager.on_mouse_button(button, action, &x_resolution_scale, &y_resolution_scale);
		break;
	case GameState::HELP:
		help_manager.on_mouse_button(button, action, &x_resolution_scale, &y_resolution_scale);
		break;
	case GameState::LEVEL_SELECTION:
		level_menu_manager.on_mouse_button(button, action, &x_resolution_scale, &y_resolution_scale);
		break;
    case GameState::MAIN_MENU:
    {
        menu_manager.on_mouse_button(button, action, &x_resolution_scale, &y_resolution_scale);
    }
    break;
	case GameState::CONFIG:
		config_manager.on_mouse_button(button, action, &x_resolution_scale, &y_resolution_scale);
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
	case GameState::HELP:
		help_manager.handle_collisions();
		break;
	case GameState::LEVEL_SELECTION:
		level_menu_manager.handle_collisions();
		break;
    case GameState::MAIN_MENU:
        menu_manager.handle_collisions();
        break;
	case GameState::CONFIG:
		config_manager.handle_collisions();
		break;
	default:
		fprintf(stderr, "Fatal: entered invalid game state: %i", current_game_state);
		exit(1);
	}

	// Remove all collisions from this simulation step
	registry.collisions.clear();
}

void GameSystem::move_to_state(GameState next_game_state) {
    
    // destroy current state's entities
    switch (current_game_state) {
        case GameState::MAIN_MENU:
            menu_manager.destroy();
            break;
        case GameState::HELP:
            help_manager.destroy();
            break;
		case GameState::LEVEL_SELECTION:
			level_menu_manager.destroy();
			break;
        case GameState::IN_LEVEL:
            level_manager.abandon_level();
            break;
		case GameState::CONFIG:
			config_manager.destroy();
			break;
    }

    // init next game state's entities
    // also check for correct state movement
    switch (next_game_state) {
        case GameState::MAIN_MENU:
            assert(current_game_state == GameState::IN_LEVEL || 
                    current_game_state == GameState::HELP ||
					current_game_state == GameState::LEVEL_SELECTION ||
					current_game_state == GameState::CONFIG);
            menu_manager.init(window, this);
            break;
		case GameState::LEVEL_SELECTION:
			assert(current_game_state == GameState::MAIN_MENU || current_game_state == GameState::IN_LEVEL);
			level_menu_manager.init(window, this);
			break;
        case GameState::HELP:
            assert(current_game_state == GameState::MAIN_MENU);
            help_manager.init(window, this);
            break;
        case GameState::IN_LEVEL:
            assert(current_game_state == GameState::LEVEL_SELECTION);
            level_manager.init(window);
            level_manager.load_level(this->level_menu_manager.selected_level);
            break;
		case GameState::CONFIG:
			assert(current_game_state == GameState::MAIN_MENU);
			config_manager.init(window, this, renderer);
			break;
    }
        
    this->next_game_state = next_game_state;      
}

bool GameSystem::is_in_level() {
    return this->current_game_state == GameState::IN_LEVEL;
}
