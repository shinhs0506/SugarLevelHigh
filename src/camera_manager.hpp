#include "components.hpp"

Entity init_camera();
void reset_camera_pos();
void reset_camera();
Entity& get_camera();

void update_camera_pos(vec2 pos);
void move_camera(vec2 velocity);
void update_camera_upper_limit(vec2 upper_limit);
void update_camera_lower_limit(vec2 lower_limit);
