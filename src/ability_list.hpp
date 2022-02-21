#include "components.hpp"

// Idea for the attack is that 

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
    2000, // TTL
    100, // Damage
    100, // Range
    (int)GEOMETRY_BUFFER_ID::SQUARE, // shape
    vec2(100, 100), // Size
    true, // Gravit Affected
    1, // Max cooldown
    0, // Current cooldown
};
AttackObject gumball_basic_attack = {
    true, // Chosen attack
    200, // TTL
    50, // Damage
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
