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
void resetEnergy(Entity entity);

Entity createHealthBar(vec2 pos, vec2 size);
void updateHealthBar(Entity entity);

Entity createOrderIndicator();
void updateOrderIndicator(Entity entity);
void removeOrderIndicator();

Entity createEnemy(vec2 pos, vec2 size, float curr_health, float curr_energy, 
        AttackArsenal attack_arsenal, bool slippery, bool damage_over_turn, bool heal_over_turn);
void removeEnemy(Entity entity);

Entity createPlayer(vec2 pos, vec2 size, float curr_health, float curr_energy,
        AttackArsenal attack_arsenal, bool slippery, bool damage_over_turn, BuffArsenal buff_arsenal);

void removePlayer(Entity entity);

Entity createTerrain(vec2 pos, vec2 size, bool breakable, int level);
void removeTerrain(Entity entity);

Entity createAttackObject(Entity attacker, AttackAbility ability, float angle, vec2 pos);
void removeAttackObject(Entity entity);

Entity createCamera(vec2 pos, vec2 offset, vec2 lower_limit, vec2 higher_limit);
void removeCamera(Entity entity);

Entity createBlinkTimer(float timer);
void removeBlinkTimer(Entity entity);

Entity createPromptWithTimer(float timer, TEXTURE_ASSET_ID texture_ID);
void removePromptWithTimer(Entity entity);

Entity createButton(vec2 pos, vec2 size, bool (*on_click)(), TEXTURE_ASSET_ID texture_ID, bool disabled = false);
void removeButton(Entity entity);
Entity createPlayerButton(vec2 pos, vec2 size, bool (*on_click)(), TEXTURE_ASSET_ID texture_ID, bool disabled = false);
void removePlayerButton(Entity entity);
Entity createAbilityButton(vec2 pos, vec2 size, bool (*on_click)(), TEXTURE_ASSET_ID texture_ID, bool disabled = false);
void removeAbilityButton(Entity entity);

// this does not create a new entity but only attach a hit effect on the existing object
Entity createHitEffect(Entity entity, float ttl_ms);
void removeHitEffect(Entity entity);

Entity createBackground(vec2 size, int level);
void removeBackground(Entity entity);

Entity createLadder(vec2 pos, vec2 size);
void removeLadder(Entity entity);

Entity createPrompt(vec2 pos, vec2 size, int step = 0);
void removePrompt(Entity entity);

Entity createStorySlide(vec2 pos, vec2 size, int slide);
void removeStorySlide(Entity entity);

Entity createUI(vec2 pos, vec2 size);
void removeUI(Entity entity);

Entity createCooldown(vec2 pos, int cool_down_left);
void removeCooldown(Entity entity);

