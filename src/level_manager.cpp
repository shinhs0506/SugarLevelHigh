// internal
#include "common.hpp"

#include <GLFW/glfw3.h>
#include <cfloat>
#include <common.hpp>
#include <vector>
#include <iostream>
#include <stdio.h>

#include "level_manager.hpp"
#include "level_init.hpp"
#include "game_init.hpp"
#include "physics_system.hpp"
#include "tiny_ecs_registry.hpp"
#include "ability.hpp"
#include "ability_list.hpp"
#include "camera_manager.hpp"
#include "tutorial_controller.hpp"

LevelManager::LevelManager()
{
    hurt_sound = Mix_LoadWAV(audio_path("hurt.wav").c_str());
}

LevelManager::~LevelManager()
{
    if (hurt_sound != nullptr)
        Mix_FreeChunk(hurt_sound);
    Mix_CloseAudio();
}

void LevelManager::init(GLFWwindow* window)
{
    reset_camera_pos();
    this->window = window;
    this->main_camera = get_camera();

    is_level_over = false;
}


// determine which levels can be accessed based on saved progress
void LevelManager::get_progress() {
    for (uint i = 1; i < 4; i++) {
        std::string saved_datafile_path = get_saved_level_data_file_path(i);
        std::ifstream sifs(saved_datafile_path);
        if (sifs.good()) {
            levels_completed[i - 1] = true;
        } 
    }
}

void LevelManager::init_data(int level) {
    Camera& camera = registry.cameras.get(main_camera);
    Motion& motion = registry.motions.get(main_camera);

    reload_manager.load(level);

    CameraData camera_data = reload_manager.get_camera_data();
    motion.position = camera_data.pos;
    camera.lower_limit = motion.position + camera_data.lower_limit_delta;
    camera.higer_limit = motion.position + camera_data.upper_limit_delta;
    BackgroundData background_data = reload_manager.get_background_data();
    background = createBackground(background_data.size, level);

    if (level == 0 || level == 1) {
        background1 = createBackground(background_data.size, 11);
        background2 = createBackground(background_data.size, 12);
    }
    else if (level == 2) {
        background1 = createBackground(background_data.size, 21);
        background2 = createBackground(background_data.size, 22);
    }
    else if (level == 3) {
        background1 = createBackground(background_data.size, 31);
        background2 = createBackground(background_data.size, 32);
    }
    else if (level == 4) {
        background1 = createBackground(background_data.size, 41);
        background2 = createBackground(background_data.size, 42);
    }

    for (auto& player_data: reload_manager.get_player_data()) {

        gingerbread_advanced_attack.current_cooldown = player_data.advanced_attack_cooldown;
        gingerbread_heal_buff.current_cooldown = player_data.heal_cooldown;
        AttackArsenal ginerbread_arsenal = { gingerbread_basic_attack, gingerbread_advanced_attack};
        BuffArsenal gingerbread_buffs = { gingerbread_heal_buff };
        Entity player = createPlayer(player_data.pos, player_data.size, (level == 0) ? 150 : player_data.health,
                player_data.energy, ginerbread_arsenal, (level == 2) ? true : false, (level == 3) ? true : false, gingerbread_buffs);

        update_healthbar_len_color(player);
        order_vector.push_back(player);
    }

    for (auto& enemy_data: reload_manager.get_enemy_data()) {
        chocolateball_advanced_attack.current_cooldown = enemy_data.advanced_attack_cooldown;
        AttackArsenal gumball_arsenal = { chocolateball_basic_attack, chocolateball_advanced_attack };
        Entity enemy = createEnemy(enemy_data.pos, enemy_data.size, enemy_data.health, 
                enemy_data.energy, gumball_arsenal, (level == 2) ? true : false, (level == 3) ? true : false, (level == 4) ? true : false);
        update_healthbar_len_color(enemy);
        order_vector.push_back(enemy);
    }

    for (auto& terrain_data: reload_manager.get_terrain_data()) {
        Entity terrain = createTerrain(terrain_data.pos, terrain_data.size, terrain_data.breakable, level);
    }

    for (auto& ladder_data : reload_manager.get_ladder_data()) {
        Entity ladder = createLadder(ladder_data.pos, ladder_data.size);
    }

    curr_order_ind = reload_manager.get_curr_order_ind();

    this->current_level_state = (LevelState) reload_manager.get_curr_level_state();
    this->next_level_state = (LevelState) reload_manager.get_curr_level_state();

    if (current_level_state == LevelState::LEVEL_START) {
        if (level > 0) {
            level_start_prompt = createPrompt(vec2(640, 360), vec2(1280, 720), level * 10);
        }
        createBlinkTimer(1000);
    }
}

bool compare(Entity a, Entity b) {
    auto a_init = registry.initiatives.get(a);
    auto b_init = registry.initiatives.get(b);
    return a_init.value < b_init.value;
};

void LevelManager::load_level(int level)
{
    this->curr_level = level; 
    this->init_data(level);

    // common to all levels

    back_button = createBackButton(vec2(100, 50), vec2(64, 64), NULL);

    if (curr_level > 0) {
        save_button = createSaveButton(vec2(1200, 50), vec2(50, 50), NULL);
    }

    heal_button = createAbilityButton(vec2(100, 450), vec2(50, 50), mock_heal_callback, TEXTURE_ASSET_ID::BUTTON_HEAL);
    basic_attack_button = createPlayerButton(vec2(100, 300), vec2(50, 50), mock_basic_attack_callback, TEXTURE_ASSET_ID::BUTTON_MELEE);
    advanced_attack_button = createPlayerButton(vec2(100, 375), vec2(50, 50), mock_advanced_attack_callback, TEXTURE_ASSET_ID::BUTTON_ADVANCED);

    this->player_controller.heal_button = heal_button;
    this->player_controller.advanced_attack_button = advanced_attack_button;

    ui_layout = createUI(vec2(640, 360), vec2(1280, 720));
    energy_bar = createEnergyBar();
    order_indicator = createOrderIndicator();

    sort(order_vector.begin(), order_vector.end(), compare);

    // level specific logic
    if (level == 0) {
        this->tutorial_controller.init(this);
        this->tutorial_controller.heal_button = heal_button;
        this->tutorial_controller.basic_attack_button = basic_attack_button;
        this->tutorial_controller.advanced_attack_button = advanced_attack_button;
        this->player_controller.cooldown_logic_enabled = false;
        this->player_controller.should_camera_snap = false;
        this->enemy_controller.should_camera_snap = false;
        registry.clickables.get(heal_button).disabled = true;
        registry.clickables.get(basic_attack_button).disabled = true;
        registry.clickables.get(advanced_attack_button).disabled = true;
    }
    else {
        this->player_controller.cooldown_logic_enabled = true;
        this->player_controller.should_camera_snap = true;
        this->enemy_controller.should_camera_snap = true;
    }
}

void LevelManager::restart_level()
{

}

void LevelManager::save_level_data(){
    if (curr_level > 0) {
        reload_manager.save(curr_level);
    }
}

void LevelManager::abandon_level()
{
    for (auto& attack_object : registry.attackObjects.entities) {
        removeAttackObject(attack_object);
    }

    for (auto& attack_preview: registry.attackPreviews.entities) {
        registry.remove_all_components_of(attack_preview);
    }

    for (auto& player : registry.playables.entities) {
        removePlayer(player);
    }

    for (auto& enemy : registry.enemies.entities) {
        removeEnemy(enemy);
    }

    for (auto& terrain : registry.terrains.entities) {
        removeTerrain(terrain);
    }

    for (auto& ladder : registry.climbables.entities) {
        removeLadder(ladder);
    }

    for (auto& cooldown : registry.cooldowns.entities) {
        removeCooldown(cooldown);
    }
  
    for (auto& prompts : registry.promptsWithTimer.entities) {
        removePromptWithTimer(prompts);
    }

    for (auto& snow : registry.snows.entities) {
        removeSnow(snow);
    }

    removeButton(back_button);
    removeButton(save_button);
    removePlayerButton(basic_attack_button);
    removePlayerButton(advanced_attack_button);
    removeAbilityButton(heal_button);

    removeUI(ui_layout);
    removeEnergyBar();
    removeOrderIndicator();
    removeBackground(background);
    removeBackground(background1);
    removeBackground(background2);

    registry.activeTurns.clear();

    order_vector.clear();

    if (curr_level == 0) {
        tutorial_controller.destroy();
    }
}

void LevelManager::remove_character(Entity entity)
{
    auto it = std::find(order_vector.begin(), order_vector.end(), entity);
    int pos = it - order_vector.begin();
    if (pos <= curr_order_ind) {
        curr_order_ind -= 1;
    }

    // directly move to evaluation state only if current active character died
    // this is to ensure turn correctly ends the current active chracter died in movement state
    
    order_vector.erase(it);

}

void LevelManager::update_curr_level_data(){
    // save camera info
    Camera& camera = registry.cameras.get(main_camera);
    Motion& camera_motion = registry.motions.get(main_camera);
    CameraData camera_data {
        camera_motion.position,
        camera.lower_limit - camera_motion.position,
        camera.higer_limit - camera_motion.position
    };
    reload_manager.update_camera_data(camera_data);
    

    // save background info
    Entity& background = registry.backgrounds.entities[0];
    Motion& background_motion = registry.motions.get(background);
    BackgroundData background_data {
        background_motion.scale,
    };
    reload_manager.update_background_data(background_data);

    // save players info
    std::vector<PlayerData> player_data_vector;
    for (auto& player : registry.playables.entities) {
        Motion& player_motion = registry.motions.get(player);
        Health& player_health = registry.healths.get(player);
        Energy& player_energy = registry.energies.get(player);
        BuffArsenal& player_buff = registry.buffArsenals.get(player);
        AttackArsenal& player_arsenal = registry.attackArsenals.get(player);
        PlayerData pd {
            player_motion.position,
            player_motion.scale,
            player_health.cur_health,
            player_energy.cur_energy,
            player_buff.heal.current_cooldown,
            player_arsenal.advanced_attack.current_cooldown
        };
        player_data_vector.push_back(pd);
    }
    reload_manager.update_player_data(player_data_vector);

    // save enemies info
    std::vector<EnemyData> enemy_data_vector;
    for (auto& enemy : registry.enemies.entities) {
        Motion& enemy_motion = registry.motions.get(enemy);
        Health& enemy_health = registry.healths.get(enemy);
        Energy& enemy_energy = registry.energies.get(enemy);
        AttackArsenal& enemy_arsenal = registry.attackArsenals.get(enemy);
        EnemyData ed {
            enemy_motion.position,
            enemy_motion.scale,
            enemy_health.cur_health,
            enemy_energy.cur_energy,
            enemy_arsenal.advanced_attack.current_cooldown
        };
        enemy_data_vector.push_back(ed);
    }
    reload_manager.update_enemy_data(enemy_data_vector);

    // save terrain info
    std::vector<TerrainData> terrain_data_vector;
    for (auto& terrain : registry.terrains.entities) {
        Motion& terrain_motion = registry.motions.get(terrain);
        TerrainData td {
            terrain_motion.position,
            terrain_motion.scale
        };
        terrain_data_vector.push_back(td);
    }
    reload_manager.update_terrain_data(terrain_data_vector);

    // save ladder info
    std::vector<LadderData> ladder_data_vector;
    for (auto& ladder : registry.climbables.entities) {
        Motion& ladder_motion = registry.motions.get(ladder);
        LadderData ld {
            ladder_motion.position,
            ladder_motion.scale
        };
        ladder_data_vector.push_back(ld);
    }
    reload_manager.update_ladder_data(ladder_data_vector);

    // save curr order ind
    reload_manager.update_curr_order_ind(curr_order_ind);
}

bool LevelManager::step(float elapsed_ms)
{
    // advance state
    this->current_level_state = this->next_level_state;

    // manage tutorial state
    if (curr_level == 0) {
        this->tutorial_controller.step(elapsed_ms);
        if (this->tutorial_controller.curr_step >= 2) {
            this->player_controller.cooldown_logic_enabled = true;
        }
    }

    for (uint i = 0; i < registry.promptsWithTimer.size(); i++) {
        Entity entity = registry.promptsWithTimer.entities[i];
        PromptWithTimer& prompWithTimer = registry.promptsWithTimer.components[i];

        prompWithTimer.timer -= elapsed_ms;
        if (prompWithTimer.timer < 0) {
            removePromptWithTimer(entity);
        }
    }

    // remove snows out of the boundary
    for (int i = 0; i < registry.snows.components.size(); i++) {
        if (registry.motions.get(registry.snows.entities[i]).position.y > 1240) {
            removeSnow(registry.snows.entities[i]);
        }
    }

    // render new snowflake
    next_snow_spawn -= elapsed_ms; 
    if (next_snow_spawn < 0.f && curr_level == 3 && registry.snows.size() < max_snow) {
        // Reset timer
        next_snow_spawn = (300 / 2) + uniform_dist(rng) * (300 / 2);
        // Create bug with random initial size
        float random = uniform_dist(rng);
        float speed = 50.0 + 100.0 * random;
        Entity snow;
        //if (random < (float)1 / 6) {
        //    snow = createSnow(vec2(2664 * uniform_dist(rng), -508), vec2(0, speed), vec2(6, 6), TEXTURE_ASSET_ID::SNOW1);
        //}
        //else if (random < (float)2 / 6) {
        //    snow = createSnow(vec2(2664 * uniform_dist(rng), -508), vec2(0, speed), vec2(10, 10), TEXTURE_ASSET_ID::SNOW2);
        //}
        //else if (random < (float)3 / 6) {
        //    snow = createSnow(vec2(2664 * uniform_dist(rng), -508), vec2(0, speed), vec2(14, 14), TEXTURE_ASSET_ID::SNOW3);
        //}
        //else if (random < (float)4 / 6) {
        //    snow = createSnow(vec2(2664 * uniform_dist(rng), -508), vec2(0, speed), vec2(22, 22), TEXTURE_ASSET_ID::SNOW4);
        //}
        //else if (random < (float)5 / 6) {
            snow = createSnow(vec2(2664 * uniform_dist(rng), -508), vec2(0, speed), vec2(34, 34), TEXTURE_ASSET_ID::SNOW5);
        //}
        //else {
            //snow = createSnow(vec2(2664 * uniform_dist(rng), -508), vec2(0, speed), vec2(34, 34), TEXTURE_ASSET_ID::SNOW6);
        //}
    }
  
    // remove dead entities (with health component and current health below 0)
    for (uint i = 0; i < registry.healths.size(); i++) {
        Entity entity = registry.healths.entities[i];
        Health health = registry.healths.components[i];
        assert(health.cur_health >= 0.f); // health shouldn't below 0

        if (health.dead) {
            std::cout << "health dead" << std::endl;
            // check playables
            if (registry.playables.has(entity)) {
                remove_character(entity);
                removePlayer(entity);
            }
            else if (registry.enemies.has(entity)) {
                remove_character(entity);
                removeEnemy(entity);
            }
            else if (registry.terrains.has(entity) && registry.terrains.get(entity).breakable) {
                removeTerrain(entity);
            }
        }
    }

    // store info of all entities in reload manager 
    update_curr_level_data();

    bool only_player_left = registry.playables.size() == registry.initiatives.size();
    bool only_enemy_left = registry.initiatives.size() == registry.enemies.size();

    switch (current_level_state) {
    case LevelState::LEVEL_START:
        break;
    case LevelState::ENEMY_BLINK:
        if (registry.blinkTimers.size() > 0) {
            Entity& entity = registry.blinkTimers.entities[0];
            BlinkTimer& timer = registry.blinkTimers.components[0];
            if (timer.timer > 0) {
                timer.timer -= elapsed_ms;
            } else {
                removeBlinkTimer(entity);
            }
        } else {
            move_to_state(LevelState::PREPARE);
        }
        break;

    case LevelState::PREPARE:
        {

            // update health bar for all characters
            for (uint i = 0; i < registry.initiatives.size(); i++) {
                Entity entity = registry.initiatives.entities[i];
                update_healthbar_len_color(entity);
            }
            // check whether level completed/failed
            if (only_player_left || only_enemy_left) {
                // allow progression to next level via menu if current level completed
                if (only_player_left) {
                    if (curr_level == 0) {
                        tutorial_controller.should_advance = true;
                    }
                    else {
                        // TODO: unique prompt per level
                        Entity prompt = createPrompt(vec2(640, 360), vec2(1280, 720), 11);
                        prompts.push_back(prompt);
                    }
                    
                    
                    if (curr_level != 4) {
                        this->levels_completed[curr_level] = true;
                    }
                }
                else if (only_enemy_left) {
                    if (curr_level == 0) {
                        tutorial_controller.failed = true;
                        tutorial_controller.should_advance = true;
                    }
                    else {
                        Entity prompt = createPrompt(vec2(640, 360), vec2(1280, 720), curr_level * 10 + 2);
                        prompts.push_back(prompt);
                    }
                }
                if (registry.cooldowns.size() > 0) {
                    for (auto& cooldown : registry.cooldowns.entities) {
                        removeCooldown(cooldown);
                    }
                }
                registry.clickables.get(basic_attack_button).disabled = true;
                registry.clickables.get(advanced_attack_button).disabled = true;
                registry.clickables.get(heal_button).disabled = true;
                move_to_state(LevelState::TERMINATION);
                break;
            }

            int num_characters = registry.initiatives.size();

            int prev_ind = curr_order_ind % num_characters;
            if (prev_ind < 0) {
                prev_ind = prev_ind + num_characters;
            }
            registry.motions.get(order_vector[prev_ind]).depth = DEPTH::CHARACTER;
            curr_order_ind = (curr_order_ind + 1) % num_characters;
            Entity& next_character = order_vector[curr_order_ind];

            registry.activeTurns.clear();
            registry.activeTurns.emplace(next_character);
            registry.motions.get(next_character).depth = DEPTH::ACTIVE;

            // reset prev character's energy level
            Energy& energy = registry.energies.get(order_vector[prev_ind]);
            energy.cur_energy = energy.max_energy;

            if (registry.playables.has(registry.activeTurns.entities[0])) {
                std::cout << "player is current character" << std::endl;
                // reset player controller
                player_controller.start_turn(registry.activeTurns.entities[0], curr_level);

                move_to_state(LevelState::PLAYER_TURN);
                resetEnergyBar();
            }
            else {
                std::cout << "enemy is current character" << std::endl;
                enemy_controller.start_turn(registry.activeTurns.entities[0]);

                move_to_state(LevelState::ENEMY_TURN);
                resetEnergyBar();
            }
        }
        break;

    case LevelState::PLAYER_TURN:
        // step player controller
        player_controller.step(elapsed_ms);
        if (player_controller.should_end_player_turn())
        {
            if (curr_level == 0 && (tutorial_controller.curr_step == 2 || tutorial_controller.curr_step == 4) 
                && !tutorial_controller.should_advance) {
                tutorial_controller.should_advance = true;
            }
            move_to_state(LevelState::EVALUATION);
        }

        if (curr_level == 0 && tutorial_controller.curr_step == 0 && !tutorial_controller.should_advance && player_controller.has_player_moved_right()) {
            tutorial_controller.should_advance = true;
        }
        
        break;
        
    case LevelState::ENEMY_TURN:
        // step enemy controller
        enemy_controller.step(elapsed_ms);
        if (enemy_controller.should_end_enemy_turn())
        {
            if (curr_level == 0 && (tutorial_controller.curr_step == 1 || tutorial_controller.curr_step == 3)
                && !tutorial_controller.should_advance) {
                tutorial_controller.should_advance = true;
            }
            move_to_state(LevelState::EVALUATION);
        }

        break;

    case LevelState::EVALUATION:
    {
        // remove timed out attack objects
        for (uint i = 0; i < registry.attackObjects.size(); i++) {
            Entity entity = registry.attackObjects.entities[i];
            AttackObject& obj = registry.attackObjects.components[i];
            obj.ttl_ms -= elapsed_ms;
            if (obj.ttl_ms < 0) {
                removeAttackObject(entity);
            }
        }

        // update hit effect ttl
        for (uint i = 0; i < registry.hitEffects.size(); i++) {
            Entity entity = registry.hitEffects.entities[i];
            HitEffect& effect = registry.hitEffects.components[i];
            effect.ttl_ms -= elapsed_ms;
            if (effect.ttl_ms < 0) {
                removeHitEffect(entity);

                // only set dead after hit effect played 
                if (registry.healths.has(entity) && registry.healths.get(entity).cur_health < epsilon) {
                    registry.healths.get(entity).dead = true;
                }
            }
        }

        // check if all attacks finished
        if (registry.attackObjects.size() == 0 && registry.hitEffects.size() == 0) {
            move_to_state(LevelState::PREPARE);
        }

    }
        break;

    case LevelState::TERMINATION:
        reload_manager.destroy_saved_level_data_file(curr_level);
        break;
    }

    return true;
}

void LevelManager::update_healthbar_len_color(Entity entity) {
    Health& health = registry.healths.get(entity);
    Entity healthBar;

    if (registry.playables.has(entity)) {
        Playable& playable = registry.playables.get(entity);
        healthBar = playable.healthBar;
    }
    if (registry.enemies.has(entity)) {
        Enemy& enemy = registry.enemies.get(entity);
        healthBar = enemy.healthBar;
    }
    Motion& healthBar_motion = registry.motions.get(healthBar);
    healthBar_motion.scale = { healthBar_motion.original_scale.x * (health.cur_health / health.max_health), 10 };

    // change health bar color based on remaining health
    vec3& color = registry.colors.get(healthBar);
    if (health.cur_health / health.max_health <= 0.2f) {
        color = vec3(1.f, 0.f, 0.f);
    }
    else if (health.cur_health / health.max_health <= 0.5f) {
        color = vec3(1.f, 0.5f, 0.f);
    }
}

void LevelManager::handle_collisions()
{
    // handle attack objects collisions
    for (uint i = 0; i < registry.collisions.size(); i++) {
        Entity entity = registry.collisions.entities[i];
        if (registry.attackObjects.has(entity)) {
            // DO NOT use get() on collisions here!!!
            // collisions registry might have two collisions on the same object
            // using get() will always retrieve the first collision component
            Entity other_entity = registry.collisions.components[i].other;

            AttackObject& attack = registry.attackObjects.get(entity);
            bool is_player_attack = registry.playables.has(attack.attacker);

            // only be able to deal damage on entities with health
            bool damagable = registry.healths.has(other_entity);

            // only be able to deal damage if two entities are different type // (not both playables or enemies) 
            bool different_team = (is_player_attack && !registry.playables.has(other_entity)) ||
                (!is_player_attack && !registry.enemies.has(other_entity));

            // check attacked set to make sure only deal damage once
            bool attacked = attack.attacked.find(other_entity) != attack.attacked.end();

            if (damagable && different_team && !attacked) {

                // If attack object is a projectile, reduce its ttl so that it dies shortly on collision
                // Shortly because we want abit of visuals that contact is made
                if (registry.projectiles.has(entity)) {
                    attack.ttl_ms = 10.0f;
                }
                // If attack was made to unbreakable terrain, shortcircut and do not do hit effect/damage
                if (registry.terrains.has(other_entity) && !registry.terrains.get(other_entity).breakable) {
                    continue;
                }
                // Attack hit damagable object
                Health& health = registry.healths.get(other_entity);
                // health shouldn't be below zero
                health.cur_health = clamp(health.cur_health - attack.damage, 0.f, FLT_MAX);
                attack.attacked.insert(other_entity);

                // Hit/hurt audio
                Mix_PlayChannel(-1, hurt_sound, 0);
  
                // change health bar length for players or enemies
                if (registry.playables.has(other_entity) || registry.enemies.has(other_entity)) {
                    update_healthbar_len_color(other_entity);
                }
              
                createHitEffect(other_entity, 200); // this ttl should be less then attack object ttl 
            }
        }
    }
}

bool LevelManager::is_over() {
    return is_level_over;
}

void LevelManager::on_key(int key, int scancode, int action, int mod)
{
    switch (current_level_state) {
    case LevelState::LEVEL_START:
        if (action == GLFW_RELEASE && key == GLFW_KEY_ENTER)
        {
            removePrompt(level_start_prompt);
            move_to_state(LevelState::ENEMY_BLINK);
        }
        break;
    case LevelState::PLAYER_TURN: 
        // handle all player logic to a player controller
        player_controller.on_key(key, scancode, action, mod);
        break;

    case LevelState::TERMINATION:
        if (action == GLFW_RELEASE && key == GLFW_KEY_ESCAPE) 
        {
            is_level_over = true;
        }
        break;
    }

    // actions to perform regardless of the state
    // camera control logic
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_LEFT:
            move_camera(vec2(-CAM_MOVE_SPEED, 0)); break;
        case GLFW_KEY_RIGHT:
            move_camera(vec2(CAM_MOVE_SPEED, 0)); break;
        case GLFW_KEY_UP:
            move_camera(vec2(0, -CAM_MOVE_SPEED)); break;
        case GLFW_KEY_DOWN:
            move_camera(vec2(0, CAM_MOVE_SPEED)); break;
        }

    }
    if (action == GLFW_RELEASE)
    {
        switch (key)
        {
        case GLFW_KEY_LEFT:
            move_camera(vec2(CAM_MOVE_SPEED, 0)); break;
        case GLFW_KEY_RIGHT:
            move_camera(vec2(-CAM_MOVE_SPEED, 0)); break;
        case GLFW_KEY_UP:
            move_camera(vec2(0, CAM_MOVE_SPEED)); break;
        case GLFW_KEY_DOWN:
            move_camera(vec2(0, -CAM_MOVE_SPEED)); break;
        }
    }

}

void LevelManager::on_mouse_move(vec2 pos)
{
    switch (current_level_state) {
    case LevelState::PLAYER_TURN:
        player_controller.on_mouse_move(pos);
        break;
    }
}

void LevelManager::on_mouse_button(int button, int action, float* x_resolution_scale, float* y_resolution_scale)
{
    double cursor_window_x, cursor_window_y;
    glfwGetCursorPos(window, &cursor_window_x, &cursor_window_y);
    vec2 cursor_window_pos = { cursor_window_x, cursor_window_y };

    vec2 camera_pos = registry.motions.get(main_camera).position;
    vec2 camera_offset = registry.cameras.get(main_camera).offset;

    float cursor_x = cursor_window_pos.x * *x_resolution_scale + camera_pos.x - camera_offset.x;
    float cursor_y = cursor_window_pos.y * *y_resolution_scale + camera_pos.y - camera_offset.y;
    vec2 cursor_world_pos = vec2(cursor_x, cursor_y);

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        Motion click_motion;
        click_motion.position = cursor_world_pos;
        click_motion.scale = { 1.f, 1.f };

        Motion back_button_motion = registry.motions.get(back_button);
        if (collides(click_motion, back_button_motion)) {

            if (current_level_state == LevelState::LEVEL_START) {
                return;
            }

            if (curr_level != (int) LevelState::TERMINATION) {
                save_level_data();
            }
            if (curr_level == 0) {
                tutorial_controller.destroy();
            }
            is_level_over = true; 
            return;
        }

        if (curr_level > 0) {
            Motion save_button_motion = registry.motions.get(save_button);
            if (collides(click_motion, save_button_motion)) {
                if (current_level_state == LevelState::LEVEL_START) {
                    return;
                }
                save_level_data();
                createPromptWithTimer(1000, TEXTURE_ASSET_ID::PROMPT_SAVED);
            }
        }
    }

    switch (current_level_state) {
    case LevelState::PLAYER_TURN:
        // handle all player logic to a player controller
        player_controller.on_mouse_button(button, action, 0, cursor_world_pos);
        break;
    }
}

LevelManager::LevelState LevelManager::current_state()
{
    return this->current_level_state;
}

// state machine functions
void LevelManager::move_to_state(LevelState next_state) {
    // some assersions to make sure state machine are working as expected
    switch (next_state) {
    case LevelState::LEVEL_START:
        std::cout << "moving to level start state" << std::endl;
        assert(this->current_level_state == LevelState::LEVEL_START); break;

    case LevelState::ENEMY_BLINK:
        std::cout << "moving to enemy blink state" << std::endl;
        assert(this->current_level_state == LevelState::LEVEL_START); break;

    case LevelState::PREPARE:
        std::cout << "moving to prepare state" << std::endl;
        assert(this->current_level_state == LevelState::EVALUATION || this->current_level_state == LevelState::ENEMY_BLINK); break;

    case LevelState::PLAYER_TURN:
        std::cout << "moving to player's state" << std::endl;
        assert(this->current_level_state == LevelState::PREPARE); break;

    case LevelState::ENEMY_TURN:
        std::cout << "moving to enemy move state, AI is handling enemy movement" << std::endl;
        assert(this->current_level_state == LevelState::PREPARE); break;

    case LevelState::EVALUATION:
        std::cout << "moving to evaluation state, calculating damages..." << std::endl;
        assert(this->current_level_state == LevelState::PLAYER_TURN || this->current_level_state == LevelState::ENEMY_TURN); break;

    case LevelState::TERMINATION:
        std::cout << "game is over!, press 'ESC' to exit" << std::endl;
        assert(this->current_level_state == LevelState::PREPARE); break;

    default:
        assert(false && "Entered invalid state"); break;
    }

    this->next_level_state = next_state;
}
