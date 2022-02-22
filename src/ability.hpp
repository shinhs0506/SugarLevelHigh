#pragma once
#include "tiny_ecs.hpp"
#include "tiny_ecs_registry.hpp"

void mock_callback();
void mock_basic_attack_callback();
void mock_advanced_attack_callback();

void perform_attack(vec2 attacker_pos, vec2 offset, vec2 direction, AttackObject chosen_attack);
