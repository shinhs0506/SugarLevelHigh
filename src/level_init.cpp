#include "level_init.hpp"
#include "tiny_ecs_registry.hpp"

Entity createDebugLine(vec2 position, vec2 scale)
{
	Entity entity = Entity();

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
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

Entity createEnemy(vec2 pos, vec2 size)
{
	auto entity = Entity();

	// TODO: tmp use red egg to represent enemy
	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = size;

	registry.enemies.emplace(entity);

	// stats
	Health health{ 100, 100 };
	Energy energy{ 100, 100 };
	Initiative initiative{ 80 };

	registry.healths.insert(entity, health);
	registry.energies.insert(entity, energy);
	registry.initiatives.insert(entity, initiative);
	
	// TODO: insert attack abilities

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::COLOURED,
			GEOMETRY_BUFFER_ID::EGG });

	registry.colors.emplace(entity, vec3(1.f, 0.f, 0.f));

	return entity;
}

Entity createPlayer(vec2 pos, vec2 size)
{
	auto entity = Entity();

	// TODO: tmp use gree egg to represent player
	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = size;

	registry.playables.emplace(entity);

	// stats
	Health health{ 100, 100 };
	Energy energy{ 100, 100 };
	Initiative initiative{ 50 };

	registry.healths.insert(entity, health);
	registry.energies.insert(entity, energy);
	registry.initiatives.insert(entity, initiative);

	// TODO: insert attack abilities

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::COLOURED,
			GEOMETRY_BUFFER_ID::EGG });

	registry.colors.emplace(entity, vec3(0.f, 1.f, 0.f));

	return entity;
}

Entity createTerrain(vec2 pos, vec2 size)
{
	auto entity = Entity();

	// TODO: tmp use yellow square to represent terrain
	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = size;

	// TODO: terrains might have more components
	Terrain terrain{ false }; 
	registry.terrains.insert(entity, terrain);
	
	// Break when the terrain is breakable and health < 0
	Health health{ 20, 20 };
	registry.healths.insert(entity, health);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::COLOURED,
			GEOMETRY_BUFFER_ID::SQUARE });

	registry.colors.emplace(entity, vec3(1.f, 1.f, 0.f));

	return entity;
}