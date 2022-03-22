#pragma once
#include "tiny_ecs.hpp"
#include "tiny_ecs_registry.hpp"

const float MELEE_ATTACK_TTL = 200; // ms

bool mock_callback();
bool mock_basic_attack_callback();
bool mock_advanced_attack_callback();
bool mock_heal_callback();

void perform_attack(Entity attacker, vec2 attacker_pos, vec2 offset, vec2 direction, AttackAbility chosen_attack);
void perform_buff_ability(Entity player);

vec2 offset_position(vec2 direction, vec2 player_pos, double angle);
void create_preview_object(vec2 player_pos);
void destroy_preview_objects();

void advance_ability_cd(Entity entity);

bool advanced_attack_available(AttackArsenal arsenal);
bool basic_attack_available(AttackArsenal arsenal);
