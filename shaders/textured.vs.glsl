#version 330

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat3 transform;
uniform mat3 projection;

// hit effect
uniform bool hit_effect;
uniform float time;

void main()
{
	texcoord = in_texcoord;
	vec3 position = in_position;

	if (hit_effect) {
		position.x += 0.2 * sin(time * 50);
	}
	vec3 pos = projection * transform * vec3(position.xy, 1.0);
	gl_Position = vec4(pos.xy, position.z, 1.0);
}