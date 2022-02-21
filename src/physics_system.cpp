// internal
#include "physics_system.hpp"
#include "level_init.hpp"

// Returns the local bounding coordinates scaled by the current size of the entity
vec2 get_bounding_box(const Motion& motion)
{
	// abs is to avoid negative scale due to the facing direction.
	return { abs(motion.scale.x), abs(motion.scale.y) };
}

// use AABB detection
bool collides(const Motion& motion1, const Motion& motion2)
{
	const vec2 bb1 = get_bounding_box(motion1) / 2.f;
	const vec2 bb2 = get_bounding_box(motion2) / 2.f;
	const vec2 pos1 = motion1.position;
	const vec2 pos2 = motion2.position;
	
	if (// x axis collision check
		(pos1[0] - bb1[0] <= pos2[0] + bb2[0] && pos1[0] + bb1[0] >= pos2[0] - bb2[0]) &&
		// y axis collision check
		(pos1[1] - bb1[1] <= pos2[1] + bb2[1] && pos1[1] + bb1[1] >= pos2[1] - bb2[1])) 
	{
		return true;
	}

	return false;
}

void PhysicsSystem::step(float elapsed_ms)
{
	// Move entities with motion component with respect to their velocity
	auto& motion_registry = registry.motions;
	for (uint i = 0; i < motion_registry.size(); i++)
	{
		Motion& motion = motion_registry.components[i];
		Entity entity = motion_registry.entities[i];

		// Gravity
		if (motion.gravity_affected == true) {
			//When collision with terrain is detected. Reset this velocity to 0
			motion.velocity.y += gravity * (elapsed_ms/1000.0f);
		}

		motion.prev_position = motion.position;
		motion.position = motion.position + elapsed_ms / 1000.f * motion.velocity;

		// Adapt angle for projectile motion
		if (registry.projectiles.has(entity)) {
			// Change rotation
			motion.angle = atan2(motion.velocity.y, motion.velocity.x);
		}

		if (registry.cameras.has(entity))
		{
			Camera& camera = registry.cameras.get(entity);
			motion.position = clamp(motion.position, camera.lower_limit, camera.higer_limit);
		}
	}

	// Update overlays relative to main camera
	auto& overlays_registry = registry.overlays;
	Entity camera = registry.cameras.entities[0];
	Motion& camera_motion = motion_registry.get(camera);
	Camera& camera_component = registry.cameras.get(camera);

	for (uint i = 0; i < overlays_registry.size(); i++) {
		Overlay& overlay = overlays_registry.components[i];
		Entity entity = overlays_registry.entities[i];
		Motion& motion = motion_registry.get(entity);
		motion.position = camera_motion.position - camera_component.offset + overlay.position;

	}

	// Check for collisions between all moving entities
    ComponentContainer<Motion> &motion_container = registry.motions;
	for(uint i = 0; i<motion_container.components.size(); i++)
	{
		Motion& motion_i = motion_container.components[i];
		Entity entity_i = motion_container.entities[i];
		
		// note starting j at i+1 to compare all (i,j) pairs only once (and to not compare with itself)
		for(uint j = i+1; j<motion_container.components.size(); j++)
		{
			Motion& motion_j = motion_container.components[j];
			if (collides(motion_i, motion_j))
			{
				Entity entity_j = motion_container.entities[j];
				// Create a collisions event
				// We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
				registry.collisions.emplace_with_duplicates(entity_i, entity_j);
				registry.collisions.emplace_with_duplicates(entity_j, entity_i);
			}
		}
	}

	// debugging of bounding boxes
	if (debugging.in_debug_mode)
	{
		uint size_before_adding_new = (uint)motion_container.components.size();
		for (uint i = 0; i < size_before_adding_new; i++)
		{
			Motion& motion_i = motion_container.components[i];
			Entity entity_i = motion_container.entities[i];

			// don't draw debugging visuals around debug lines
			if (registry.debugComponents.has(entity_i))
				continue;

			// visualize the radius with two axis-aligned lines
			const vec2 bonding_box = get_bounding_box(motion_i);
			float radius = sqrt(dot(bonding_box/2.f, bonding_box/2.f));
			vec2 line_scale1 = { motion_i.scale.x / 10, 2*radius };
			vec2 line_scale2 = { 2*radius, motion_i.scale.x / 10};
			vec2 position = motion_i.position;
			Entity line1 = createDebugLine(motion_i.position, line_scale1);
			Entity line2 = createDebugLine(motion_i.position, line_scale2);

		}
	}
}