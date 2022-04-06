#pragma once

#include <vector>

#include "common.hpp"
#include "tiny_ecs.hpp"

class GameSystem;

class LevelMenuManager {
private:
    GameSystem* game_system;

    GLFWwindow* window;

    Entity back_button;

    int slide = 0;

    // buttons
    Entity next_button;
    Entity tutorial_button;
    Entity level_1_button;
    Entity level_2_button;
    Entity level_3_button;

    bool is_back_button_clicked;

    std::vector<Entity> all_entities;
    std::vector<Entity> story_slides;

public:
    int selected_level = 0;

    LevelMenuManager();

    ~LevelMenuManager();

    void init(GLFWwindow* window, GameSystem* game_system);

    void generateLevelMenu();

    void destroy();
    void removeStorySlides();

    bool step(float elapsed_ms);
    void handle_collisions();
    bool is_over();

    void on_key(int key, int, int action, int mod);
    void on_mouse_move(vec2 pos);
    void on_mouse_button(int button, int action, float* x_resolution_scale, float* y_resolution_scale);
};

