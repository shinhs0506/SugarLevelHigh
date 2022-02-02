#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;

// hit effect
uniform int hit_effect;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	color = vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
	if (hit_effect == 1) {
		// add some red component
		color = vec4(normalize(color.xyz + vec3(0.3, 0, 0)), color.w);
	}
}
