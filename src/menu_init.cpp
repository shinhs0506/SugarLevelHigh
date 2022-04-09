#include "menu_init.hpp"
#include "tiny_ecs.hpp"
#include "tiny_ecs_registry.hpp"
#include "game_init.hpp"

Entity createStartButton(vec2 pos, vec2 size, bool (*on_click)())
{
    Entity entity = createGenericButton(pos, size, on_click);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::START_BUTTON,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createHelpButton(vec2 pos, vec2 size, bool (*on_click)()) 
{
    Entity entity = createGenericButton(pos, size, on_click);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::HELP_BUTTON,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createExitButton(vec2 pos, vec2 size, bool (*on_click)())
{
    Entity entity = createGenericButton(pos, size, on_click);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::EXIT_BUTTON,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createTutorialButton(vec2 pos, vec2 size, bool (*on_click)())
{
	Entity entity = createGenericButton(pos, size, on_click);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TUTORIAL_BUTTON,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createLevel1Button(vec2 pos, vec2 size, bool (*on_click)(), bool disabled)
{
	Entity entity = createGenericButton(pos, size, on_click, disabled);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::LEVEL_1_BUTTON,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createLevel2Button(vec2 pos, vec2 size, bool (*on_click)(), bool disabled)
{
	Entity entity = createGenericButton(pos, size, on_click, disabled);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::LEVEL_2_BUTTON,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createLevel3Button(vec2 pos, vec2 size, bool (*on_click)(), bool disabled)
{
	Entity entity = createGenericButton(pos, size, on_click, disabled);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::LEVEL_3_BUTTON,
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
	motion.goal_velocity = { 0.f, 0.f };
	motion.scale = size;


	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::HELP_IMAGE,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createConfigButton(vec2 pos, vec2 size, bool (*on_click)(), TEXTURE_ASSET_ID texture_ID)
{
	Entity entity = createGenericButton(pos, size, on_click);

	registry.renderRequests.insert(
		entity,
		{ texture_ID, 
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}