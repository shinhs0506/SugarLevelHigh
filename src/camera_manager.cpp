#include "camera_manager.hpp"
#include "level_init.hpp"
#include "tiny_ecs_registry.hpp"


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
    vec2 offset = vec2(window_width_px / 2, window_height_px / 2);
    Motion& camera_motion = registry.motions.get(get_camera());
    camera_motion.position = offset;
}

Entity& get_camera() {
    return registry.cameras.entities[0];
}

void update_camera_pos(vec2 pos) {
    Motion& camera_motion = registry.motions.get(get_camera());
    camera_motion.position = pos;
}

#include "iostream"
void move_camera(vec2 velocity) {
    Motion& camera_motion = registry.motions.get(get_camera());
    std::cout << "moving" << std::endl;
    camera_motion.goal_velocity += velocity;
    std::cout << camera_motion.position.x << std::endl;
}

void update_camera_upper_limit(vec2 upper_limit) {
    Camera& camera = registry.cameras.get(get_camera());
    camera.higer_limit = upper_limit; 
    std::cout << camera.higer_limit.x << std::endl;
    std::cout << camera.higer_limit.y << std::endl;
}

void update_camera_lower_limit(vec2 lower_limit) {
    Camera& camera = registry.cameras.get(get_camera());
    camera.lower_limit = lower_limit; 
}
