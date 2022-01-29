#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// a red line for debugging purposes
Entity createDebugLine(vec2 position, vec2 size);

Entity createEnemy(vec2 pos, vec2 size);

Entity createPlayer(vec2 pos, vec2 size);

Entity createTerrain(vec2 pos, vec2 size);
