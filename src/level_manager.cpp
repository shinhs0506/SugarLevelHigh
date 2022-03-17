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


LevelManager::LevelManager()
{

}

LevelManager::~LevelManager()
{

}

void LevelManager::init(GLFWwindow* window)
{
    this->window = window;
    this->main_camera = registry.cameras.entities[0]; // currently we only have one camera

    is_level_over = false;

    this->current_level_state = LevelState::PREPARE;
    this->next_level_state = LevelState::PREPARE;
}

void LevelManager::init_data(int level){
    Camera& camera = registry.cameras.get(main_camera);
    Motion& motion = registry.motions.get(main_camera);

    reload_manager.load(level);

    CameraData camera_data = reload_manager.get_camera_data();
    camera.lower_limit = motion.position + camera_data.lower_limit_delta;
    camera.higer_limit = motion.position + camera_data.upper_limit_delta;

    BackgroundData background_data = reload_manager.get_background_data();
    background = createBackground(background_data.size, level);

    for (auto& player_data: reload_manager.get_player_data()) {
        gingerbread_advanced_attack.current_cooldown = player_data.advanced_attack_cooldown;
        AttackArsenal ginerbread_arsenal = { gingerbread_basic_attack, gingerbread_advanced_attack};
        Entity player = createPlayer(player_data.pos, player_data.size, player_data.health, 
                player_data.energy, ginerbread_arsenal);
        update_healthbar_len_color(player);
        order_vector.push_back(player);
    }

    for (auto& enemy_data: reload_manager.get_enemy_data()) {
        gumball_advanced_attack.current_cooldown = enemy_data.advanced_attack_cooldown;
        AttackArsenal gumball_arsenal = { gumball_basic_attack, gumball_advanced_attack };
        Entity enemy = createEnemy(enemy_data.pos, enemy_data.size, enemy_data.health, 
                enemy_data.energy, gumball_arsenal);
        update_healthbar_len_color(enemy);
        order_vector.push_back(enemy);
    }

    for (auto& terrain_data: reload_manager.get_terrain_data()) {
        Entity terrain = createTerrain(terrain_data.pos, terrain_data.size);
    }

    curr_order_ind = reload_manager.get_curr_order_ind();

    for (auto& ladder_data : reload_manager.get_ladder_data()) {
        Entity ladder = createLadder(ladder_data.pos, ladder_data.size);
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

    // level specific logic
    if (level == 0) {
        this->init_data(level);
    }

    // common to all levels
    
    back_button = createBackButton(vec2(100, 50), vec2(50,50), NULL); 

    basic_attack_button = createButton(vec2(100, 300), vec2(50, 50), mock_basic_attack_callback);
    advanced_attack_button = createButton(vec2(100, 375), vec2(50, 50), mock_advanced_attack_callback);

    energy_bar = createEnergyBar();

    sort(order_vector.begin(), order_vector.end(), compare);

    // update energy bar
}

void LevelManager::restart_level()
{

}

void LevelManager::save_level_data(){
    reload_manager.save(curr_level);
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

    removeButton(back_button);
    removeButton(basic_attack_button);
    removeButton(advanced_attack_button);

    removeEnergyBar();
    removeBackground(background);

    registry.activeTurns.clear();

    order_vector.clear();
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
    if (order_vector[pos] == registry.activeTurns.entities[0])
    {
        move_to_state(LevelState::EVALUATION);
    }
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
        AttackArsenal& player_arsenal = registry.attackArsenals.get(player);
        PlayerData pd {
            player_motion.position,
            player_motion.scale,
            player_health.cur_health,
            player_energy.cur_energy,
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

    // remove dead entities (with health component and current health below 0)
    for (uint i = 0; i < registry.healths.size(); i++) {
        Entity entity = registry.healths.entities[i];
        Health health = registry.healths.components[i];
        assert(health.cur_health >= 0.f); // health shouldn't below 0

        if (health.dead) {
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
    case LevelState::PREPARE:
        {
            // check whether level completed/failed
            if (only_player_left || only_enemy_left) {
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
                player_controller.start_turn(registry.activeTurns.entities[0]);

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
            move_to_state(LevelState::EVALUATION);
        }
        break;

    case LevelState::ENEMY_TURN:
        // step player controller
        enemy_controller.step(elapsed_ms);
        if (enemy_controller.should_end_enemy_turn())
        {
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
    healthBar_motion.scale = { healthBar_motion.scale.x * (health.cur_health / health.max_health), 10 };

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

                // change health bar length
                update_healthbar_len_color(other_entity);

                createHitEffect(other_entity, 200); // this ttl should be less then attack object ttl
          
            }
        }
    }
}

bool LevelManager::is_over() {
    return is_level_over;
}

void LevelManager::update_camera(vec2 velocity) {
    auto& motion_registry = registry.motions;

    // update camera position
    Motion& camera_motion = motion_registry.get(main_camera);
    camera_motion.velocity += velocity;

}

void LevelManager::on_key(int key, int scancode, int action, int mod)
{
    switch (current_level_state) {
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
    Motion& camera_motion = registry.motions.get(main_camera);
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_LEFT:
            update_camera(vec2(-CAM_MOVE_SPEED, 0)); break;
        case GLFW_KEY_RIGHT:
            update_camera(vec2(CAM_MOVE_SPEED, 0)); break;
        case GLFW_KEY_UP:
            update_camera(vec2(0, -CAM_MOVE_SPEED)); break;
        case GLFW_KEY_DOWN:
            update_camera(vec2(0, CAM_MOVE_SPEED)); break;
        }

    }
    else if (action == GLFW_RELEASE)
    {
        switch (key)
        {
        case GLFW_KEY_LEFT:
            update_camera(vec2(CAM_MOVE_SPEED, 0)); break;
        case GLFW_KEY_RIGHT:
            update_camera(vec2(-CAM_MOVE_SPEED, 0)); break;
        case GLFW_KEY_UP:
            update_camera(vec2(0, CAM_MOVE_SPEED)); break;
        case GLFW_KEY_DOWN:
            update_camera(vec2(0, -CAM_MOVE_SPEED)); break;
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

void LevelManager::on_mouse_button(int button, int action, int mod)
{
    double cursor_window_x, cursor_window_y;
    glfwGetCursorPos(window, &cursor_window_x, &cursor_window_y);
    vec2 cursor_window_pos = { cursor_window_x, cursor_window_y };

    vec2 camera_pos = registry.motions.get(main_camera).position;
    vec2 camera_offset = registry.cameras.get(main_camera).offset;

    vec2 cursor_world_pos = cursor_window_pos + camera_pos - camera_offset;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        Motion click_motion;
        click_motion.position = cursor_world_pos;
        click_motion.scale = { 1.f, 1.f };

        Motion back_button_motion = registry.motions.get(back_button);

        if (collides(click_motion, back_button_motion)) {
            // move to IN_LEVEL state

            if (curr_level != (int) LevelState::TERMINATION) {
                save_level_data();
            }
        is_level_over = true; 
        return;
        }
    }

    switch (current_level_state) {
    case LevelState::PLAYER_TURN:
        // handle all player logic to a player controller
        player_controller.on_mouse_button(button, action, mod, cursor_world_pos);
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
    case LevelState::PREPARE:
        std::cout << "moving to prepare state" << std::endl;
        assert(this->current_level_state == LevelState::EVALUATION); break;

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
