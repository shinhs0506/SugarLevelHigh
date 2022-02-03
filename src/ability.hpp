#pragma once
#include "tiny_ecs.hpp"
#include "tiny_ecs_registry.hpp"

void mock_callback();

// TODO: those attacks should be associated with abilities later
void melee_attack(Entity attacker, float damage, vec2 offset, vec2 direction, vec2 size);
