
#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>

// internal
#include "ai_system.hpp"
#include "physics_system.hpp"
#include "render_system.hpp"
#include "game_system.hpp"

using Clock = std::chrono::high_resolution_clock;

// Entry point
int main()
{
	// Global systems
	GameSystem game;
	RenderSystem renderer;
	PhysicsSystem physics;
	AISystem ai;

	srand((unsigned int)time(NULL));

	// Initializing window
	GLFWwindow* window = game.create_window();
	if (!window) {
		// Time to read the error message
		printf("Press any key to exit");
		getchar();
		return EXIT_FAILURE;
	}

	// initialize the main systems
	renderer.init(window);
	game.init(&renderer);
	ai.init(&game.level_manager);

	// variable timestep loop
	auto t = Clock::now();
	while (!game.is_over()) {
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms =
			(float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;

		game.step(elapsed_ms);
		ai.step(elapsed_ms);
		physics.step(elapsed_ms);
		game.handle_collisions();

		renderer.draw();
	}

	return EXIT_SUCCESS;
}
