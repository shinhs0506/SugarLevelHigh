#include "level_menu_manager.hpp"
#include "tiny_ecs_registry.hpp"
#include "menu_init.hpp"
#include "game_init.hpp"
#include "physics_system.hpp"
#include "game_system.hpp"
#include "camera_manager.hpp"
#include <level_init.hpp>
#include <iostream>

LevelMenuManager::LevelMenuManager() {

}

LevelMenuManager::~LevelMenuManager() {

}

void LevelMenuManager::init(GLFWwindow* window, GameSystem* game_system) {
    reset_camera_pos();
    this->window = window;
    this->game_system = game_system;
    this->game_system->level_manager.get_progress();

    is_back_button_clicked = false;

    if (slide > 4) { // always enter the level selection menu after all the story slides are watched
        generateLevelMenu();
    }
    else {
        Entity story_slide = createStorySlide(vec2(640, 360), vec2(1280, 720), this->slide);
        next_button = createNextButton(vec2(1180, 620), vec2(50, 50), NULL);

        story_slides.push_back(next_button);
        story_slides.push_back(story_slide);
    }

    return;
}

void LevelMenuManager::generateLevelMenu()
{
    back_button = createBackButton(vec2(100, 100), vec2(64, 64), NULL);
    tutorial_button = createTutorialButton(vec2(640, 125), vec2(200, 50), NULL);
    level_1_button = createLevel1Button(vec2(640, 265), vec2(200, 50), NULL, !this->game_system->level_manager.levels_completed[0]);
    level_2_button = createLevel2Button(vec2(640, 405), vec2(200, 50), NULL, !this->game_system->level_manager.levels_completed[1]);
    level_3_button = createLevel3Button(vec2(640, 545), vec2(200, 50), NULL, !this->game_system->level_manager.levels_completed[2]);

    all_entities.push_back(back_button);
    all_entities.push_back(tutorial_button);
    all_entities.push_back(level_1_button);
    all_entities.push_back(level_2_button);
    all_entities.push_back(level_3_button);
}

void LevelMenuManager::destroy() {
    for (Entity& e : all_entities) {
        registry.remove_all_components_of(e);
    }
    return;
}

void LevelMenuManager::removeStorySlides() {
    for (Entity& e : story_slides) {
        registry.remove_all_components_of(e);
    }
    return;
}

bool LevelMenuManager::step(float elapsed_ms) {
    // do nothing;
    return false;
}

void LevelMenuManager::handle_collisions() {
    // do nothing;
    return;
}

bool LevelMenuManager::is_over() {
    return is_back_button_clicked;
}

void LevelMenuManager::on_key(int key, int, int action, int mod) 
{
    // enable all levels if press X
    if (action == GLFW_RELEASE)
    {
        switch (key) {
            case GLFW_KEY_X:
                printf("pressed x\n");
                this->game_system->level_manager.levels_completed = { true, true, true, true };
                destroy();
                generateLevelMenu();
                break;
        }
    }
    return;
}

void LevelMenuManager::on_mouse_move(vec2 pos) {
    // do nothing;
    return;
}

void LevelMenuManager::on_mouse_button(int button, int action, float* x_resolution_scale, float* y_resolution_scale) {
    double cursor_window_x, cursor_window_y;
    glfwGetCursorPos(window, &cursor_window_x, &cursor_window_y);
    vec2 cursor_window_pos = { cursor_window_x, cursor_window_y };

    Entity& camera = registry.cameras.entities[0];
    vec2 camera_pos = registry.motions.get(camera).position;
    vec2 camera_offset = registry.cameras.get(camera).offset;

    vec2 cursor_world_pos = cursor_window_pos + camera_pos - camera_offset;
    cursor_world_pos.x = cursor_world_pos.x * *x_resolution_scale;
    cursor_world_pos.y = cursor_world_pos.y * *y_resolution_scale;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        Motion click_motion;
        click_motion.position = cursor_world_pos;
        click_motion.scale = { 1.f, 1.f };

        if (slide <= 4) {
            if (collides(click_motion, registry.motions.get(next_button))) {
                slide++;
                if (slide > 4) {
                    removeStorySlides();
                    generateLevelMenu();
                }
                else {
                    story_slides.push_back(createStorySlide(vec2(640, 360), vec2(1280, 720), slide));
                }
            }
        }
        else {
            if (collides(click_motion, registry.motions.get(back_button))) {
                is_back_button_clicked = true;
            }
            else if (collides(click_motion, registry.motions.get(tutorial_button))) {
                this->selected_level = 0;
                this->game_system->move_to_state(GameSystem::GameState::IN_LEVEL);
            }
            else if (collides(click_motion, registry.motions.get(level_1_button)) && this->game_system->level_manager.levels_completed[0]) {
                this->selected_level = 1;
                this->game_system->move_to_state(GameSystem::GameState::IN_LEVEL);
            }
            else if (collides(click_motion, registry.motions.get(level_2_button)) && this->game_system->level_manager.levels_completed[1]) {
                this->selected_level = 2;
                this->game_system->move_to_state(GameSystem::GameState::IN_LEVEL);
            }
            else if (collides(click_motion, registry.motions.get(level_3_button)) && this->game_system->level_manager.levels_completed[2]) {
                this->selected_level = 3;
                this->game_system->move_to_state(GameSystem::GameState::IN_LEVEL);
            }
        }
    }
    return;
}
