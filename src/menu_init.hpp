#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

Entity createStartButton(vec2 pos, vec2 size, bool (*on_click)());
Entity createHelpButton(vec2 pos, vec2 size, bool (*on_click)());
Entity createExitButton(vec2 pos, vec2 size, bool (*on_click)());
Entity createTutorialButton(vec2 pos, vec2 size, bool (*on_click)());
Entity createLevel1Button(vec2 pos, vec2 size, bool (*on_click)(), bool disabled = true);
Entity createLevel2Button(vec2 pos, vec2 size, bool (*on_click)(), bool disabled = true);
Entity createLevel3Button(vec2 pos, vec2 size, bool (*on_click)(), bool disabled = true);
Entity createHelpImage(vec2 pos, vec2 size);
Entity createConfigButton(vec2 pos, vec2 size, bool (*on_click)());
