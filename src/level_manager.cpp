// internal
#include "common.hpp"

#include <GLFW/glfw3.h>
#include <cfloat>
#include <vector>
#include <iostream>

#include "level_manager.hpp"
#include "level_init.hpp"
#include "physics_system.hpp"
#include "tiny_ecs_registry.hpp"
#include "ability.hpp"

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

    // start with a move state
    this->current_level_state = LevelState::PREPARE;
    this->next_level_state = LevelState::PREPARE;

    is_level_over = false;
}

bool compare(Entity a, Entity b) {
    auto a_init = registry.initiatives.get(a);
    auto b_init = registry.initiatives.get(b);
    return a_init.value < b_init.value;
};

void LevelManager::load_level(int level)
{
    this->curr_level = level;

    Camera& camera = registry.cameras.get(main_camera);
    Motion& motion = registry.motions.get(main_camera);

    if (level == 0) {
        // change camera limits based on level
        camera.lower_limit = motion.position - vec2(100);
        camera.higer_limit = motion.position + vec2(100);

        Entity background = createBackground(vec2(1480, 920), level);
        Entity player = createPlayer(vec2(500, 500), vec2(80, 100));
        Entity enemy = createEnemy(vec2(600, 500), vec2(80, 100));
        Entity button = createButton(vec2(100, 300), vec2(50, 50), mock_callback);

        level_entity_vector.push_back(background);
        level_entity_vector.push_back(enemy);
        level_entity_vector.push_back(player);
        level_entity_vector.push_back(button);

        float terrain_x_offset = 0.f;
        while (terrain_x_offset < 1200.f) {
            Entity curr = createTerrain(vec2(100.001 + terrain_x_offset, 600.001), vec2(100, 100));
            terrain_vector.push_back(curr);
            level_entity_vector.push_back(curr);
            terrain_x_offset += 100.001;
        }
        float terrain_y_offset = 100.001;
        while (terrain_y_offset < 200.f) {
            Entity curr1 = createTerrain(vec2(100.001, 600.001 - terrain_y_offset), vec2(100, 100));
            Entity curr2 = createTerrain(vec2(1200.001, 600.001 - terrain_y_offset), vec2(100, 100));
            terrain_vector.push_back(curr1);
            terrain_vector.push_back(curr2);
            level_entity_vector.push_back(curr1);
            level_entity_vector.push_back(curr2);
            terrain_y_offset += 100.001;
        }

        order_vector.push_back(enemy);
        order_vector.push_back(player);

        sort(order_vector.begin(), order_vector.end(), compare);
        // to retrieve current entity
        // registry.initiatives.entities[currOrderIndex];
        // or 
        // registry.activeTurns.entities[0] from outside level_manager
        // or might add a global Entity variable later
        // should_initialize_active_turn = true means game has just started
        curr_order_ind = 0;
        should_initialize_active_turn = true;
    }
}

void LevelManager::restart_level()
{

}

void LevelManager::abandon_level()
{
    for (auto& entity : level_entity_vector) {
        registry.remove_all_components_of(entity); 
    }
    terrain_vector.clear();
    level_entity_vector.clear();
    order_vector.clear();
}

void LevelManager::remove_character(Entity entity)
{
    auto it = std::lower_bound(order_vector.begin(), order_vector.end(), entity, compare);
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

    bool only_player_left = registry.playables.size() == registry.initiatives.size();
    bool only_enemy_left = registry.initiatives.size() == registry.enemies.size();
    switch (current_level_state) {
    case LevelState::PREPARE:
        // check whether level completed/failed
        if (only_player_left || only_enemy_left) {
            move_to_state(LevelState::TERMINATION);
            break;
        }

        // determine turn order
        if (should_initialize_active_turn) {
            registry.activeTurns.emplace(order_vector[0]);
            registry.motions.get(order_vector[0]).depth = DEPTH::ACTIVE; 
            should_initialize_active_turn = false;
        }
        else {
            // advance turn order & give active character closest depth
            int num_characters = registry.initiatives.size();

            registry.motions.get(order_vector[curr_order_ind]).depth = DEPTH::CHARACTER;
            curr_order_ind = (curr_order_ind + 1) % num_characters;
            Entity& next_character = order_vector[curr_order_ind];

            registry.activeTurns.clear();
            registry.activeTurns.emplace(next_character);
            registry.motions.get(next_character).depth = DEPTH::ACTIVE;
        }

        if (registry.playables.has(registry.activeTurns.entities[0])) {
            std::cout << "player is current character" << std::endl;
            // reset player controller
            player_controller.reset(registry.activeTurns.entities[0]);
            move_to_state(LevelState::PLAYER_TURN);
        }
        else {
            std::cout << "enemy is current character" << std::endl;
            move_to_state(LevelState::ENEMY_MOVE);
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

    case LevelState::EVALUATION:
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
        break;
    }

    return true;
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

            if (registry.terrains.has(other_entity) && !registry.terrains.get(other_entity).breakable) {
                continue;
            }

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
                Health& health = registry.healths.get(other_entity);
                // health shouldn't be below zero
                health.cur_health = clamp(health.cur_health - attack.damage, 0.f, FLT_MAX);
                attack.attacked.insert(other_entity);

                // change health bar length
                Entity healthBar;
                if (registry.playables.has(other_entity)) {
                    Playable& playable = registry.playables.get(other_entity);
                    healthBar = playable.healthBar;
                }
                if (registry.enemies.has(other_entity)) {
                    Enemy& enemy = registry.enemies.get(other_entity);
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

                createHitEffect(other_entity, 200); // this ttl should be less then attack object ttl
            }
        }

        // handle character and terrain collisions
        if (registry.terrains.has(entity)) {
            Entity other_entity = registry.collisions.components[i].other;
            if (registry.playables.has(other_entity) || registry.enemies.has(other_entity)) {
                Motion& position = registry.motions.get(other_entity);
                position.position = position.prev_position;
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

}

void LevelManager::on_mouse_button(int button, int action, int mod)
{
    double cursor_window_x, cursor_window_y;
    glfwGetCursorPos(window, &cursor_window_x, &cursor_window_y);
    vec2 cursor_window_pos = { cursor_window_x, cursor_window_y };

    vec2 camera_pos = registry.motions.get(main_camera).position;
    vec2 camera_offset = registry.cameras.get(main_camera).offset;

    vec2 cursor_world_pos = cursor_window_pos + camera_pos - camera_offset;

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

    case LevelState::ENEMY_MOVE:
        std::cout << "moving to enemy move state, AI is handling enemy movement" << std::endl;
        assert(this->current_level_state == LevelState::PREPARE || this->current_level_state == LevelState::ENEMY_ATTACK); break;

    case LevelState::ENEMY_ATTACK:
        std::cout << "moving to enemy attack state, AI is handling enemy attack" << std::endl;
        assert(this->current_level_state == LevelState::ENEMY_MOVE); break;

    case LevelState::EVALUATION:
        std::cout << "moving to evaluation state, calculating damages..." << std::endl;
        assert(this->current_level_state == LevelState::PLAYER_TURN || this->current_level_state == LevelState::ENEMY_ATTACK); break;

    case LevelState::TERMINATION:
        std::cout << "game is over!, press 'ESC' to exit" << std::endl;
        assert(this->current_level_state == LevelState::PREPARE); break;

    default:
        assert(false && "Entered invalid state"); break;
    }

    this->next_level_state = next_state;
}
