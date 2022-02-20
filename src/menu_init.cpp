#include "menu_init.hpp"
#include "tiny_ecs_registry.hpp"
#include "game_init.hpp"

Entity createStartButton(vec2 pos, vec2 size, void (*on_click)())
{
    Entity entity = createGenericButton(pos, size, on_click);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::START_BUTTON,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createExitButton(vec2 pos, vec2 size, void (*on_click)())
{
    Entity entity = createGenericButton(pos, size, on_click);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::EXIT_BUTTON,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}
