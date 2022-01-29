#version 330

uniform vec3 fcolor;

// Output color
layout(location = 0) out vec4 out_color;

void main()
{
	out_color = vec4(fcolor, 1.0);
}
