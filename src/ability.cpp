#include <stdio.h>

#include "ability.hpp"
#include "level_init.hpp"

void mock_callback() { 
	printf("A button was clicked!\n");
}

void mock_basic_attack_callback() {
	printf("Basic attack callback button was clicked!\n");
}

void mock_advanced_attack_callback() {
	printf("Advanced attack callback button was clicked!\n");
}


void basic_attack(Entity attacker, float damage, vec2 offset, vec2 direction, vec2 size, bool gravity_affected) {
	// manually calculate a world position with some offsets
	vec2 attacker_pos = registry.motions.get(attacker).position;

	Transform trans;
	trans.translate(attacker_pos);
	trans.rotate(-atan2(direction[0], direction[1]) + M_PI / 2);
	trans.translate(offset);

	vec2 attack_pos = trans.mat * vec3(0, 0, 1);
	createAttackObject(attacker, GEOMETRY_BUFFER_ID::SQUARE, damage, 200, 0, 
		attack_pos, vec2(0, 0), size, gravity_affected);
}

void advanced_attack(Entity attacker, float damage, vec2 offset, vec2 direction, vec2 size, bool gravity_affected) {
	// manually calculate a world position with some offsets
	vec2 attacker_pos = registry.motions.get(attacker).position;

	Transform trans;
	trans.translate(attacker_pos);
	trans.rotate(-atan2(direction[0], direction[1]) + M_PI / 2);
	trans.translate(offset);

	double angle = -atan2(direction[0], direction[1]) + M_PI / 2;
	//vec2((current_speed * 100.f) * -(float)cos(chosen_chicken_motion.angle), (current_speed * 100.f) * -(float)sin(chosen_chicken_motion.angle));


	vec2 attack_pos = trans.mat * vec3(0, 0, 1);

	//vec2((current_speed * 100.f) * -(float)cos(chosen_chicken_motion.angle), (current_speed * 100.f) * -(float)sin(chosen_chicken_motion.angle));

	float power = 300.0f;
	vec2 attack_object_velocity = vec2(power * (float)cos(angle), power * (float)sin(angle));

	Entity advance_object_attack = createAttackObject(attacker, GEOMETRY_BUFFER_ID::SQUARE, damage, 7000, 0,
		attack_pos, attack_object_velocity, size, gravity_affected);
	registry.projectiles.emplace(advance_object_attack);
	Motion& advance_attack_motion = registry.motions.get(advance_object_attack);
	advance_attack_motion.angle = angle;

}
