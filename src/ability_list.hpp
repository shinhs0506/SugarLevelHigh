#include "components.hpp"

/*
Idea for the attack and ui is that texture/image for the two boxes (for basic and advanced attacks)
are constant for all characters. Image for respective abilities will change based on the active 
character or deactivated if enemy's turn. When box is clicked, it will retrieve active players
abilities and attack from there. (boxes not shown/active during enemy turn)
*/

/*
COOLDOWN
cd has to be > 2 for an actual cooldown. B/c at the start of a players turn, ability cd is reduced by 1
so if a cd is only 1, then on that char's next turn, it'll be 0 (available) anyways.
*/
AttackObject gingerbread_basic_attack = {
    false, // Chosen attack
    200, // TTL
    50, // Damage
    0, // Range
    (int)GEOMETRY_BUFFER_ID::SQUARE, // shape
    vec2(100, 100), // Size
    false, // Gravit Affected
    0, // Max cooldown
    0, // Current cooldown 
};
AttackObject gingerbread_advanced_attack = {
    false, // Chosen attack
    7000, // TTL
    100, // Damage
    300, // Range
    (int)GEOMETRY_BUFFER_ID::SQUARE, // shape
    vec2(100, 50), // Size
    true, // Gravit Affected
    2, // Max cooldown
    0, // Current cooldown
};

// Gumball used for enemy, at the moment, enemy only uses melee attack (the basic attack hardcoded to true)
AttackObject gumball_basic_attack = {
    true, // Chosen attack
    200, // TTL
    20, // Damage
    0, // Range
    (int)GEOMETRY_BUFFER_ID::SQUARE, // shape
    vec2(100, 100), // Size
    false, // Gravit Affected
    0, // Max cooldown
    0, // Current cooldown 
};
AttackObject gumball_advanced_attack = {
    false, // Chosen attack
    2000, // TTL
    100, // Damage
    100, // Range
    (int)GEOMETRY_BUFFER_ID::SQUARE, // shape
    vec2(100, 100), // Size
    false, // Gravit Affected
    1, // Max cooldown
    0, // Current cooldown
};
