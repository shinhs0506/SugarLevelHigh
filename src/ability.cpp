#include <stdio.h>

#include "ability.hpp"
#include "level_init.hpp"

void mock_callback() {
	printf("A button was clicked!\n");
}

void melee_attack(Entity attacker, float damage, vec2 offset, vec2 direction, vec2 size) {
	// manually calculate a world position with some offsets
	vec2 attacker_pos = registry.motions.get(attacker).position;

	Transform trans;
	trans.translate(attacker_pos);
	trans.rotate(-atan2(direction[0], direction[1]) + M_PI / 2);
	trans.translate(offset);

	vec2 attack_pos = trans.mat * vec3(0, 0, 1);
	createAttackObject(attacker, GEOMETRY_BUFFER_ID::SQUARE, damage, 200, 0, 
		attack_pos, vec2(0, 0), size);
}