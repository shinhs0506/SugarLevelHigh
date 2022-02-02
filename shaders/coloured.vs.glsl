#version 330

// !!! Simple shader for colouring basic meshes

// Input attributes
in vec3 in_position;

// Application data
uniform mat3 transform;
uniform mat3 projection;

// hit effect
uniform int hit_effect;
uniform float time;

void main()
{
	vec3 position = in_position;
	if (hit_effect == 1) {
		position.x += 0.2 * sin(time * 50);
	}
	vec3 pos = projection * transform * vec3(position.xy, 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}
