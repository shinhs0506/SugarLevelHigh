#include "game_init.hpp"
#include "components.hpp"
#include "tiny_ecs_registry.hpp"

Entity createGenericButton(vec2 pos, vec2 size, bool (*on_click)(), bool disabled){
    Entity entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.prev_position = pos;
	motion.angle = 0.f;
	motion.goal_velocity = { 0.f, 0.f };
	motion.scale = size;

	Clickable clickable{ on_click, disabled };
	registry.clickables.insert(entity, clickable);

	Overlay overlay{pos};
	registry.overlays.insert(entity, overlay);

    return entity;
}

Entity createBackButton(vec2 pos, vec2 size, bool (*on_click)())
{
    Entity entity = createGenericButton(pos, size, on_click);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BACK_BUTTON,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createNextButton(vec2 pos, vec2 size, bool (*on_click)())
{
	Entity entity = createGenericButton(pos, size, on_click);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::NEXT_BUTTON,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

