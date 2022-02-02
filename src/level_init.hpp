#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// a red line for debugging purposes
Entity createDebugLine(vec2 position, vec2 size);

Entity createEnemy(vec2 pos, vec2 size);
void removeEnemy(Entity entity);

Entity createPlayer(vec2 pos, vec2 size);
void removePlayer(Entity entity);

Entity createTerrain(vec2 pos, vec2 size);
void removeTerrain(Entity entity);

Entity createAttackObject(Entity attacker, GEOMETRY_BUFFER_ID shape,
	float damage, float ttl, float angle, vec2 pos, vec2 velocity, vec2 size);
void removeAttackObject(Entity entity);

Entity createCamera(vec2 pos, vec2 offset);
void removeCamera(Entity entity);


Entity createButton(vec2 pos, vec2 size, void (*on_click)());
void removeButton(Entity entity);

Entity createMouseEvent(vec2 pos);
void removeMouseEvent(Entity entity);

// this does not create a new entity but only attach a hit effect on the existing object
Entity createHitEffect(Entity entity, float ttl_ms);
void removeHitEffect(Entity entity);

