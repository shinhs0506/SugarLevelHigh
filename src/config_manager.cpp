#include "config_manager.hpp"
#include "render_system.hpp"
#include <iostream>

ConfigManager::ConfigManager() {

}

ConfigManager::~ConfigManager() {

}

void ConfigManager::init(GLFWwindow* window, GameSystem* game_system, RenderSystem* renderer) {
    this->window = window;
    this->game_system = game_system;
    this->renderer = renderer;

    back_button = createBackButton(vec2(100, 100), vec2(50, 50), NULL);

    reso1920x1080_button = createConfigButton(vec2(640, 125), vec2(200, 50), NULL);
    reso800x600_button = createConfigButton(vec2(640, 225), vec2(200, 50), NULL);

    is_back_button_clicked = false;

    all_entities.push_back(back_button);
    all_entities.push_back(reso1920x1080_button);
    all_entities.push_back(reso800x600_button);

    return;
}

void ConfigManager::destroy() {
    for (Entity& e : all_entities) {
        registry.remove_all_components_of(e);
    }
    return;
}

bool ConfigManager::step(float elapsed_ms) {
    // do nothing;
    return false;
}

void ConfigManager::handle_collisions() {
    // do nothing;
    return;
}

bool ConfigManager::is_over() {
    return is_back_button_clicked;
}

void ConfigManager::on_key(int key, int, int action, int mod) {
    // do nothing;
    return;
}

void ConfigManager::on_mouse_move(vec2 pos) {
    // do nothing;
    return;
}

void ConfigManager::on_mouse_button(int button, int action, float* x_resolution_scale, float* y_resolution_scale) {
    double cursor_window_x, cursor_window_y;
    glfwGetCursorPos(window, &cursor_window_x, &cursor_window_y);
    vec2 cursor_window_pos = { cursor_window_x, cursor_window_y };

    Entity& camera = get_camera();
    vec2 camera_pos = registry.motions.get(camera).position;
    vec2 camera_offset = registry.cameras.get(camera).offset;

    vec2 cursor_world_pos = cursor_window_pos + camera_pos - camera_offset;
    cursor_world_pos.x = cursor_world_pos.x * *x_resolution_scale;
    cursor_world_pos.y = cursor_world_pos.y * *y_resolution_scale;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        Motion click_motion;
        click_motion.position = cursor_world_pos;
        click_motion.scale = { 1.f, 1.f };

        Motion back_button_motion = registry.motions.get(back_button);
        Motion reso1920x1080_motion = registry.motions.get(reso1920x1080_button);
        Motion reso800x600_motion = registry.motions.get(reso800x600_button);
        if (collides(click_motion, back_button_motion)) {
            // move to MAIN_MENU state
            is_back_button_clicked = true;
        }
        else if (collides(click_motion, reso1920x1080_motion)) {
            glfwSetWindowSize(window, 1280, 720);
            *x_resolution_scale = 1;
            *y_resolution_scale = 1;

            Camera& camera = registry.cameras.get(get_camera());
            float x_offset = 1280 / 2.f;
            float y_offset = 720 / 2.f;
            vec2 new_offset = vec2{ x_offset, y_offset };
            //camera.offset = new_offset;
            //reset_camera();

            delete_frame_buffer();
            remake_frame_buffer(1280, 720);
        }
        else if (collides(click_motion, reso800x600_motion)) {
            glfwSetWindowSize(window, 853, 480);
            float mouse_x = 1280.f / 853.f;
            float mouse_y = 720.f / 480.f;
            *x_resolution_scale = mouse_x;
            *y_resolution_scale = mouse_y;

            Camera& camera = registry.cameras.get(get_camera());
            float x_offset = 854.f / 2.f;
            float y_offset = 480.f / 2.f;
            vec2 new_offset = vec2{ x_offset, y_offset };
            //camera.offset = new_offset;
            //reset_camera();

            delete_frame_buffer();
            remake_frame_buffer(853, 480);
        }
        
    }
    return;
}

void ConfigManager:: delete_frame_buffer() {
    renderer->deleteFrameBuffer();
}

void ConfigManager:: remake_frame_buffer(int new_width, int new_height) {
    renderer->remakeFrameBuffer(window, new_width, new_height);
}