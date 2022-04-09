#pragma once

#include <tiny_ecs.hpp>
#include <tiny_ecs_registry.hpp>
#include "ability.hpp"
#include "physics_system.hpp"

class GameSystem;

class MenuManager
{
    private:

        //game
        GameSystem* game_system;

        // window
        GLFWwindow* window;

        // Images
        Entity background;
        Entity title;

        // buttons
        Entity level_selection_button;
        Entity help_button;
        Entity exit_button;
        Entity config_button;

        // player exit click
        bool did_player_exit = false;

    public:

        MenuManager();

        ~MenuManager();

        void init(GLFWwindow* window, GameSystem* game_system);
        void destroy();

        void step(float elapsed_ms);
        void handle_collisions();
        bool is_over();

        void on_key(int key, int, int action, int mod);
        void on_mouse_move(vec2 pos);
        void on_mouse_button(int button, int action, float* x_resolution_scale, float* y_resolution_scale);
};
