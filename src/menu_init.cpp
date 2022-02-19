#include "menu_init.hpp"
#include "tiny_ecs.hpp"
#include "tiny_ecs_registry.hpp"

Entity createMenuButton(vec2 pos, vec2 size) {
    Entity entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.prev_position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = size;

	Overlay overlay{pos};
	registry.overlays.insert(entity, overlay);

    return entity;
}

Entity createStartButton(vec2 pos, vec2 size)
{
    Entity entity = createMenuButton(pos, size);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::START_BUTTON,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createHelpButton(vec2 pos, vec2 size) 
{
    Entity entity = createMenuButton(pos, size);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::HELP_BUTTON,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createExitButton(vec2 pos, vec2 size)
{
    Entity entity = createMenuButton(pos, size);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::EXIT_BUTTON,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createBackButton(vec2 pos, vec2 size)
{
    Entity entity = createMenuButton(pos, size);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BACK_BUTTON,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createHelpImage(vec2 pos, vec2 size)
{
    Entity entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.prev_position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = size;


	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::HELP_IMAGE,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}
