#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

Entity createStartButton(vec2 pos, vec2 size, bool (*on_click)());
Entity createExitButton(vec2 pos, vec2 size, bool (*on_click)());
