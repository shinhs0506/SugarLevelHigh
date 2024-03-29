#include "level_init.hpp"
#include "tiny_ecs_registry.hpp"
#include "game_init.hpp"
#include "ability.hpp"
#include <iostream>

Entity createDebugLine(vec2 position, vec2 scale)
{
	Entity entity = Entity();

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.goal_velocity = { 0, 0 };
	motion.position = position;
	motion.prev_position = position;
	motion.scale = scale;

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
		 EFFECT_ASSET_ID::COLOURED,
		 GEOMETRY_BUFFER_ID::DEBUG_LINE });

	registry.colors.emplace(entity, vec3(1.f, 0.f, 0.f));

	registry.debugComponents.emplace(entity);
	return entity;
}

Entity createSnow(vec2 pos, vec2 velocity, vec2 size, TEXTURE_ASSET_ID texture)
{
	auto entity = Entity();

	registry.snows.emplace(entity);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = { pos };
	motion.prev_position = { pos };
	motion.angle = 0.f;
	motion.goal_velocity = { velocity };
	motion.scale = { size };
	motion.gravity_affected = false;
	motion.depth = DEPTH::BACKGROUND;

	registry.renderRequests.insert(
		entity,
		{ texture,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

void removeSnow(Entity snow) {
	registry.motions.remove(snow);
	registry.renderRequests.remove(snow);
	registry.snows.remove(snow);
}

Entity createEnergyBar()
{
	auto entity = Entity();

	registry.energyBars.emplace(entity);
	vec2 pos = vec2(694, 672); // subject to change when adjusting UI positions

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = { pos };
	motion.prev_position = { pos };
	motion.angle = 0.f;
	motion.goal_velocity = { 0.f, 0.f };
	motion.scale = { 400, 20 };
	motion.gravity_affected = false;
	motion.depth = DEPTH::UI_TOP;

	Overlay overlay{ pos, pos };
	registry.overlays.insert(entity, overlay);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BAR,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	registry.colors.emplace(entity, vec3(1.f, 0.f, 0.f));

	return entity;
}

Entity createOrderIndicator(){
	auto entity = Entity();

	registry.orderIndicators.emplace(entity);
	vec2 pos = vec2(100000, 100000); // subject to change when adjusting UI positions, out of screen when started
	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = { pos };
	motion.prev_position = { pos };
	motion.angle = 0.f;
	motion.goal_velocity = { 0.f, 0.f };
	motion.scale = { 50, 50 };
	motion.gravity_affected = false;
	motion.depth = DEPTH::UI;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TURN_INDICATOR,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

void updateOrderIndicator(Entity entity) {

        Motion& character_motion = registry.motions.get(entity);
        Entity& indicator = registry.orderIndicators.entities[0];
        Motion& indicator_motion = registry.motions.get(indicator);

        vec2 delta = {0, -(character_motion.scale.y)};
        indicator_motion.position = character_motion.position + delta;
}

void removeOrderIndicator(){
	Entity entity = registry.orderIndicators.entities[0];
	registry.motions.remove(entity);
	registry.renderRequests.remove(entity);
    registry.orderIndicators.remove(entity);
}

void resetEnergyBar() 
{
	// As all characters share one energy bar, there should always be only 1 entity inside energyBars
	Motion& motion = registry.motions.get(registry.energyBars.entities[0]);
	vec2 pos = vec2(694, 672); // subject to change when adjusting UI positions
	motion.position = { pos };
	motion.prev_position = { pos };
	motion.goal_velocity = { 0.f, 0.f };
	motion.scale = { 400, 20 };

	Overlay& overlay = registry.overlays.get(registry.energyBars.entities[0]);
	overlay.position = overlay.original_position;
}


void updateEnergyBar(Energy energy)
{
	Motion& motion = registry.motions.get(registry.energyBars.entities[0]);
	motion.scale.x = 400 * (energy.cur_energy / energy.max_energy);
    float dx = (400 - motion.scale.x) / 2;
    Overlay& overlay = registry.overlays.get(registry.energyBars.entities[0]);
    overlay.position.x = overlay.original_position.x - dx;
}

void removeEnergyBar()
{
	Entity entity = registry.energyBars.entities[0];
	registry.motions.remove(entity);
	registry.renderRequests.remove(entity);
	registry.overlays.remove(entity);
	registry.colors.remove(entity);
	registry.energyBars.remove(entity);
}

void resetEnergy(Entity entity)
{
	Energy& energy = registry.energies.get(entity);
	energy.cur_energy = energy.max_energy;
}

Entity createHealthBar(vec2 pos, vec2 size)
{
	auto entity = Entity();

	registry.healthBars.emplace(entity);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = { pos.x, pos.y - size.y/2 - 10 };
	motion.prev_position = { pos.x, pos.y - size.y / 2 - 10 };
	motion.angle = 0.f;
	motion.goal_velocity = { 0.f, 0.f };
	motion.scale = { size.x*0.8, 10 };
    motion.original_scale = motion.scale;
	motion.gravity_affected = false;
	motion.depth = DEPTH::CHARACTER;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BAR,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	registry.colors.emplace(entity, vec3(0.f, 1.f, 0.f));

	return entity;
}

void updateHealthBar(Entity entity) {
	if (registry.playables.has(entity)) {
		Playable& playable = registry.playables.get(entity);
		Entity healthBar = playable.healthBar;
		Motion& healthBar_motion = registry.motions.get(healthBar);
		Motion& entity_motion = registry.motions.get(entity);
		healthBar_motion.position.x = entity_motion.position.x;
		healthBar_motion.position.y = entity_motion.position.y - 60;
	}
	if (registry.enemies.has(entity)) {
		Enemy& enemy = registry.enemies.get(entity);
		Entity healthBar = enemy.healthBar;
		Motion& healthBar_motion = registry.motions.get(healthBar);
		Motion& entity_motion = registry.motions.get(entity);
		healthBar_motion.position.x = entity_motion.position.x;
		healthBar_motion.position.y = entity_motion.position.y - 60;
	}
}

void removeHealthBar(Entity healthBar) {
	registry.motions.remove(healthBar);
	registry.renderRequests.remove(healthBar);
	registry.colors.remove(healthBar);
	registry.healthBars.remove(healthBar);
}


Entity createEnemy(vec2 pos, vec2 size, float starting_health, float starting_energy, 
        AttackArsenal attack_arsenal, bool slippery, bool damage_over_turn, bool heal_over_turn)
{
	auto entity = Entity();

	// TODO: tmp use red egg to represent enemy
	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.prev_position = pos;
	motion.angle = 0.f;
	motion.goal_velocity = { 0.f, 0.f };
	motion.scale = size;
	motion.gravity_affected = true;
	motion.depth = DEPTH::CHARACTER;
	motion.location = LOCATION::NORMAL;
	motion.slippery = slippery;

	Entity healthBar = createHealthBar(pos, size);
	Enemy enemy{ healthBar };
	registry.enemies.insert(entity, enemy);

	// stats
	Health& health = registry.healths.emplace(entity);
	health.max_health = 100;
	health.cur_health = starting_health;
	health.damage_per_turn = damage_over_turn;
	health.heal_per_turn = heal_over_turn;

	Energy energy{ 150, starting_energy, starting_energy};
	Initiative initiative{ 80 };

	registry.energies.insert(entity, energy);
	registry.initiatives.insert(entity, initiative);
	registry.AIs.emplace(entity);
	
	registry.attackArsenals.insert(entity, attack_arsenal);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMY,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createEnemyHealer(vec2 pos, vec2 size, float starting_health, float starting_energy,
	AttackArsenal attack_arsenal, bool slippery, bool damage_over_turn, bool heal_over_turn, BuffArsenal buff_arsenal)
{
	auto entity = Entity();

	// TODO: tmp use red egg to represent enemy
	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.prev_position = pos;
	motion.angle = 0.f;
	motion.goal_velocity = { 0.f, 0.f };
	motion.scale = vec2(48, size.y);
	motion.gravity_affected = true;
	motion.depth = DEPTH::CHARACTER;
	motion.location = LOCATION::NORMAL;
	motion.slippery = slippery;

	Entity healthBar = createHealthBar(pos, size);
	Enemy enemy{ healthBar };
	registry.enemies.insert(entity, enemy);

	// stats
	Health health{ 50, starting_health, damage_over_turn, false, heal_over_turn};
	Energy energy{ 100, starting_energy, starting_energy };
	Initiative initiative{ 90 };

	registry.healths.insert(entity, health);
	registry.energies.insert(entity, energy);
	registry.initiatives.insert(entity, initiative);
	registry.AIs.emplace(entity);

	registry.attackArsenals.insert(entity, attack_arsenal);
	registry.buffArsenals.insert(entity, buff_arsenal);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMY_ICE_CREAM,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	// registry.colors.emplace(entity, vec3(0.2f, 0.4f, 0.2f));

	return entity;
}

void removeEnemy(Entity entity)
{
	Enemy& enemy = registry.enemies.get(entity);
	removeHealthBar(enemy.healthBar);

	registry.motions.remove(entity);
	registry.enemies.remove(entity);
	registry.healths.remove(entity);
	registry.energies.remove(entity);
	registry.initiatives.remove(entity);
	registry.renderRequests.remove(entity);
	registry.attackArsenals.remove(entity);
	registry.AIs.remove(entity);
    registry.attackArsenals.remove(entity);
	registry.buffArsenals.remove(entity);
    registry.collisions.remove(entity);
	registry.colors.remove(entity);
}

Entity createPlayer(vec2 pos, vec2 size, float starting_health, float starting_energy,
        AttackArsenal attack_arsenal, bool slippery, bool damage_over_turn, BuffArsenal buff_arsenal)

{
	auto entity = Entity();

	// TODO: tmp use gree egg to represent player
	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.prev_position = pos;
	motion.angle = 0.f;
	motion.goal_velocity = { 0.f, 0.f };
	motion.scale = size;
	motion.gravity_affected = true;
	motion.depth = DEPTH::CHARACTER;
	motion.slippery = slippery;

	Entity healthBar = createHealthBar(pos, size);
	Playable player{ healthBar };
	registry.playables.insert(entity, player);

	// stats
	Health health{ 100, starting_health, damage_over_turn };
	Energy energy{ 150, starting_energy, starting_energy };
	/* Energy energy{ 500, 500, 500 }; */
	Initiative initiative{ 50 };

	registry.healths.insert(entity, health);
	registry.energies.insert(entity, energy);
	registry.initiatives.insert(entity, initiative);

	registry.attackArsenals.insert(entity, attack_arsenal);
    registry.buffArsenals.insert(entity, buff_arsenal);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::PLAYER,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

void removePlayer(Entity entity)
{
	Playable& playable = registry.playables.get(entity);
	removeHealthBar(playable.healthBar);

	registry.motions.remove(entity);
	registry.playables.remove(entity);
	registry.healths.remove(entity);
	registry.energies.remove(entity);
	registry.initiatives.remove(entity);
	registry.renderRequests.remove(entity);
	registry.attackArsenals.remove(entity);
    registry.collisions.remove(entity);
}

Entity createTerrain(vec2 pos, vec2 size, bool breakable, int level)
{
	auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.prev_position = pos;
	motion.angle = 0.f;
	motion.goal_velocity = { 0.f, 0.f };
	motion.scale = size;
	motion.depth = DEPTH::TERRAIN;

	// TODO: terrains might have more components
	Terrain terrain{ breakable };
	registry.terrains.insert(entity, terrain);
	
	// Break when the terrain is breakable and health < 0
	Health health{ 20, 20 };
	registry.healths.insert(entity, health);

	if (!breakable) {
		switch (level)
		{
		case 0:
		case 1:
			registry.renderRequests.insert(
				entity,
				{ TEXTURE_ASSET_ID::TERRAIN1,
					EFFECT_ASSET_ID::TEXTURED,
					GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case 2:
			registry.renderRequests.insert(
				entity,
				{ TEXTURE_ASSET_ID::TERRAIN2,
					EFFECT_ASSET_ID::TEXTURED,
					GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case 3:
			registry.renderRequests.insert(
				entity,
				{ TEXTURE_ASSET_ID::TERRAIN3,
					EFFECT_ASSET_ID::TEXTURED,
					GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case 4:
			registry.renderRequests.insert(
				entity,
				{ TEXTURE_ASSET_ID::TERRAIN4,
					EFFECT_ASSET_ID::TEXTURED,
					GEOMETRY_BUFFER_ID::SPRITE });
			break;
		default:
			break;
		}
	}
	else {
		switch (level)
		{
		case 0:
		case 1:
		case 2:
			registry.renderRequests.insert(
				entity,
				{ TEXTURE_ASSET_ID::TERRAIN2_BREAKABLE,
					EFFECT_ASSET_ID::TEXTURED,
					GEOMETRY_BUFFER_ID::SPRITE });
			break;
		case 3:
			registry.renderRequests.insert(
				entity,
				{ TEXTURE_ASSET_ID::TERRAIN3_BREAKABLE,
					EFFECT_ASSET_ID::TEXTURED,
					GEOMETRY_BUFFER_ID::SPRITE });
			break;
		default:
			break;
		}
	}

	

	return entity;
}

void removeTerrain(Entity entity)
{
	registry.motions.remove(entity);
	registry.terrains.remove(entity);
	registry.healths.remove(entity);
	registry.renderRequests.remove(entity);
    registry.collisions.remove(entity);
}

Entity createAttackObject(Entity attacker, AttackAbility ability, float angle, vec2 pos) {
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.prev_position = pos;
	motion.angle = angle;
	motion.goal_velocity = vec2(ability.speed * (float)cos(angle), ability.speed * (float)sin(angle));
	motion.scale = ability.size;
	motion.depth = DEPTH::ATTACK;
	motion.gravity_affected = ability.gravity_affected;

	// ttl can be approximated by range/speed (gravity can increase speed)
	// melee attack has a constant ttl
	float ttl = ability.range == 0.f ? MELEE_ATTACK_TTL : ability.range / ability.speed * 1000;
	AttackObject obj{ ttl, ability.damage, attacker };
	registry.attackObjects.insert(entity, obj);

	registry.renderRequests.insert(
		entity,
		{ (TEXTURE_ASSET_ID)ability.texture_ID, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}

void removeAttackObject(Entity entity)
{
	registry.motions.remove(entity);
	registry.attackObjects.remove(entity);
	registry.renderRequests.remove(entity);
	registry.colors.remove(entity); // TODO: remove this line when we have a proper sprite
	registry.projectiles.remove(entity);
    registry.collisions.remove(entity);
}

Entity createCamera(vec2 pos, vec2 offset, vec2 lower_limit, vec2 higher_limit)
{
	auto entity = Entity();
	
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.prev_position = pos;
	motion.angle = 0.f;
	motion.goal_velocity = {0.f, 0.f};
	motion.scale = { window_width_px / 3, window_height_px / 3 };
	motion.depth = DEPTH::CAMERA;

	Camera camera{ offset, lower_limit, higher_limit };
	registry.cameras.insert(entity, camera);

	return entity;
}

void removeCamera(Entity entity)
{
	registry.motions.remove(entity);
	registry.cameras.remove(entity);
}

Entity createBlinkTimer(float ms) {
    auto entity = Entity();

    BlinkTimer timer { ms };
    registry.blinkTimers.insert(entity, timer);
    
    return entity;
}

void removeBlinkTimer(Entity entity) {
    registry.blinkTimers.remove(entity);
}

Entity createPromptWithTimer(float ms, TEXTURE_ASSET_ID texture_ID) {

    // delete previous prompts
    for (uint i = 0; i < registry.promptsWithTimer.size(); i++) {
        Entity& entity = registry.promptsWithTimer.entities[i];
        removePromptWithTimer(entity);
    }

    auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.goal_velocity = { 0, 0 };
	motion.position = { 640, 360 };
	motion.prev_position = motion.position;
	motion.scale = { 1280, 720 };
    motion.depth = DEPTH::PROMPT;

	Overlay overlay{ motion.position };
	registry.overlays.insert(entity, overlay);

    PromptWithTimer timer { ms };
    registry.promptsWithTimer.insert(entity, timer);


	registry.renderRequests.insert(
		entity,
		{ texture_ID,
			EFFECT_ASSET_ID::TEXTURED, // TODO COOLDOWN effect
			GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

void removePromptWithTimer(Entity entity) {
    registry.motions.remove(entity);
    registry.overlays.remove(entity);
    registry.promptsWithTimer.remove(entity);
    registry.renderRequests.remove(entity);
}

Entity createButton(vec2 pos, vec2 size, bool (*on_click)(), TEXTURE_ASSET_ID texture_ID, bool disabled)
{
	auto entity = createGenericButton(pos, size, on_click, disabled);

	registry.renderRequests.insert(
		entity,
		{ texture_ID,
			EFFECT_ASSET_ID::TEXTURED, // TODO COOLDOWN effect
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

void removeButton(Entity entity)
{
	registry.motions.remove(entity);
	registry.clickables.remove(entity);
	registry.overlays.remove(entity);
	registry.renderRequests.remove(entity);
}

Entity createPlayerButton(vec2 pos, vec2 size, bool (*on_click)(), TEXTURE_ASSET_ID texture_ID, bool disabled)
{
	auto entity = createButton(pos, size, on_click, texture_ID, disabled);

    registry.playerButtons.emplace(entity);

	return entity;
}

void removePlayerButton(Entity entity)
{
    registry.playerButtons.remove(entity);
    removeButton(entity);
}

Entity createAbilityButton(vec2 pos, vec2 size, bool (*on_click)(), TEXTURE_ASSET_ID texture_ID, bool disabled)
{
    auto entity = createPlayerButton(pos, size, on_click, texture_ID, disabled);

    registry.abilityButtons.emplace(entity);

    return entity;
}

void removeAbilityButton(Entity entity)
{
	registry.abilityButtons.remove(entity);
	removePlayerButton(entity);
}

void createAbilityTooltip(vec2 pos, int ability)
{
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.prev_position = pos;
	motion.angle = 0.f;
	motion.goal_velocity = { 0.f, 0.f };
	motion.scale = { 300.f, 300.f };
	motion.depth = DEPTH::UI_TOP;

	registry.abilityTooltip.emplace(entity);
	/*Overlay overlay{ pos };
	registry.overlays.insert(entity, overlay);*/

	if (ability == 0) {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::MELEE_ATTACK_TOOLTIP,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
	}
	else if (ability == 1) {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::ADVANCED_ATTACK_TOOLTIP,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
	}
	else {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::HEALTH_POTION_TOOLTIP,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
	}
}

void removeAbilityTooltip()
{
	if (registry.abilityTooltip.size() > 0) {
		Entity entity = registry.abilityTooltip.entities[0];
		registry.motions.remove(entity);
		registry.abilityTooltip.remove(entity);
		registry.renderRequests.remove(entity);
		registry.overlays.remove(entity);
	}
}

Entity createHitEffect(Entity entity, float ttl_ms)
{
	HitEffect effect{ ttl_ms };
	registry.hitEffects.insert(entity, effect);
	return entity;
}

void removeHitEffect(Entity entity)
{
	registry.hitEffects.remove(entity);
}

Entity createBackground(vec2 size, int level)
{
	auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = { window_width_px / 2, window_height_px / 2 };
	motion.angle = 0.f;
	motion.goal_velocity = { 0.f, 0.f };
	motion.scale = size;
	motion.depth = DEPTH::BACKGROUND;

	Background background{ };

	// Level number would determine which texture would be used
	switch (level)
	{
	case 0:
	case 1:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::BACKGROUND1,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		background.proportion_velocity = 0.5;
		break;
	case 2:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::BACKGROUND2,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		background.proportion_velocity = 0.5;
		break;
	case 3:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::BACKGROUND3,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		background.proportion_velocity = 0.5;
		break;
	case 4:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::BACKGROUND4,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		background.proportion_velocity = 0.5;
		break;
	case 11:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::BACKGROUND11,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		background.proportion_velocity = 0.9;
		break;
	case 12:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::BACKGROUND12,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		background.proportion_velocity = 0.7;
		break;
	case 21:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::BACKGROUND21,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		background.proportion_velocity = 0.9;
		break;
	case 22:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::BACKGROUND22,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		background.proportion_velocity = 0.7;
		break;
	case 31:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::BACKGROUND31,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		background.proportion_velocity = 0.9;
		break;
	case 32:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::BACKGROUND32,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		background.proportion_velocity = 0.7;
		break;
	case 41:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::BACKGROUND41,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		background.proportion_velocity = 0.9;
		break;
	case 42:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::BACKGROUND42,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		background.proportion_velocity = 0.7;
		break;
	default:
		break;
	}

	registry.backgrounds.insert(entity, background);

	return entity;
}

void removeBackground(Entity entity)
{
    registry.motions.remove(entity);
	registry.backgrounds.remove(entity);
    registry.renderRequests.remove(entity);
}

Entity createLadder(vec2 pos, vec2 size)
{
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.prev_position = pos;
	motion.angle = 0.f;
	motion.goal_velocity = { 0.f, 0.f };
	motion.scale = size;
	motion.depth = DEPTH::LADDER;


	registry.climbables.emplace(entity);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::LADDER,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

void removeLadder(Entity entity)
{
	registry.motions.remove(entity);
	registry.climbables.remove(entity);
	registry.renderRequests.remove(entity);
}

Entity createCooldown(vec2 pos, int cool_down_left) {
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.prev_position = pos;
	motion.angle = 0.f;
	motion.goal_velocity = { 0.f, 0.f };
	motion.scale = vec2(50, 50);
	motion.depth = DEPTH::COOLDOWN;

	CoolDown coolDown{ };
	registry.cooldowns.insert(entity, coolDown);
	Overlay overlay{pos};
	registry.overlays.insert(entity, overlay);

	switch (cool_down_left)
	{
	case 1:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::COOLDOWN1,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 2:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::COOLDOWN2,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 3:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::COOLDOWN3,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
	}
	return entity;
}

void removeCooldown(Entity entity)
{
	registry.motions.remove(entity);
	registry.cooldowns.remove(entity);
	registry.overlays.remove(entity);
	registry.renderRequests.remove(entity);
}

Entity createPrompt(vec2 pos, vec2 size, int step) {
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.prev_position = pos;
	motion.angle = 0.f;
	motion.goal_velocity = { 0.f, 0.f };
	motion.scale = size;
	motion.depth = DEPTH::PROMPT;

	Overlay overlay{ pos };
	registry.overlays.insert(entity, overlay);

	switch (step)
	{
	case -100:// tutorial failed
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::TUTORIAL_FAIL,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 0:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::TUTORIAL_MOVE,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 1:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::TUTORIAL_ATTACK_BASIC,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 2:
	case 3: 
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::TUTORIAL_ATTACK_ADVANCED,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 4:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::TUTORIAL_HEAL,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 5:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::TUTORIAL_DEFEAT,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 6:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::TUTORIAL_END,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 10:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::LEVEL_1_START,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 11:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::LEVEL_1_WON,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 12:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::LEVEL_1_LOST,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 20:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::LEVEL_2_START,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 21:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::LEVEL_2_WON,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 22:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::LEVEL_2_LOST,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 30: 
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::LEVEL_3_START,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 31:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::LEVEL_3_WON,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 32:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::LEVEL_3_LOST,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 40:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::LEVEL_4_START,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 41:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::LEVEL_4_WON,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 42:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::LEVEL_4_LOST,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	default:
		break;
	}

	return entity;
}

void removePrompt(Entity entity)
{
	registry.motions.remove(entity);
	registry.overlays.remove(entity);
	registry.renderRequests.remove(entity);
}

Entity createStorySlide(vec2 pos, vec2 size, int slide) {
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.prev_position = pos;
	motion.angle = 0.f;
	motion.goal_velocity = { 0.f, 0.f };
	motion.scale = size;
	motion.depth = DEPTH::BACKGROUND;

	Overlay overlay{ pos };
	registry.overlays.insert(entity, overlay);

	switch (slide)
	{
	case 0:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::STORY1,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 1:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::STORY2,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 2:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::STORY3,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 3:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::STORY4,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 4:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::STORY5,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 10:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::STORY11,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 11:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::STORY12,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 12:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::STORY13,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 13:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::STORY14,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 14:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::STORY15,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 15:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::STORY16,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 16:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::STORY17,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 17:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::STORY18,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	default:
		break;
	}

	return entity;
}

void removeStorySlide(Entity entity)
{
	registry.motions.remove(entity);
	registry.overlays.remove(entity);
	registry.renderRequests.remove(entity);
}

Entity createUI(vec2 pos, vec2 size) {
	auto entity = Entity();
	registry.UIs.emplace(entity);

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.prev_position = pos;
	motion.angle = 0.f;
	motion.goal_velocity = { 0.f, 0.f };
	motion.scale = size;
	motion.depth = DEPTH::UI;

	Overlay overlay{ pos };
	registry.overlays.insert(entity, overlay);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::UI_LAYOUT,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });
		

	return entity;
}

void removeUI(Entity entity)
{
	registry.UIs.remove(entity);
	registry.motions.remove(entity);
	registry.overlays.remove(entity);
	registry.renderRequests.remove(entity);
}
