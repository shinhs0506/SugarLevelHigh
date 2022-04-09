#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "tiny_ecs_registry.hpp"
#include "game_init.hpp"
#include "camera_manager.hpp"
#include "physics_system.hpp"
#include "menu_init.hpp"
#include "camera_manager.hpp"

class GameSystem;

class ConfigManager {
private:
    GameSystem* game_system;
    GLFWwindow* window;

    bool is_back_button_clicked;

    Entity background;
    Entity reso1920x1080_button;
    Entity reso1600x900_button;
    Entity reso1366x768_button;
    Entity reso1280x800_button;
    Entity reso1176x664_button;
    Entity reso800x600_button;
    Entity back_button;
    std::vector<Entity> all_entities;

    // Game renderer
    RenderSystem* renderer;

public:
    ConfigManager();

    ~ConfigManager();

    void init(GLFWwindow* window, GameSystem* game_system, RenderSystem* renderer);

    void destroy();

    bool step(float elapsed_ms);
    void handle_collisions();
    bool is_over();

    void on_key(int key, int, int action, int mod);
    void on_mouse_move(vec2 pos);
    void on_mouse_button(int button, int action, float* mouse_x_resolution_scale, float* mouse_y_resolution_scale);
    void delete_frame_buffer();
    void remake_frame_buffer(int new_width, int new_height);
    void resizing_window(float width, float height, float* x_resolution_scale, float* y_resolution_scale);
};