#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;

// hit effect
uniform bool hit_effect;

// color disabled UI elements 
uniform bool disabled;

// spritesheet
uniform bool is_character;
uniform bool is_ladder;
uniform bool is_enemy;
uniform int movement;
uniform float time;

uniform int ladder_height;

uniform float blink;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	if (is_character) {
		// https://gist.github.com/passiomatic/4e108740db98eea5d2e6873267271f37
		vec2 index = vec2(0, 0); // by default, use the sprite on the upper left corner

		if (movement == 1) { // move left
			index = vec2(1, 1);
			if (sin(time) > 0) {
				index.x = 2;
			}
		}
		if (movement == 2) { // move right
			index = vec2(1, 0);
			if (sin(time) > 0) {
				index.x = 2;
			}
		}
		if (movement == 3) { // falling
			index = vec2(2, 2);
		}
		if (movement == 4) { // climb
			index = vec2(0, 2);
			if (sin(time) > 0) {
				index.x = 1;
			}
		}

		float col = mod(index.x, 3);
		float row = mod(index.y, 3);
		color = vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x * 1/3 + col * 1/3, texcoord.y * 1/3 + row * 1/3));
	}
	else if (is_ladder) {
			color = vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y * ladder_height));
	}

	else {
		color = vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
	}

    if (is_enemy) {
        color = vec4(color.xyz + (((sin((0.01 * blink + 3) * (3.14/2))+1)/2) * (vec3(1, 0, 0))), color.w);
    }

	if (hit_effect) {
		// add some red component
		color = vec4(normalize(color.xyz + vec3(0.3, 0, 0)), color.w);
	}
	
	if (disabled) {
		// make the button greyscale
		vec3 greyscale = vec3(dot( color.xyz, vec3(0.3, 0.3, 0.3)));
		color = vec4(greyscale.x, greyscale.y, greyscale.z, color.w);
	}
}