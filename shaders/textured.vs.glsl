#version 330

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat3 transform;
uniform mat3 transforms[50];
uniform mat3 projection;

// hit effect
uniform bool hit_effect;
uniform float time;
uniform bool is_snow;

void main()
{
	texcoord = in_texcoord;
	vec3 position = in_position;

	if (hit_effect) {
		position.x += 0.2 * sin(time * 50);
	}
	if (is_snow) {
		vec3 pos = projection * transforms[gl_InstanceID] * vec3(in_position.xy, 1.0);
		gl_Position = vec4(pos.xy, position.z, 1.0);
	}
	else {
		vec3 pos = projection * transform * vec3(position.xy, 1.0);
		gl_Position = vec4(pos.xy, position.z, 1.0);
	}
	
}

