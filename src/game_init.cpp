#include "game_init.hpp"
#include "components.hpp"
#include "tiny_ecs_registry.hpp"

Entity createGenericButton(vec2 pos, vec2 size, void (*on_click)()){
    Entity entity = Entity();

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.prev_position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = size;

	Clickable clickable{ on_click };
	registry.clickables.insert(entity, clickable);

	Overlay overlay{pos};
	registry.overlays.insert(entity, overlay);

    return entity;
}
