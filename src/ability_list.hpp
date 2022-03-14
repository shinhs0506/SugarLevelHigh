#pragma once
#include "components.hpp"

/*
Idea for the attack and ui is that texture/image for the two boxes (for basic and advanced attacks)
are constant for all characters. Image for respective abilities will change based on the active 
character or deactivated if enemy's turn. When box is clicked, it will retrieve active players
abilities and attack from there. (boxes not shown/active during enemy turn)
*/

AttackAbility gingerbread_basic_attack = {
    .activated = false,
    .speed = 0,
    .damage = 20,
    .range = 0,
    .shape = (int)GEOMETRY_BUFFER_ID::SQUARE, 
    .size = vec2(100, 100),
    .gravity_affected = false,
    .max_cooldown = 0,
    .current_cooldown = 0
};

AttackAbility gingerbread_advanced_attack = {
    .activated = false,
    .speed = 500,
    .damage = 50,
    .range = 500,
    .shape = (int)GEOMETRY_BUFFER_ID::SQUARE, 
    .size = vec2(100, 50),
    .gravity_affected = true,
    .max_cooldown = 2,
    .current_cooldown = 0
};

BuffAbility gingerbread_heal_buff = {
    .movement_speed_delta = 0,
    .health_delta = 20,
    .max_cooldown = 3,
    .current_cooldown = 0
};

// Gumball used for enemy, at the moment, enemy only uses melee attack (the basic attack active hardcoded to true)
AttackAbility gumball_basic_attack = {
    .activated = false,
    .speed = 0,
    .damage = 20,
    .range = 0,
    .shape = (int)GEOMETRY_BUFFER_ID::SQUARE, 
    .size = vec2(100, 100),
    .gravity_affected = false,
    .max_cooldown = 0,
    .current_cooldown = 0
};

AttackAbility gumball_advanced_attack = {
    .activated = false,
    .speed = 500,
    .damage = 40,
    .range = 100,
    .shape = (int)GEOMETRY_BUFFER_ID::SQUARE, 
    .size = vec2(100, 100),
    .gravity_affected = false,
    .max_cooldown = 2,
    .current_cooldown = 0
};
