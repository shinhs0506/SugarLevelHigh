#version 330

uniform vec3 fcolor;
uniform int hit_effect;

// Output color
layout(location = 0) out vec4 out_color;

void main()
{
	if (hit_effect == 1) {
		// turn to red on hit
		out_color = vec4(1.0, 0.0, 0.0, 1.0);
	} else {
		out_color = vec4(fcolor, 1.0);
	}
}
