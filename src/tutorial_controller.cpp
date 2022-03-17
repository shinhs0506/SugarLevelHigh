#include "tutorial_controller.hpp"
#include "level_init.hpp"
#include "tiny_ecs_registry.hpp"

TutorialController::TutorialController() {

}

TutorialController::~TutorialController() {

}

void TutorialController::init(LevelManager* level_manager) {
    this->level_manager = level_manager;
    return;
}

void TutorialController::step(float elapsed_ms) {
    // if there is already a prompt on the screen, decrement counter
    if (curr_step >= max_step) {
        return;
    }
    if (prompt_active && should_advance) {
        prompt_timer -= elapsed_ms;
    }
    // if current tutorial step has timed out, remove all prompts on the screen
    if (prompt_timer <= 0 && prompt_active) {
        remove_prompts();
        prompt_active = false;
        should_advance = false;
        curr_step += 1;
        prompt_timer = timeout;
    }
    // if no prompts on the screen, add the next one 
    if (!prompt_active) {
        Entity prompt = createPrompt(vec2(640, 360), vec2(1280, 720), curr_step);
        all_entities.push_back(prompt);
        prompt_active = true;
    }
}

void TutorialController::remove_prompts() {
    for (Entity& e : all_entities) {
        registry.remove_all_components_of(e);
    }
}

void TutorialController::destroy() {
    remove_prompts();
    prompt_active = false;
    curr_step = 0;
    prompt_timer = timeout;
    should_advance = false;
    return;
}
