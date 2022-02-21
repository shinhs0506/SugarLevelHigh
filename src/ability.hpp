#pragma once
#include "tiny_ecs.hpp"
#include "tiny_ecs_registry.hpp"

void mock_callback();
void mock_basic_attack_callback();
void mock_advanced_attack_callback();

// TODO: those attacks should be associated with abilities later
void basic_attack(Entity attacker, float damage, vec2 offset, vec2 direction, vec2 size, bool gravity_affected);
void advanced_attack(Entity attacker, float damage, vec2 offset, vec2 direction, vec2 size, bool gravity_affected);
