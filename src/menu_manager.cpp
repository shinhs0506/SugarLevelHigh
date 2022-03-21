#include "common.hpp"
#include "menu_init.hpp"
#include "game_init.hpp"
#include "menu_manager.hpp"
#include "game_system.hpp"
#include "camera_manager.hpp"

MenuManager::MenuManager() {

}

MenuManager::~MenuManager() {

}

void MenuManager::init(GLFWwindow* window, GameSystem* game_system) {
    reset_camera_pos();
    this->window = window;
    this->game_system = game_system;

    level_selection_button = createStartButton(vec2(640, 230), vec2(200,50), NULL);
    help_button = createHelpButton(vec2(640, 370), vec2(200,50), NULL);
    exit_button = createExitButton(vec2(640, 510), vec2(200,50), NULL);
}

void MenuManager::destroy() {
    registry.remove_all_components_of(level_selection_button);
    registry.remove_all_components_of(help_button);
    registry.remove_all_components_of(exit_button);
}

void MenuManager::step(float elapsed_ms) {
    // do nothing;
}

void MenuManager::handle_collisions() {
    // do nothing;
}

bool MenuManager::is_over() {
    return did_player_exit;
}

void MenuManager::on_key(int key, int, int action, int mod) {
    // do nothing
}

void MenuManager::on_mouse_move(vec2 pos) {
    // do nothing
}

void MenuManager::on_mouse_button(int button, int action, int mod) {
    double cursor_window_x, cursor_window_y;
    glfwGetCursorPos(window, &cursor_window_x, &cursor_window_y);
    vec2 cursor_window_pos = { cursor_window_x, cursor_window_y };

    Entity& camera = get_camera();
    vec2 camera_pos = registry.motions.get(camera).position;
    vec2 camera_offset = registry.cameras.get(camera).offset;

    vec2 cursor_world_pos = cursor_window_pos + camera_pos - camera_offset;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        Motion click_motion;
        click_motion.position = cursor_world_pos;
        click_motion.scale = { 1.f, 1.f };

        Motion level_selection_button_motion = registry.motions.get(level_selection_button);
        Motion help_button_motion = registry.motions.get(help_button);
        Motion exit_button_motion = registry.motions.get(exit_button);

        if (collides(click_motion, level_selection_button_motion)) {
            // open level selection menu
            this->game_system->move_to_state(GameSystem::GameState::LEVEL_SELECTION);
        } else if (collides(click_motion, help_button_motion)) {
            // open help menu
            this->game_system->move_to_state(GameSystem::GameState::HELP);
        } else if (collides(click_motion, exit_button_motion)) {
            // exit game
            did_player_exit = true;
        }
    }
}

