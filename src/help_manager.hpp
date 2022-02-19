#pragma once

#include <vector>

#include "common.hpp"
#include "tiny_ecs.hpp"

class GameSystem;

class HelpManager {
    private:
        GameSystem* game_system;

        GLFWwindow* window;

        Entity back_button;
        Entity help_image;
        std::vector<Entity> all_entities;

        bool is_back_button_clicked;

    public:
        HelpManager();

        ~HelpManager();

        void init(GLFWwindow* window, GameSystem* game_system);

        void destroy();

        bool step(float elapsed_ms);
        void handle_collisions();
        bool is_over();

        void on_key(int key, int, int action, int mod);
        void on_mouse_move(vec2 pos);
        void on_mouse_button(int button, int action, int mod);
};

