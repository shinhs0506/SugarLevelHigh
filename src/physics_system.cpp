// internal
#include "physics_system.hpp"
#include "level_init.hpp"
#include "camera_manager.hpp"
#include <iostream>

// Returns the local bounding coordinates scaled by the current size of the entity
vec2 get_bounding_box(const Motion& motion)
{
	// abs is to avoid negative scale due to the facing direction.
	return { abs(motion.scale.x), abs(motion.scale.y) };
}

bool collide_bottom(const Motion& motion1, const Motion& motion2)
{
	const float player_bottom = motion1.position.y + abs(motion1.scale.y);
	const float terrain_bottom = motion2.position.y + abs(motion2.scale.y);
	const float player_right = motion1.position.x + abs(motion1.scale.x);
	const float terrain_right = motion2.position.x + abs(motion2.scale.x);

	const float b_collision = terrain_bottom - motion1.position.y;
	const float t_collision = player_bottom - motion2.position.y;
	const float l_collision = player_right - motion2.position.x;
	const float r_collision = terrain_right - motion1.position.x;

	if (t_collision < b_collision && t_collision < l_collision && t_collision < r_collision)
	{
		return true;
	}

	return false;
}

bool collide_right(const Motion& motion1, const Motion& motion2)
{
	const float player_bottom = motion1.position.y + abs(motion1.scale.y);
	const float terrain_bottom = motion2.position.y + abs(motion2.scale.y);
	const float player_right = motion1.position.x + abs(motion1.scale.x);
	const float terrain_right = motion2.position.x + abs(motion2.scale.x);

	const float b_collision = terrain_bottom - motion1.position.y;
	const float t_collision = player_bottom - motion2.position.y;
	const float l_collision = player_right - motion2.position.x;
	const float r_collision = terrain_right - motion1.position.x;

	if (l_collision < r_collision && l_collision < t_collision && l_collision < b_collision)
	{
		return true;
	}

	return false;
}

bool collide_left(const Motion& motion1, const Motion& motion2)
{
	const float player_bottom = motion1.position.y + abs(motion1.scale.y);
	const float terrain_bottom = motion2.position.y + abs(motion2.scale.y);
	const float player_right = motion1.position.x + abs(motion1.scale.x);
	const float terrain_right = motion2.position.x + abs(motion2.scale.x);

	const float b_collision = terrain_bottom - motion1.position.y;
	const float t_collision = player_bottom - motion2.position.y;
	const float l_collision = player_right - motion2.position.x;
	const float r_collision = terrain_right - motion1.position.x;

	if (r_collision < l_collision && r_collision < t_collision && r_collision < b_collision)
	{
		return true;
	}

	return false;
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

float round_to_nearest_hundred(float num) {
	return 100 * round(num / 100.0);
}

bool is_above_climbable(Motion& motion, Motion& climbable) {
	
	const vec2 climbable_bb = get_bounding_box(climbable) / 2.f;
	const vec2 char_bb = get_bounding_box(motion) / 2.f;
	const vec2 char_pos = motion.position;
	const vec2 climbable_pos = climbable.position;

	if (char_pos[0] - char_bb[0] <= climbable_pos[0] // x collision left
		&& char_pos[0] + char_bb[0] >= climbable_pos[0] // x collision right
		&& char_pos[1] + char_bb[1] + 0.001 <= (climbable_pos[1] + climbable_bb[1]) // y collision
		) {
		motion.position.y = round(climbable_pos[1] - climbable_bb[1] - char_bb[1]); // tmp avoid fake collisions
		return true;
	}

	return false;
}


bool is_below_climbable(Motion& motion, Motion& climbable) {

	const vec2 climbable_bb = get_bounding_box(climbable) / 2.f;
	const vec2 char_bb = get_bounding_box(motion) / 2.f;
	const vec2 char_pos = motion.position;
	const vec2 climbable_pos = climbable.position;

	if (char_pos[0] - char_bb[0] <= climbable_pos[0]  // x collision left
		&& char_pos[0] + char_bb[0] >= climbable_pos[0]  // x collision right
		&& char_pos[1] + char_bb[1] + 0.001 >= (climbable_pos[1] + climbable_bb[1]) // y collision
		) {
		motion.position.y = round(climbable_pos[1] + climbable_bb[1] - char_bb[1]); // tmp avoid fake collisions
		return true;
	}
	return false;
}

// kind of hacky; this should only ever be called in the update_location context below
bool is_on_climbable(Motion& motion, Motion& climbable) {

	const vec2 climbable_bb = get_bounding_box(climbable) / 2.f;
	const vec2 char_bb = get_bounding_box(motion) / 2.f;
	const vec2 char_pos = motion.position;
	const vec2 climbable_pos = climbable.position;

	if (char_pos[0] - char_bb[0] <= climbable_pos[0]  // x collision left
		&& char_pos[0] + char_bb[0] >= climbable_pos[0]  // x collision right
		) {
		return true;
	}
	return false;
}


void update_location(Motion& motion) {
	auto& climbable_registry = registry.climbables;
	auto& motion_registry = registry.motions;
	for (uint i = 0; i < climbable_registry.size(); i++) {
		Entity& climbable = climbable_registry.entities[i];
		Motion& climbable_motion = motion_registry.get(climbable);

		if (collides(motion, motion_registry.get(climbable))) {
			if (is_below_climbable(motion, climbable_motion)) {
				motion.location = LOCATION::BELOW_CLIMBABLE;
				motion.gravity_affected = false;
				return;
			}
			else if (is_on_climbable(motion, climbable_motion) && !motion.is_falling) {
				motion.location = LOCATION::ON_CLIMBABLE;
				motion.gravity_affected = false;
				return;
			}
		}
		else {
			if (is_above_climbable(motion, climbable_motion)) {
				motion.location = LOCATION::ABOVE_CLIMBABLE;
				motion.gravity_affected = false;
				if (motion.is_falling) {
					motion.goal_velocity.y = 0;
					motion.is_falling = false;
				}
				return;
			}
		}
	}
	motion.location = LOCATION::NORMAL;
	motion.gravity_affected = true;
	motion.position.y = round(motion.position.y);
}

float interpolation_acceleration(float goal_velocity, float current_velocity, bool slippery) {
	
	float acceleration = 20.0f;

	if (slippery == true) {
		acceleration = 5.0f;
	}

	float velocity_difference = goal_velocity - current_velocity;

	if (velocity_difference > acceleration) {
		return current_velocity + acceleration; // increase in velocity
	}
	else if (velocity_difference < -acceleration) {
		return current_velocity - acceleration; // decrease in velocity
	}
	return goal_velocity; // reached goal
}

void PhysicsSystem::step(float elapsed_ms)
{
	// Move entities with motion component with respect to their velocity
	auto& motion_registry = registry.motions;
	for (uint i = 0; i < motion_registry.size(); i++)
	{
		Motion& motion = motion_registry.components[i];
		Entity entity = motion_registry.entities[i];

		// Adapt angle/rotation for projectile motion
		if (registry.projectiles.has(entity)) {
			motion.angle = atan2(motion.goal_velocity.y, motion.goal_velocity.x);
		}

		if (registry.playables.has(entity) || registry.enemies.has(entity)) {
			// update location for players and enemies
			update_location(motion);

			if (motion.slippery == false) {
				motion.prev_position = motion.position;
				motion.position = motion.position + elapsed_ms / 1000.f * motion.goal_velocity;
			}
			else {
				motion.prev_position = motion.position;
				motion.current_velocity.x = interpolation_acceleration(motion.goal_velocity.x, motion.current_velocity.x, motion.slippery);
				motion.current_velocity.y = motion.goal_velocity.y;
				motion.position = motion.position + elapsed_ms / 1000.f * motion.current_velocity;
			}

			updateHealthBar(entity);
			if (registry.activeTurns.has(entity)) {
				updateOrderIndicator(entity);
			}
		}
		
		else {
			motion.prev_position = motion.position;
			motion.current_velocity.x = interpolation_acceleration(motion.goal_velocity.x, motion.current_velocity.x, motion.slippery);
			motion.current_velocity.y = interpolation_acceleration(motion.goal_velocity.y, motion.current_velocity.y, motion.slippery);
			motion.position = motion.position + elapsed_ms / 1000.f * motion.current_velocity;
		}

		if (registry.cameras.has(entity))
		{
			Camera& camera = registry.cameras.get(entity);
			motion.position = clamp(motion.position, camera.lower_limit, camera.higer_limit);
			for (int i = 0; i < registry.backgrounds.size(); i++) {
				Entity& entity = registry.backgrounds.entities[i];
				Motion& background_motion = registry.motions.get(entity);
				float proportion = registry.backgrounds.get(entity).proportion_velocity;
				// TODO: this original position might not be the center of the window in future levels
				vec2 original_position = { window_width_px / 2, window_height_px / 2 };
				vec2 lower_limit_offset = proportion * (camera.lower_limit - original_position);
				vec2 higher_limit_offset = proportion * (camera.higer_limit - original_position);
				background_motion.position = clamp(background_motion.position, original_position + lower_limit_offset, original_position + higher_limit_offset);
			}
		}
		
	}

	// Update overlays relative to main camera
	auto& overlays_registry = registry.overlays;
	Entity camera = get_camera();
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
			Entity entity_j = motion_container.entities[j];
			if (collides(motion_i, motion_j))
			{
				// Create a collisions event
				// We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
				registry.collisions.emplace_with_duplicates(entity_i, entity_j);
				registry.collisions.emplace_with_duplicates(entity_j, entity_i);
			}
		}
	}

	// Check collision between players and terrainss
	auto& characters = registry.initiatives;
	for (uint i = 0; i < characters.size(); i++) {
		Entity character = characters.entities[i];
		Motion& character_motion = registry.motions.get(character);

		auto& terrains = registry.terrains;
		for (uint j = 0; j < terrains.size(); j++) {
			Entity terrain = terrains.entities[j];
			Motion& terrain_motion = registry.motions.get(terrain);

			// Collision Handler
			if (collides(character_motion, terrain_motion)) {
				// Collision between bottom of the character and top of the terrain
				
				// Collision between right of the character and left of the terrain
				if (collide_right(character_motion, terrain_motion)) {
					if (character_motion.goal_velocity.x > 0) {
						character_motion.position.x = character_motion.prev_position.x;
					}
					
				}
				// Collision between left of the character and right of the terrain
				if (collide_left(character_motion, terrain_motion)) {
					if (character_motion.goal_velocity.x < 0) {
						character_motion.position.x = character_motion.prev_position.x;
					}
				}
				if (collide_bottom(character_motion, terrain_motion)
					&& character_motion.location != LOCATION::ABOVE_CLIMBABLE
					&& character_motion.location != LOCATION::ON_CLIMBABLE
					&& character_motion.is_falling) {
					character_motion.goal_velocity.y = 0;
					character_motion.position.y = round_to_nearest_hundred(character_motion.prev_position.y);
					character_motion.is_falling = false;
				}
			}
			else if (character_motion.location == LOCATION::NORMAL) {
				character_motion.goal_velocity.y += gravity * (elapsed_ms / 1000.0f);
				character_motion.is_falling = true;
			}
		}
		if (registry.activeTurns.has(character)) {
			updateOrderIndicator(character);
		}
		updateHealthBar(character);
	}
}
