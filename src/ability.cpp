#include <stdio.h>

#include "ability.hpp"
#include "level_init.hpp"
#include <iostream>

void mock_callback() { 
	printf("A button was clicked!\n");
}

void mock_basic_attack_callback() {
	printf("Basic attack callback button was clicked!\n");
	Entity active_character = registry.activeTurns.entities[0];
	AttackArsenal& active_arsenal = registry.attackArsenals.get(active_character);
	active_arsenal.basic_attack.activated = true;
	active_arsenal.advanced_attack.activated = false;
}

void mock_advanced_attack_callback() {
	printf("Advanced attack callback button was clicked!\n");
	Entity active_character = registry.activeTurns.entities[0];
	AttackArsenal& active_arsenal = registry.attackArsenals.get(active_character);
	active_arsenal.advanced_attack.activated = true;
	active_arsenal.basic_attack.activated = false;
}

void perform_attack( vec2 offset, vec2 direction) {
	Entity active_character = registry.activeTurns.entities[0];
	AttackArsenal active_arsenal = registry.attackArsenals.get(active_character);

	// manually calculate a world position with some offsets
	vec2 attacker_pos = registry.motions.get(active_character).position;

	Transform trans;
	trans.translate(attacker_pos);
	trans.rotate(-atan2(direction[0], direction[1]) + M_PI / 2);
	trans.translate(offset);

	double angle = -atan2(direction[0], direction[1]) + M_PI / 2;

	vec2 attack_pos = trans.mat * vec3(0, 0, 1);

	AttackObject chosen_attack = (active_arsenal.basic_attack.activated == true) ? active_arsenal.basic_attack : active_arsenal.advanced_attack;


	Entity attack_object = createAttackObject(chosen_attack, angle, attack_pos);

	if (chosen_attack.range > 0) {
		registry.projectiles.emplace(attack_object);
	}

	Motion& attack_object_motion = registry.motions.get(attack_object);
	attack_object_motion.angle = angle;
}

