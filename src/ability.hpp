#pragma once
#include "tiny_ecs.hpp"
#include "tiny_ecs_registry.hpp"

bool mock_callback();
bool mock_basic_attack_callback();
bool mock_advanced_attack_callback();

void perform_attack(Entity attacker, vec2 attacker_pos, vec2 offset, vec2 direction, AttackAbility chosen_attack);

vec2 offset_position(vec2 direction, vec2 player_pos, double angle);
void create_preview_object(vec2 player_pos);
void destroy_preview_objects();
