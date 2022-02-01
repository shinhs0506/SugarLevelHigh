// Header
#include "game_system.hpp"

// stlib
#include <cassert>
#include <sstream>

#include "physics_system.hpp"

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
	
	// Directly start a level for now
	game_state = GameState::IN_LEVEL; // currently only working on the level

	level_manager = LevelManager();
	level_manager.init(window);

	level_manager.load_level(0);
}

bool GameSystem::is_over() {
	return bool(glfwWindowShouldClose(window));
}

// Update our game world
bool GameSystem::step(float elapsed_ms_since_last_update) {
	// Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
	    registry.remove_all_components_of(registry.debugComponents.entities.back());

	switch (game_state) {
	case GameState::IN_LEVEL:
		level_manager.step(elapsed_ms_since_last_update);
		break;
	default: 
		fprintf(stderr, "Fatal: entered invalid game state: %i", game_state);
		exit(1);
	}

	return true;
}

// On key callback
void GameSystem::on_key(int key, int, int action, int mod) {
	switch (game_state) {
	case GameState::IN_LEVEL:
		level_manager.on_key(key, 0, action, mod);
		break;
	default:
		fprintf(stderr, "Fatal: entered invalid game state: %i", game_state);
		exit(1);
	}
}

void GameSystem::on_mouse_move(vec2 mouse_position) {
	switch (game_state) {
	case GameState::IN_LEVEL:
		level_manager.on_mouse_move(mouse_position);
		break;
	default:
		fprintf(stderr, "Fatal: entered invalid game state: %i", game_state);
		exit(1);
	}
}

void GameSystem::on_mouse_button(int button, int action, int mod) {
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	switch (game_state) {
	case GameState::IN_LEVEL:
		level_manager.on_mouse_button(button, action, mod, xpos, ypos);
		break;
	default:
		fprintf(stderr, "Fatal: entered invalid game state: %i", game_state);
		exit(1);
	}
}

void GameSystem::handle_collisions() {
	switch (game_state) {
	case GameState::IN_LEVEL:
		level_manager.handle_collisions();
		break;
	default:
		fprintf(stderr, "Fatal: entered invalid game state: %i", game_state);
		exit(1);
	}

	// Remove all collisions from this simulation step
	registry.collisions.clear();
}
