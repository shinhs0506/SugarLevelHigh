// internal
#include "common.hpp"

#include <GLFW/glfw3.h>
#include <cfloat>

#include "level_manager.hpp"
#include "level_init.hpp"
#include "physics_system.hpp"
#include "tiny_ecs_registry.hpp"
#include "attack_system.hpp"

#include <iostream>

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
}

void LevelManager::load_level(int level)
{
    this->curr_level = level;
	// change camera limits based on level
	Camera& camera = registry.cameras.get(main_camera);
	Motion& motion = registry.motions.get(main_camera);
	camera.lower_limit = motion.position - vec2(100);
	camera.higer_limit = motion.position + vec2(100);

    if (level == 0) {
        Entity enemy = createEnemy(vec2(600, 500), vec2(50, 100));
        Entity player = createPlayer(vec2(500, 500), vec2(50, 100));
        Entity terrain = createTerrain(vec2(600, 600), vec2(800, 50));
		Entity button = createButton(vec2(100, 300), vec2(50, 50), simple_attack);

        auto compare = [](Entity& a, Entity& b) {
                Initiative& aInitiative = registry.initiatives.get(a);
                Initiative& bInitiative = registry.initiatives.get(b);
                return aInitiative.value < bInitiative.value;
        };
        registry.initiatives.sort(compare);

        // to retrieve current entity
        // registry.initiatives.entities[currOrderIndex];
        // or 
        // registry.activeTurns.entities[0] from outside level_manager
        // or might add a global Entity variable later
        curr_order_index = 0;
        Entity& current_character = registry.initiatives.entities[curr_order_index];
        registry.activeTurns.emplace(current_character);

        // start with a move state
        state = State::PREPARE;
        num_characters = registry.initiatives.size();
	}
}

void LevelManager::restart_level()
{

}

void LevelManager::abandon_level()
{

}

bool LevelManager::step(float elapsed_ms)
{
    switch (state) {
        case State::PREPARE: 
            {
                // advance turn order
                int num_characters = registry.initiatives.size();
                Entity& current_character = registry.activeTurns.entities[0];
                auto it = find(registry.initiatives.entities.begin(), registry.initiatives.entities.end(), current_character);
                int pos = it - registry.initiatives.entities.begin(); 
                std::cout << pos << std::endl;
                
                Entity& next_character = registry.initiatives.entities[(pos + 1) % num_characters];
                while (registry.healths.get(next_character).dead) {
                    pos = (pos + 1) % num_characters;
                    next_character = registry.initiatives.entities[pos];
                }

                registry.activeTurns.remove(current_character);
                registry.activeTurns.emplace(next_character);

                // remove dead entities (with health component and current health below 0)
                for (uint i = 0; i < registry.healths.size(); i++) {
                    Entity entity = registry.healths.entities[i];
                    Health health = registry.healths.components[i];
                    assert(health.cur_health >= 0.f); // health shouldn't below 0

                    if (health.dead) {
                        // check playables
                        if (registry.playables.has(entity)) {
                            removePlayer(entity);
                        } else if (registry.enemies.has(entity)) {
                            removeEnemy(entity);
                        } else if (registry.terrains.has(entity) && registry.terrains.get(entity).breakable)
                            removeTerrain(entity);
                    }
                }

                if (registry.playables.has(registry.activeTurns.entities[0])) {
                    std::cout << "player is current character, moving to player move state" << std::endl;
                    move_to_state(State::PLAYER_MOVE);
                } else {
                    std::cout << "enemy is current character, moving to enemy move state" << std::endl;
                    move_to_state(State::ENEMY_MOVE);
                }
            }
            break;
        case State::PLAYER_MOVE:
            break;
        case State::PLAYER_ATTCK:
            break;
        case State::ENEMY_MOVE:
            break;
        case State::ENEMY_ATTACK: 
            break;
        case State::EVALUATION:
            // check if all attacks finished
            if (registry.attackObjects.size() == 0 && registry.hitEffects.size() == 0) {
                std::cout << "all attacks and effects cleared, moving to prepare state" << std::endl;
                move_to_state(State::PREPARE);
            }

            break;
    }

    // remove timed out attack objects
    for (uint i = 0; i < registry.attackObjects.size(); i ++) {
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

	return true;
}

void LevelManager::handle_collisions()
{
    switch (state) {
        case State::PREPARE:
            // do nothing
            break;
        case State::PLAYER_MOVE:
            // handle player movement collision
            break;
        case State::PLAYER_ATTCK:
            // do nothing
            break;
        case State::ENEMY_MOVE:
            // handle enemy movement collision
            break;
        case State::ENEMY_ATTACK:
            // do nothing
            break;
        case State::EVALUATION:
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
                        createHitEffect(other_entity, 200); // this ttl should be less then attack object ttl
                    }
                }
            }

            break;
    }

}

bool LevelManager::level_ended()
{
	return ended;
}


void LevelManager::update_ui(vec2 velocity) {
	auto& motion_registry = registry.motions;

	// update camera position
	Motion& camera_motion = motion_registry.get(main_camera);
	camera_motion.velocity += velocity;

	// fix ui overlays relative to camera
	auto& overlays_registry = registry.overlays;
	for (uint i = 0; i < overlays_registry.size(); i++) {
		Entity& entity = overlays_registry.entities[i];
		Motion& motion = motion_registry.get(entity);
		motion.velocity += velocity;
	}
}

void LevelManager::on_key(int key, int, int action, int mod)
{
    switch (state) {
        case State::PREPARE:
            // do nothing
            break;
        case State::PLAYER_MOVE: 
            {
                // move player
                // then move to player attack state
                
                // player horizontal movement logic
                Entity player = registry.activeTurns.entities[0];
                Motion& player_horizontal_movement = registry.motions.get(player);

                if (action == GLFW_PRESS)
                {
                    switch (key)
                    {
                    case GLFW_KEY_A:
                        player_horizontal_movement.velocity += vec2(-CAM_MOVE_SPEED, 0); break;
                    case GLFW_KEY_D:
                        player_horizontal_movement.velocity += vec2(CAM_MOVE_SPEED, 0); break;
                    }
                }
                else if (action == GLFW_RELEASE)
                {
                    switch (key)
                    {
                    case GLFW_KEY_A:
                        player_horizontal_movement.velocity += vec2(CAM_MOVE_SPEED, 0); break;
                    case GLFW_KEY_D:
                        player_horizontal_movement.velocity += vec2(-CAM_MOVE_SPEED, 0); break;

                    }
                }

                if (key == GLFW_KEY_N && action == GLFW_PRESS) {
                    std::cout << "player moved, going to player attack state" << std::endl;
                    move_to_state(State::PLAYER_ATTCK);
                }
            }
            break;
        case State::PLAYER_ATTCK:
            // do nothing
            break;
        case State::ENEMY_MOVE:
            // do nothing, some placeholder functions for now
            if (key == GLFW_KEY_Z) {
                std::cout << "enemy moved, going to enemy attack state" << std::endl;
                move_to_state(State::ENEMY_ATTACK);
            }
            break;
        case State::ENEMY_ATTACK:
            // do nothing, some placeholder functions for now
            if (key == GLFW_KEY_X) {
                std::cout << "enemy attacked, going to evaluation" << std::endl;
                move_to_state(State::EVALUATION);
            }
            break;
        case State::EVALUATION:
            // do nothing
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
			update_ui(vec2(-CAM_MOVE_SPEED, 0)); break;
		case GLFW_KEY_RIGHT:
			update_ui(vec2(CAM_MOVE_SPEED, 0)); break;
		case GLFW_KEY_UP:
			update_ui(vec2(0, -CAM_MOVE_SPEED)); break;
		case GLFW_KEY_DOWN:
			update_ui(vec2(0, CAM_MOVE_SPEED)); break;
		}

	} else if (action == GLFW_RELEASE)
	{
		switch (key)
		{
		case GLFW_KEY_LEFT:
			update_ui(vec2(CAM_MOVE_SPEED, 0)); break;
		case GLFW_KEY_RIGHT:
			update_ui(vec2(-CAM_MOVE_SPEED, 0)); break;
		case GLFW_KEY_UP:
			update_ui(vec2(0, CAM_MOVE_SPEED)); break;
		case GLFW_KEY_DOWN:
			update_ui(vec2(0, -CAM_MOVE_SPEED)); break;
		}
	}

}

void LevelManager::on_mouse_move(vec2 pos)
{

}

void LevelManager::on_mouse_button(int button, int action, int mod)
{
    switch (state) {
        case State::PREPARE:
            // do nothing
            break;
        case State::PLAYER_MOVE:
            // click on attack action to go to PLAYER_ATTACK state
            break;
        case State::PLAYER_ATTCK: {
            // calculate cursor position and it's world position
            double cursor_window_x, cursor_window_y;
            glfwGetCursorPos(window, &cursor_window_x, &cursor_window_y);
            vec2 cursor_window_pos = { cursor_window_x, cursor_window_y };

            vec2 camera_pos = registry.motions.get(main_camera).position;
            vec2 camera_offset = registry.cameras.get(main_camera).offset;

            vec2 cursor_world_pos = cursor_window_pos + camera_pos - camera_offset;

            // tmp use left click for buttons or perform attck only
            if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
                Motion click_motion;
                click_motion.position = cursor_world_pos;
                click_motion.scale = { 1.f, 1.f };

                // check to see if click was on a button first
                for (uint i = 0; i < registry.clickables.size(); i++) {

                    Entity entity = registry.clickables.entities[i];
                    Motion motion = registry.motions.get(entity);

                    if (collides(click_motion, motion)) {
                        registry.clickables.get(entity).on_click();
                        break;
                    }
                }

                Entity player = registry.activeTurns.entities[0];

                // manually calculate a world position with some offsets
                vec2 player_pos = registry.motions.get(player).position;
                
                vec2 direction = cursor_world_pos - player_pos;

                vec2 offset{ 75.f, 0.f }; // a bit before the character
                Transform trans;
                trans.translate(player_pos);
                trans.rotate(-atan2(direction[0], direction[1]) + M_PI/2);
                trans.translate(offset);
                
                vec2 attack_pos = trans.mat * vec3(0, 0, 1);
                createAttackObject(player, GEOMETRY_BUFFER_ID::SQUARE, 50.f, 200, 0, attack_pos, vec2(0, 0), vec2(100, 100));
                std::cout << "craeted attack object" << std::endl;
                std::cout << "player attacked, moving to evaluation state" << std::endl;
                move_to_state(State::EVALUATION);
            }

            break;
        }
        case State::ENEMY_MOVE:
            // do nothing
            break;
        case State::ENEMY_ATTACK:
            // do nothing
            break;
        case State::EVALUATION:
            // do nothing
            break;
    }
    
    // actions to perform regardless of the state
}

// state machine functions
void LevelManager::move_to_state(State state) {
    this->state = state;
}
