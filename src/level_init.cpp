#include "level_init.hpp"
#include "tiny_ecs_registry.hpp"
#include "game_init.hpp"
#include "ability.hpp"

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
	motion.goal_velocity = { 0.f, 0.f };
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

Entity createOrderIndicator(){
	auto entity = Entity();

	registry.orderIndicators.emplace(entity);
	vec2 pos = vec2(700, 600); // subject to change when adjusting UI positions
	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = { pos };
	motion.prev_position = { pos };
	motion.angle = 0.f;
	motion.goal_velocity = { 0.f, 0.f };
	motion.scale = { 20, 20 };
	motion.gravity_affected = false;
	motion.depth = DEPTH::UI;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
			EFFECT_ASSET_ID::COLOURED,
			GEOMETRY_BUFFER_ID::SQUARE });

	registry.colors.emplace(entity, vec3(0.f, 1.f, 0.f));

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
	registry.colors.remove(entity);
    registry.orderIndicators.remove(entity);
}

void resetEnergyBar() 
{
	// As all characters share one energy bar, there should always be only 1 entity inside energyBars
	Motion& motion = registry.motions.get(registry.energyBars.entities[0]);
	vec2 pos = vec2(700, 600); // subject to change when adjusting UI positions
	motion.position = { pos };
	motion.prev_position = { pos };
	motion.goal_velocity = { 0.f, 0.f };
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
	motion.gravity_affected = false;
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
        AttackArsenal attack_arsenal)
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

	Entity healthBar = createHealthBar(pos, size);
	Enemy enemy{ healthBar };
	registry.enemies.insert(entity, enemy);

	// stats
	Health health{ 100, starting_health };
	Energy energy{ 150, starting_energy, starting_energy};
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
	registry.attackArsenals.remove(entity);
	registry.AIs.remove(entity);
    registry.attackArsenals.remove(entity);
    registry.collisions.remove(entity);
}

Entity createPlayer(vec2 pos, vec2 size, float starting_health, float starting_energy,
        AttackArsenal attack_arsenal)
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

	Entity healthBar = createHealthBar(pos, size);
	Playable player{ healthBar };
	registry.playables.insert(entity, player);

	// stats
	Health health{ 100, starting_health };
	Energy energy{ 150, starting_energy, starting_energy };
	/* Energy energy{ 500, 500, 500 }; */
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
    registry.collisions.remove(entity);
}

Entity createTerrain(vec2 pos, vec2 size, bool breakable)
{
	auto entity = Entity();

	// TODO: tmp use yellow square to represent terrain
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
	registry.renderRequests.remove(entity);
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
	registry.backgrounds.insert(entity, background);

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
		break;
	default:
		break;
	}

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
	motion.depth = DEPTH::UI;

	switch (step)
	{
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
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::TUTORIAL_ATTACK_ADVANCED,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 3:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::TUTORIAL_COOLDOWN,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		break;
	case 4:
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::TUTORIAL_END,
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
	registry.renderRequests.remove(entity);
}