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

    reso1920x1080_button = createConfigButton(vec2(640, 125), vec2(200, 50), NULL, TEXTURE_ASSET_ID::EXIT_BUTTON);
    reso1600x900_button = createConfigButton(vec2(640, 225), vec2(200, 50), NULL, TEXTURE_ASSET_ID::EXIT_BUTTON);
    reso1366x768_button = createConfigButton(vec2(640, 325), vec2(200, 50), NULL, TEXTURE_ASSET_ID::EXIT_BUTTON);
    reso1280x800_button = createConfigButton(vec2(640, 425), vec2(200, 50), NULL, TEXTURE_ASSET_ID::EXIT_BUTTON);
    reso1176x664_button = createConfigButton(vec2(640, 525), vec2(200, 50), NULL, TEXTURE_ASSET_ID::EXIT_BUTTON);
    reso800x600_button = createConfigButton(vec2(640, 625), vec2(200, 50), NULL, TEXTURE_ASSET_ID::EXIT_BUTTON);
    
    

    is_back_button_clicked = false;

    all_entities.push_back(back_button);
    all_entities.push_back(reso1920x1080_button);
    all_entities.push_back(reso1600x900_button);
    all_entities.push_back(reso1366x768_button);
    all_entities.push_back(reso1280x800_button);
    all_entities.push_back(reso1176x664_button);
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
        Motion reso1600x900_motion = registry.motions.get(reso1600x900_button);
        Motion reso1366x768_motion = registry.motions.get(reso1366x768_button);
        Motion reso1280x800_motion = registry.motions.get(reso1280x800_button);
        Motion reso1176x664_motion = registry.motions.get(reso1176x664_button);
        Motion reso800x600_motion = registry.motions.get(reso800x600_button);

        if (collides(click_motion, back_button_motion)) {
            // move to MAIN_MENU state
            is_back_button_clicked = true;
        }
        else if (collides(click_motion, reso1920x1080_motion)) {
            resizing_window(1280, 720, x_resolution_scale, y_resolution_scale);
        }
        else if (collides(click_motion, reso1600x900_motion)) {
            resizing_window(1066, 600, x_resolution_scale, y_resolution_scale);
        }
        else if (collides(click_motion, reso1366x768_motion)) {
            resizing_window(910, 512, x_resolution_scale, y_resolution_scale);
        }
        else if (collides(click_motion, reso1280x800_motion)) {
            resizing_window(883, 503, x_resolution_scale, y_resolution_scale);
        }
        else if (collides(click_motion, reso1176x664_motion)) {
            resizing_window(784, 442, x_resolution_scale, y_resolution_scale);
        }
        else if (collides(click_motion, reso800x600_motion)) {
            resizing_window(580, 320, x_resolution_scale, y_resolution_scale);
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

void ConfigManager::resizing_window(float width, float height, float* x_resolution_scale, float* y_resolution_scale) {
    glfwSetWindowSize(window, width, height);
    *x_resolution_scale = 1280.f / width;
    *y_resolution_scale = 720.f / height;

    delete_frame_buffer();
    remake_frame_buffer(width, height);
}