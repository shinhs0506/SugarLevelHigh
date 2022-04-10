#include "tutorial_controller.hpp"
#include "level_init.hpp"
#include "tiny_ecs_registry.hpp"

TutorialController::TutorialController() {

}

TutorialController::~TutorialController() {

}

void TutorialController::init(LevelManager* level_manager) {
    this->level_manager = level_manager;
    failed = false;
    should_advance = false;
    return;
}

void TutorialController::step(float elapsed_ms) {
    // if tutorial failed, reset 
    if (failed) {
        remove_prompts();
        Entity prompt = createPrompt(vec2(640, 360), vec2(1280, 720), -100);
        all_entities.push_back(prompt);
        prompt_active = true;
        return;
    }
    // if tutorial finished, do nothing
    if (curr_step >= max_step) {
        return;
    }
    // if there is already a prompt on the screen and its action is completed, decrement counter
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
    // also manage UI elements as appropriate
    if (!prompt_active) {
        Entity prompt = createPrompt(vec2(640, 360), vec2(1280, 720), curr_step);
        all_entities.push_back(prompt);
        prompt_active = true;
        init_step();
    }
}

void TutorialController::init_step() {
    if (curr_step == 1) {
        registry.clickables.get(basic_attack_button).disabled = false;
    }
    if (curr_step == 2) {
        registry.clickables.get(basic_attack_button).disabled = true;
        registry.clickables.get(heal_button).disabled = true;
    }
    if (curr_step >= 2) {
        registry.clickables.get(advanced_attack_button).disabled = false;
    }
    if (curr_step == 4) {
        registry.clickables.get(advanced_attack_button).disabled = true;
        registry.clickables.get(basic_attack_button).disabled = true;
    }
    if (curr_step >= 4) {
        registry.clickables.get(heal_button).disabled = false;
    }
    if (curr_step >= 5) {
        registry.clickables.get(basic_attack_button).disabled = false;
    }
}

void TutorialController::remove_prompts() {
    for (Entity& e : all_entities) {
        removePrompt(e);
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
