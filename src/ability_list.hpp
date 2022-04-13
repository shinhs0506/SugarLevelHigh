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
    0, // curr cooldown
    (int)TEXTURE_ASSET_ID::MELEE_ATTACK,
};

AttackAbility gingerbread_advanced_attack = {
    false, // chosen
    500, // speed
    50, // damage
    500, // range
    (int)GEOMETRY_BUFFER_ID::SQUARE, // shape
    vec2(60, 40), // Size
    true, // Gravit Affected
    2, // Max cooldown
    0, // Current cooldown
    (int)TEXTURE_ASSET_ID::BEAR_ADVANCED_ATTACK,
};

BuffAbility gingerbread_heal_buff = {
    0, // speed delta
    40, // health delta
    0, // damage delta
    3, // max cooldown
    0 // curr cooldown
};

// Chocolate Ball used for enemy
AttackAbility chocolateball_basic_attack = {
    true, // Chosen attack
    0, // speed
    20, // Damage
    0, // Range
    (int)GEOMETRY_BUFFER_ID::SQUARE, // shape
    vec2(100, 100), // Size
    false, // Gravit Affected
    0, // Max cooldown
    0, // Current cooldown 
    (int)TEXTURE_ASSET_ID::MELEE_ATTACK,
};

AttackAbility chocolateball_advanced_attack = {
    false, // Chosen attack
    500, // speed
    40, // Damage
    100, // Range
    (int)GEOMETRY_BUFFER_ID::SQUARE, // shape
    vec2(70, 50), // Size
    false, // Gravit Affected
    2, // Max cooldown
    0, // Current cooldown
    (int)TEXTURE_ASSET_ID::CHOCOLATE_ADVANCED_ATTACK,
};

BuffAbility enemy_heal_buff = {
    0, // speed delta
    20, // health delta
    0, // damage delta
    1, // max cooldown
    0 // curr cooldown
};
