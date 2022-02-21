#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// a red line for debugging purposes
Entity createDebugLine(vec2 position, vec2 size);

Entity createEnemy(vec2 pos, vec2 size, AttackArsenal attack_arsenal);
void removeEnemy(Entity entity);

Entity createPlayer(vec2 pos, vec2 size, AttackArsenal attack_arsenal);
void removePlayer(Entity entity);

Entity createTerrain(vec2 pos, vec2 size);
void removeTerrain(Entity entity);

Entity createAttackObject(AttackObject attack, float angle, vec2 pos);
//Entity createAttackObject(Entity attacker, GEOMETRY_BUFFER_ID shape,
//	float damage, float ttl, float angle, vec2 pos, vec2 velocity, vec2 size, bool gravity_affected);
void removeAttackObject(Entity entity);

Entity createCamera(vec2 pos, vec2 offset, vec2 lower_limit, vec2 higher_limit);
void removeCamera(Entity entity);


Entity createButton(vec2 pos, vec2 size, void (*on_click)());
void removeButton(Entity entity);

// this does not create a new entity but only attach a hit effect on the existing object
Entity createHitEffect(Entity entity, float ttl_ms);
void removeHitEffect(Entity entity);

Entity createBackground(vec2 size, int level);
void removeBackground(Entity entity);

