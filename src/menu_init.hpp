#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

Entity createStartButton(vec2 pos, vec2 size);
Entity createHelpButton(vec2 pos, vec2 size, void (*on_click)());
Entity createExitButton(vec2 pos, vec2 size);
Entity createBackButton(vec2 pos, vec2 size, void (*on_click)());
Entity createHelpImage(vec2 pos, vec2 size);
Entity createStartButton(vec2 pos, vec2 size, void (*on_click)());
Entity createExitButton(vec2 pos, vec2 size, void (*on_click)());
