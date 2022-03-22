#pragma once

#include <vector>

#include "common.hpp"
#include "tiny_ecs.hpp"

class LevelManager;

class TutorialController {
private:
    LevelManager* level_manager;

    std::vector<Entity> all_entities;

    // whether or not there is an instrtuction on the screen
    bool prompt_active = false;

    // number of tutorial steps
    int max_step = 4;

    // how long to continue showing an instruction after the player follows it
    int timeout = 1000;
    int prompt_timer = timeout;

public:

    // stages of the tutorial
    // 0 = tell player to move
    // 1 = tell player to try basic atttack
    // 2 = tell player to move back and try advanced attack
    // 3 = tell player about cooldown and to kill the enemy
    // 4 = finished the tutorial 
    int curr_step = 0;

    // whether the tutorial should show the next instruction 
    bool should_advance = false;
    // whether the player has failed the tutorial
    bool failed = false;

    TutorialController();

    ~TutorialController();

    void init(LevelManager* level_manager);

    void step(float elapsed_ms);

    void remove_prompts();

    void destroy();

};

