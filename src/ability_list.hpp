#pragma once
#include "components.hpp"

/*
Idea for the attack and ui is that texture/image for the two boxes (for basic and advanced attacks)
are constant for all characters. Image for respective abilities will change based on the active 
character or deactivated if enemy's turn. When box is clicked, it will retrieve active players
abilities and attack from there. (boxes not shown/active during enemy turn)
*/

AttackAbility gingerbread_basic_attack = {
    false, // chosen
    0, // speed
    20, // damage
    0, // range    
    (int)GEOMETRY_BUFFER_ID::SQUARE, // shape
    vec2(100, 100), // size
    false, // gravity affected
    0, // max cooldown
    0 // curr cooldown
};

AttackAbility gingerbread_advanced_attack = {
    false, // chosen
    500, // speed
    50, // damage
    500, // range
    (int)GEOMETRY_BUFFER_ID::SQUARE, // shape
    vec2(100, 50), // size
    true, // gravity affected
    2, // max cooldown
    0 // curr cooldown
};

BuffAbility gingerbread_heal_buff = {
    0, // speed delta
    20, // health delta
    3, // max cooldown
    0 // curr cooldown
};

// Gumball used for enemy, at the moment, enemy only uses melee attack (the basic attack active hardcoded to true)
AttackAbility gumball_basic_attack = {
    false,
    0,
    20,
    0,
    (int)GEOMETRY_BUFFER_ID::SQUARE, 
    vec2(100, 100),
    false,
    0,
    0
};

AttackAbility gumball_advanced_attack = {
    false,
    500,
    40,
    100,
    (int)GEOMETRY_BUFFER_ID::SQUARE, 
    vec2(100, 100),
    false,
    2,
    0
};
