#include "level_init.hpp"
#include "tiny_ecs_registry.hpp"
#include "game_init.hpp"

Entity createDebugLine(vec2 position, vec2 scale)
{
	Entity entity = Entity();

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
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

Entity createEnergyBar()
{
	auto entity = Entity();

	registry.energyBars.emplace(entity);
	vec2 pos = vec2(700, 600); // subject to change when adjusting UI positions

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = { pos };
	motion.prev_position = { pos };
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 300, 20 };
	motion.gravity_affected = false;
	motion.depth = DEPTH::UI;

	Overlay overlay{ pos };
	registry.overlays.insert(entity, overlay);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::COLOURED,
			GEOMETRY_BUFFER_ID::SQUARE });

	registry.colors.emplace(entity, vec3(1.f, 0.f, 0.f));

	return entity;
}

void resetEnergyBar() 
{
	// As all characters share one energy bar, there should always be only 1 entity inside energyBars
	Motion& motion = registry.motions.get(registry.energyBars.entities[0]);
	vec2 pos = vec2(700, 600); // subject to change when adjusting UI positions
	motion.position = { pos };
	motion.prev_position = { pos };
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 300, 20 };
}

void updateEnergyBar(Energy energy)
{
	Motion& motion = registry.motions.get(registry.energyBars.entities[0]);
	motion.scale.x = 300 * (energy.cur_energy / energy.max_energy);
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

Entity createHealthBar(vec2 pos, vec2 size)
{
	auto entity = Entity();

	registry.healthBars.emplace(entity);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = { pos.x, pos.y - size.y/2 - 10 };
	motion.prev_position = { pos.x, pos.y - size.y / 2 - 10 };
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { size.x*0.8, 10 };
	motion.gravity_affected = true;
	motion.depth = DEPTH::CHARACTER;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::COLOURED,
			GEOMETRY_BUFFER_ID::SQUARE });

	registry.colors.emplace(entity, vec3(0.f, 1.f, 0.f));

	return entity;
}

void updateHealthBar(Entity entity) {
	if (registry.playables.has(entity)) {
		Playable& playable = registry.playables.get(entity);
		Entity healthBar = playable.healthBar;
		Motion& healthBar_motion = registry.motions.get(healthBar);
		healthBar_motion.velocity = registry.motions.get(entity).velocity;
	}
	if (registry.enemies.has(entity)) {
		Enemy& enemy = registry.enemies.get(entity);
		Entity healthBar = enemy.healthBar;
		Motion& healthBar_motion = registry.motions.get(healthBar);
		healthBar_motion.velocity = registry.motions.get(entity).velocity;
	}
}

void removeHealthBar(Entity healthBar) {
	registry.motions.remove(healthBar);
	registry.renderRequests.remove(healthBar);
	registry.colors.remove(healthBar);
	registry.healthBars.remove(healthBar);
}


Entity createEnemy(vec2 pos, vec2 size, AttackArsenal attack_arsenal)
{
	auto entity = Entity();

	// TODO: tmp use red egg to represent enemy
	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.prev_position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = size;
	motion.gravity_affected = true;
	motion.depth = DEPTH::CHARACTER;

	Entity healthBar = createHealthBar(pos, size);
	Enemy enemy{ healthBar };
	registry.enemies.insert(entity, enemy);

	// stats
	Health health{ 100, 100 };
	Energy energy{ 500, 500 };
	Initiative initiative{ 80 };

	registry.healths.insert(entity, health);
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
	registry.AIs.remove(entity);
}

Entity createPlayer(vec2 pos, vec2 size, AttackArsenal attack_arsenal)
{
	auto entity = Entity();

	// TODO: tmp use gree egg to represent player
	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.prev_position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = size;
	motion.gravity_affected = true;
	motion.depth = DEPTH::CHARACTER;

	Entity healthBar = createHealthBar(pos, size);
	Playable player{ healthBar };
	registry.playables.insert(entity, player);

	// stats
	Health health{ 100, 100 };
	Energy energy{ 500, 500 };
	Initiative initiative{ 50 };

	registry.healths.insert(entity, health);
	registry.energies.insert(entity, energy);
	registry.initiatives.insert(entity, initiative);

	registry.attackArsenals.insert(entity, attack_arsenal);

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
}

Entity createTerrain(vec2 pos, vec2 size)
{
	auto entity = Entity();

	// TODO: tmp use yellow square to represent terrain
	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.prev_position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = size;
	motion.depth = DEPTH::TERRAIN;

	// TODO: terrains might have more components
	Terrain terrain{ false }; 
	registry.terrains.insert(entity, terrain);
	
	// Break when the terrain is breakable and health < 0
	Health health{ 20, 20 };
	registry.healths.insert(entity, health);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TERRAIN1,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

void removeTerrain(Entity entity)
{
	registry.motions.remove(entity);
	registry.terrains.remove(entity);
	registry.healths.remove(entity);
	registry.renderRequests.remove(entity);
}

Entity createAttackObject(Entity attacker, AttackAbility ability, float angle, vec2 pos) {
	auto entity = Entity();

	vec2 attack_object_velocity = vec2(ability.range * (float)cos(angle), ability.range * (float)sin(angle));

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.prev_position = pos;
	motion.angle = angle;
	motion.velocity = attack_object_velocity;
	motion.scale = ability.size;
	motion.depth = DEPTH::ATTACK;
	motion.gravity_affected = ability.gravity_affected;

	AttackObject obj{ ability.ttl_ms, ability.damage, attacker };
	registry.attackObjects.insert(entity, obj);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::COLOURED,
			(GEOMETRY_BUFFER_ID)ability.shape });

	registry.colors.emplace(entity, vec3(1.f, 0.f, 0.f));

	return entity;
}

void removeAttackObject(Entity entity)
{
	registry.motions.remove(entity);
	registry.attackObjects.remove(entity);
	registry.renderRequests.remove(entity);
	registry.colors.remove(entity); // TODO: remove this line when we have a proper sprite
	registry.projectiles.remove(entity);
}

Entity createCamera(vec2 pos, vec2 offset, vec2 lower_limit, vec2 higher_limit)
{
	auto entity = Entity();
	
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.prev_position = pos;
	motion.angle = 0.f;
	motion.velocity = {0.f, 0.f};
	motion.scale = {1.f, 1.f};
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

Entity createButton(vec2 pos, vec2 size, bool (*on_click)())
{
	auto entity = createGenericButton(pos, size, on_click);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::COLOURED,
			GEOMETRY_BUFFER_ID::SQUARE });

	registry.colors.emplace(entity, vec3(0.f, 0.f, 1.f));

	return entity;
}

void removeButton(Entity entity)
{
	registry.motions.remove(entity);
	registry.clickables.remove(entity);
	registry.overlays.remove(entity);
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
	motion.velocity = { 0.f, 0.f };
	motion.scale = size;
	motion.depth = DEPTH::BACKGROUND;

	Background background{ };
	registry.backgrounds.insert(entity, background);

	// Level number would determine which texture would be used
	switch (level)
	{
	case 0:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::BACKGROUND1,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	default:
		break;
	}

	return entity;
}

void removeBackground(Entity entity)
{
	registry.backgrounds.remove(entity);
}

Entity createLadder(vec2 pos, vec2 size)
{
	auto entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.prev_position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = size;
	motion.depth = DEPTH::LADDER;


	registry.climbables.emplace(entity);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::COLOURED,
			GEOMETRY_BUFFER_ID::SQUARE });

	registry.colors.emplace(entity, vec3(1.f, 1.f, 0.f));

	return entity;
}

void removeLadder(Entity entity)
{
	registry.motions.remove(entity);
	registry.climbables.remove(entity);
}
