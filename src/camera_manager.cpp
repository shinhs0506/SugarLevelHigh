#include "camera_manager.hpp"
#include "level_init.hpp"
#include "tiny_ecs_registry.hpp"
#include "physics_system.hpp"

#include <iostream>


Entity init_camera() {
	//// init a camera that is shared across all scenes
	//// camera offsets are the same as the window size
    vec2 offset = vec2(window_width_px / 2, window_height_px / 2);
	//// init position at center of the window, which is the same as offset
	//// also set x, y limit to the same as offset so the camera is not really movable
	//// need to modify limits in each level to match the map
    Entity camera = createCamera(offset, offset, offset, offset);
    return camera;
}

void reset_camera_pos() {
    Motion& camera_motion = registry.motions.get(get_camera());
    Camera camera = registry.cameras.get(get_camera());
    vec2 offset = camera.offset;
    camera_motion.position = offset;

    /*
    Motion& camera_motion = registry.motions.get(get_camera());
    Camera camera = registry.cameras.get(get_camera());
    vec2 offset = camera.offset;
    camera.higer_limit = offset;
    camera.lower_limit = offset;
    camera_motion.position = offset;
    */
}

Entity& get_camera() {
    return registry.cameras.entities[0];
}

void update_camera_pos(vec2 pos) {
    Camera& camera = registry.cameras.get(get_camera());
    Motion& camera_motion = registry.motions.get(get_camera());
    vec2 old_pos = camera_motion.position;
    vec2 distance = { pos - old_pos };
    camera_motion.position = pos; 
    camera_motion.position = clamp(camera_motion.position, camera.lower_limit, camera.higer_limit);
    for (int i = 0; i < registry.backgrounds.size(); i++) {
        Entity& entity = registry.backgrounds.entities[i];
        Motion& background_motion = registry.motions.get(entity);
        float proportion = registry.backgrounds.get(entity).proportion_velocity;
        background_motion.position += distance * proportion;
        // TODO: this original position might not be the center of the window in future levels
        vec2 original_position = { window_width_px / 2, window_height_px / 2 };
        vec2 lower_limit_offset = proportion * (camera.lower_limit - original_position);
        vec2 higher_limit_offset = proportion * (camera.higer_limit - original_position);
        background_motion.position = clamp(background_motion.position, original_position + lower_limit_offset, original_position + higher_limit_offset);
    }
}

void move_camera(vec2 velocity) {
    Motion& camera_motion = registry.motions.get(get_camera());
    camera_motion.goal_velocity += velocity;
    for (int i = 0; i < registry.backgrounds.size(); i++) {
        Entity& entity = registry.backgrounds.entities[i];
        Motion& motion = registry.motions.get(entity);
        motion.goal_velocity += velocity * registry.backgrounds.get(entity).proportion_velocity;
    }
}

void update_camera_upper_limit(vec2 upper_limit) {
    Camera& camera = registry.cameras.get(get_camera());
    camera.higer_limit = upper_limit; 
}

void update_camera_lower_limit(vec2 lower_limit) {
    Camera& camera = registry.cameras.get(get_camera());
    camera.lower_limit = lower_limit; 
}
