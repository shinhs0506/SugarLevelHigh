#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// a red line for debugging purposes
Entity createDebugLine(vec2 position, vec2 size);

Entity createEnergyBar();
void resetEnergyBar();
void updateEnergyBar(Energy energy);
void removeEnergyBar();

Entity createHealthBar(vec2 pos, vec2 size);
void updateHealthBar(Entity entity);

Entity createEnemy(vec2 pos, vec2 size, AttackArsenal attack_arsenal);
void removeEnemy(Entity entity);

Entity createPlayer(vec2 pos, vec2 size, AttackArsenal attack_arsenal);
void removePlayer(Entity entity);

Entity createTerrain(vec2 pos, vec2 size);
void removeTerrain(Entity entity);

Entity createAttackObject(Entity attacker, AttackAbility ability, float angle, vec2 pos);
void removeAttackObject(Entity entity);

Entity createCamera(vec2 pos, vec2 offset, vec2 lower_limit, vec2 higher_limit);
void removeCamera(Entity entity);


Entity createButton(vec2 pos, vec2 size, bool (*on_click)());
void removeButton(Entity entity);

// this does not create a new entity but only attach a hit effect on the existing object
Entity createHitEffect(Entity entity, float ttl_ms);
void removeHitEffect(Entity entity);

Entity createBackground(vec2 size, int level);
void removeBackground(Entity entity);

Entity createLadder(vec2 pos, vec2 size);
void removeLadder(Entity entity);

