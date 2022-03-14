#include <stdio.h>

#include "ability.hpp"
#include "level_init.hpp"
#include <iostream>

bool mock_callback() { 
	printf("A button was clicked!\n");
	return true;
}

// Basic attack will always return true b/c basic attacks will always have no cooldown
bool mock_basic_attack_callback() { 
    printf("Basic attack callback button was clicked!\n"); 
    Entity active_character = registry.activeTurns.entities[0]; 
    AttackArsenal& attack_arsenal = registry.attackArsenals.get(active_character); 
 	attack_arsenal.basic_attack.activated = true; 
 	attack_arsenal.advanced_attack.activated = false; 
 	return true; 
 } 

bool mock_advanced_attack_callback() { 
    printf("Advanced attack callback button was clicked!\n"); 
 	Entity active_character = registry.activeTurns.entities[0]; 
 	AttackArsenal& attack_arsenal = registry.attackArsenals.get(active_character); 
 	if (attack_arsenal.advanced_attack.current_cooldown == 0) { 
 		attack_arsenal.advanced_attack.activated = true; 
 		attack_arsenal.basic_attack.activated = false;
 		return true; 
 	} 
 	std::cout << "Attack on cool down" << std::endl; 
 	return false; 
} 

bool mock_heal_callback() {
    printf("Heal callback button was clicked!\n"); 
    Entity active_character = registry.activeTurns.entities[0]; 
    BuffArsenal& buff_arsenal = registry.buffArsenals.get(active_character);
    if (buff_arsenal.heal.current_cooldown == 0) {
        return true;
    }
    std::cout << "Heal on cool down" << std::endl; 
    return false;
}

void perform_attack(Entity attacker, vec2 attacker_pos, vec2 offset, vec2 direction, AttackAbility chosen_attack) {
    
	// manually calculate a world position with some offsets
	double angle = -atan2(direction[0], direction[1]) + M_PI / 2;
	Transform trans;
	trans.translate(attacker_pos);
	trans.rotate(angle);
	trans.translate(offset);
	vec2 attack_pos = trans.mat * vec3(0, 0, 1);

	// Creating the actual attack object
	Entity attack_object = createAttackObject(attacker, chosen_attack, angle, attack_pos);

	// If the attack has a range > 0 then its a projectile and NOT melee
	if (chosen_attack.range > 0) {
		registry.projectiles.emplace(attack_object);
	}

	// Sets angle of attack
	Motion& attack_object_motion = registry.motions.get(attack_object);
	attack_object_motion.angle = angle;


	advance_ability_cd(attacker);
}

void perform_buff_ability(Entity player) {
    BuffArsenal& buffs = registry.buffArsenals.get(player);
    
    // only 1 type of buff ability for now
    // update health
    Health& health = registry.healths.get(player);     
    health.cur_health += buffs.heal.health_delta;
    if (health.cur_health > health.max_health) {
        health.cur_health = health.max_health;
    }

    // update speed
    Motion& motion = registry.motions.get(player);
    motion.speed += buffs.heal.movement_speed_delta;

    buffs.heal.current_cooldown = buffs.heal.max_cooldown;
}

vec2 offset_position(vec2 direction, vec2 player_pos, double angle) {
	vec2 offset{ 100.f, 0.f }; // a bit before the character
	Transform trans;
	trans.translate(player_pos);
	trans.rotate(angle);
	trans.translate(offset);
	vec2 attack_preview_pos = trans.mat * vec3(0, 0, 1);
	return attack_preview_pos;
}

void create_preview_object(vec2 player_pos) {

	auto entity = Entity();

	vec2 direction = -player_pos;
	double angle = -atan2(direction[0], direction[1]) + M_PI / 2;

	vec2 attack_preview_pos = offset_position(direction, player_pos, angle);

	Motion& motion = registry.motions.emplace(entity);
	motion.position = attack_preview_pos;
	motion.angle = angle;
	motion.goal_velocity = vec2(0, 0);
	motion.scale = vec2(75, 20);
	motion.depth = DEPTH::ATTACK;
	motion.gravity_affected = false;

	registry.attackPreviews.emplace(entity);

    AttackArsenal attack_arsenal = registry.attackArsenals.get(registry.activeTurns.entities[0]);
    if (attack_arsenal.basic_attack.activated == true){
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::BASIC_ATTACK_PREVIEW,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
    } else {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::ADVANCED_ATTACK_PREVIEW, 
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
    }

	registry.colors.emplace(entity, vec3(1.f, 0.f, 0.f));
}

void destroy_preview_objects() {
	Entity attack_preview = registry.attackPreviews.entities[0];
	registry.motions.remove(attack_preview);
	registry.attackPreviews.remove(attack_preview);
	registry.renderRequests.remove(attack_preview);
	registry.colors.remove(attack_preview);
}

void advance_ability_cd(Entity entity) {
	AttackArsenal& attack_arsenal = registry.attackArsenals.get(entity);
    if (attack_arsenal.basic_attack.current_cooldown > 0) {
        attack_arsenal.basic_attack.current_cooldown -= 1;
    }
    if (attack_arsenal.advanced_attack.current_cooldown > 0) {
        attack_arsenal.advanced_attack.current_cooldown -= 1;
    }

    if (registry.playables.has(entity)) {
        BuffArsenal& buff_arsenal = registry.buffArsenals.get(entity);
        if (buff_arsenal.heal.current_cooldown > 0) {
            buff_arsenal.heal.current_cooldown -= 1;
        }
    }

	printf("advanced ability cd\n");
}

bool advanced_attack_available(AttackArsenal arsenal) {
	return arsenal.advanced_attack.current_cooldown == 0;
}

bool basic_attack_available(AttackArsenal arsenal) {
	return arsenal.advanced_attack.current_cooldown == 0;
}
