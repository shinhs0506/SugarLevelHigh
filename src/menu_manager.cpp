#include "common.hpp"
#include "menu_manager.hpp"
#include "game_system.hpp"

MenuManager::MenuManager() {

}

MenuManager::~MenuManager() {

}

void MenuManager::init(GLFWwindow* window, GameSystem* game_system) {
    this->window = window;
    this->game_system = game_system;

    level_selection_button = createButton(vec2(700, 200), vec2(200,50), mock_callback);
    //help_button = createButton(vec2(700, 300), vec2(200,50), mock_callback);
    exit_button = createButton(vec2(700, 400), vec2(200,50), mock_callback);
}

void MenuManager::destroy() {
    registry.remove_all_components_of(level_selection_button);
    //registry.remove_all_components_of(help_button);
    registry.remove_all_components_of(exit_button);
}

bool MenuManager::step(float elapsed_ms) {
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

    Entity& camera = registry.cameras.entities[0];
    vec2 camera_pos = registry.motions.get(camera).position;
    vec2 camera_offset = registry.cameras.get(camera).offset;

    vec2 cursor_world_pos = cursor_window_pos + camera_pos - camera_offset;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        Motion click_motion;
        click_motion.position = cursor_world_pos;
        click_motion.scale = { 1.f, 1.f };

        Motion level_selection_button_motion = registry.motions.get(level_selection_button);
        //Motion help_button_motion = registry.motions.get(help_button);
        Motion exit_button_motion = registry.motions.get(exit_button);

        if (collides(click_motion, level_selection_button_motion)) {
            // move to IN_LEVEL state
            this->game_system->move_to_state(GameSystem::GameState::IN_LEVEL);
        //} else if (collides(click_motion, help_button_motion)) {
            //// TODO: handle help
        } else if (collides(click_motion, exit_button_motion)) {
            // exit game
            did_player_exit = true;
        }
    }
}

