#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

Entity createGenericButton(vec2 pos, vec2 size, bool (*on_click)());
Entity createBackButton(vec2 pos, vec2 size, bool (*on_click)());
