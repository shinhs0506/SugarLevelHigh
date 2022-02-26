#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;

// hit effect
uniform bool hit_effect;

// spritesheet
uniform bool is_character;
uniform int movement;
uniform float time;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	if (is_character) {
		// https://gist.github.com/passiomatic/4e108740db98eea5d2e6873267271f37
		vec2 index = vec2(0, 1); // by default, use the sprite on the upper left corner

		if (movement == 1) {
			index = vec2(1, 2);
			if (sin(time) > 0) {
				index.x = 2;
			}
		}
		if (movement == 2) { // move right
			index = vec2(1, 1);
			if (sin(time) > 0) {
				index.x = 2;
			}
		}

		float col = mod(index.x, 3);
		float row = floor(index.y / 2);
		color = vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x * 1/3 + col * 1/3, 1.0 - 1/2 - row * 1/2 + texcoord.y * 1/2));
	}
	else {
		color = vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
	}

	if (hit_effect) {
		// add some red component
		color = vec4(normalize(color.xyz + vec3(0.3, 0, 0)), color.w);
	}
}
