#pragma once
#include "components.hpp"

/*
Idea for the attack and ui is that texture/image for the two boxes (for basic and advanced attacks)
are constant for all characters. Image for respective abilities will change based on the active 
character or deactivated if enemy's turn. When box is clicked, it will retrieve active players
abilities and attack from there. (boxes not shown/active during enemy turn)
*/

AttackAbility gingerbread_basic_attack = {
    false, // Chosen attack
    0, // speed
    30, // Damage
    0, // Range
    (int)GEOMETRY_BUFFER_ID::SQUARE, // shape
    vec2(100, 100), // Size
    false, // Gravit Affected
    0, // Max cooldown
    0, // Current cooldown 
};
AttackAbility gingerbread_advanced_attack = {
    false, // Chosen attack
    500, // speed
    50, // Damage
    500, // Range
    (int)GEOMETRY_BUFFER_ID::SQUARE, // shape
    vec2(100, 50), // Size
    true, // Gravit Affected
    2, // Max cooldown
    0, // Current cooldown
};

// Gumball used for enemy, at the moment, enemy only uses melee attack (the basic attack active hardcoded to true)
AttackAbility gumball_basic_attack = {
    true, // Chosen attack
    0, // speed
    10, // Damage
    0, // Range
    (int)GEOMETRY_BUFFER_ID::SQUARE, // shape
    vec2(100, 100), // Size
    false, // Gravit Affected
    0, // Max cooldown
    0, // Current cooldown 
};
AttackAbility gumball_advanced_attack = {
    false, // Chosen attack
    500, // speed
    20, // Damage
    100, // Range
    (int)GEOMETRY_BUFFER_ID::SQUARE, // shape
    vec2(100, 100), // Size
    false, // Gravit Affected
    2, // Max cooldown
    0, // Current cooldown
};
