#include "help_manager.hpp"
#include "tiny_ecs_registry.hpp"
#include "menu_init.hpp"
#include "physics_system.hpp"
#include "game_system.hpp"

HelpManager::HelpManager() {

}

HelpManager::~HelpManager() {

}

void HelpManager::init(GLFWwindow *window, GameSystem *game_system) {
    this->window = window;
    this->game_system = game_system;

    back_button = createBackButton(vec2(100, 100), vec2(50, 50), NULL);
    help_image = createHelpImage(vec2(700, 400), vec2(500, 500));

    is_back_button_clicked = false;

    all_entities.push_back(back_button);
    all_entities.push_back(help_image);

    return;
}

void HelpManager::destroy() {
    for (Entity& e : all_entities) {
        registry.remove_all_components_of(e);
    }
    return;
}

bool HelpManager::step(float elapsed_ms) {
    // do nothing;
    return false;
}

void HelpManager::handle_collisions() {
    // do nothing;
    return;
}

bool HelpManager::is_over() {
    return is_back_button_clicked;
}

void HelpManager::on_key(int key, int, int action, int mod) {
    // do nothing;
    return ;
}

void HelpManager::on_mouse_move(vec2 pos) {
    // do nothing;
    return;
}

void HelpManager::on_mouse_button(int button, int action, int mod) {
    double cursor_window_x, cursor_window_y;
    glfwGetCursorPos(window, &cursor_window_x, &cursor_window_y);
    vec2 cursor_window_pos = { cursor_window_x, cursor_window_y };

    Entity& camera = registry.cameras.entities[0];
    vec2 camera_pos = registry.motions.get(camera).position;
    vec2 camera_offset = registry.cameras.get(camera).offset;

    vec2 cursor_world_pos = cursor_window_pos + camera_pos - camera_offset;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        Motion click_motion;
        click_motion.position = cursor_world_pos;
        click_motion.scale = { 1.f, 1.f };

        Motion back_button_motion = registry.motions.get(back_button);

        if (collides(click_motion, back_button_motion)) {
            // move to IN_LEVEL state
            is_back_button_clicked = true;
        }
    }
    return;
}
