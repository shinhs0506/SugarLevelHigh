// internal
#include "render_system.hpp"
#include <SDL.h>

#include "tiny_ecs_registry.hpp"

#include <iostream>

struct {
	bool operator()(Entity entity1, Entity entity2) const {
		return registry.motions.get(entity1).depth > registry.motions.get(entity2).depth;
	}
} compare_depths; 


void RenderSystem::drawSnow()
{
	if (registry.snows.size() == 0) {
		return;
	}
	std::vector<Transform> snow_transforms;
	for (uint i = 0; i < registry.snows.size(); i++) {
		Entity& entity = registry.snows.entities[i];
		Motion& motion = registry.motions.get(entity);
		Transform transform;
		transform.translate(motion.position);
		transform.rotate(motion.angle);
		transform.scale(motion.scale);
		snow_transforms.push_back(transform);
	}

	Entity& entity = registry.snows.entities[0];
	const RenderRequest& render_request = registry.renderRequests.get(entity);
	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
	const GLuint program = (GLuint)effects[used_effect_enum];
	const mat3 projection = createProjectionMatrix();

	// Setting shaders
	glUseProgram(program);
	gl_has_errors();

	assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
	const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	GLint in_position_loc = glGetAttribLocation(program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
	gl_has_errors();
	assert(in_texcoord_loc >= 0);

	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
		sizeof(TexturedVertex), (void*)0);
	gl_has_errors();

	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(
		in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
		(void*)sizeof(
			vec3)); // note the stride to skip the preceeding vertex position

	// Enabling and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	gl_has_errors();

	assert(registry.renderRequests.has(entity));
	GLuint texture_id =
		texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

	glBindTexture(GL_TEXTURE_2D, texture_id);
	gl_has_errors();

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(program, "fcolor");
	const vec3 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec3(1);
	glUniform3fv(color_uloc, 1, (float*)&color);
	gl_has_errors();

	GLuint is_snow_loc = glGetUniformLocation(program, "is_snow");
	glUniform1i(is_snow_loc, 1);
	gl_has_errors();

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();
	
	GLsizei num_indices = size / sizeof(uint16_t);

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);

	for (uint i = 0; i < registry.snows.size(); i++) {
		std::string loc = "transforms[" + std::to_string(i) + "]";
		GLuint transforms_loc = glGetUniformLocation(currProgram, loc.c_str());
		assert(transforms_loc >= 0);
		glUniformMatrix3fv(transforms_loc, 1, GL_FALSE, (float*)&snow_transforms[i].mat);
	}

	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float*)&projection);

	gl_has_errors();
	glDrawElementsInstanced(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr, registry.snows.size());
	gl_has_errors();
}

void RenderSystem::drawTexturedMesh(Entity entity,
									const mat3 &projection)
{
	Motion &motion = registry.motions.get(entity);
	// Transformation code, see Rendering and Transformation in the template
	// specification for more info Incrementally updates transformation matrix,
	// thus ORDER IS IMPORTANT
	Transform transform;
	transform.translate(motion.position);
	transform.rotate(motion.angle);
	transform.scale(motion.scale);

	assert(registry.renderRequests.has(entity));
	const RenderRequest &render_request = registry.renderRequests.get(entity);

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
	const GLuint program = (GLuint)effects[used_effect_enum];

	// Setting shaders
	glUseProgram(program);
	gl_has_errors();

	assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
	const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	// Input data location as in the vertex buffer
	if (render_request.used_effect == EFFECT_ASSET_ID::TEXTURED)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(TexturedVertex), (void *)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void *)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		assert(registry.renderRequests.has(entity));
		GLuint texture_id =
			texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

		if (registry.climbables.has(entity)) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}

		if (registry.energyBars.has(entity)) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
		
		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::COLOURED) 
	{
		// COLOURED effect uses colored vertex
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		gl_has_errors();

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(ColoredVertex), (void*)0);
		gl_has_errors();
	}
	else
	{
		assert(false && "Type of render request not supported");
	}

	GLuint is_snow_loc = glGetUniformLocation(program, "is_snow");
	glUniform1i(is_snow_loc, 0);

	// character uniform: check whether the entity is a character with a spritesheet
	GLint is_character_uloc = glGetUniformLocation(program, "is_character");
	glUniform1i(is_character_uloc, registry.playables.has(entity) || registry.enemies.has(entity));
	gl_has_errors();

	// movement uniform
	GLint movement_uloc = glGetUniformLocation(program, "movement");
	int movement = 0; // idle
	if (motion.goal_velocity.x < 0 && motion.location != LOCATION::ON_CLIMBABLE) movement = 1; // left
	if (motion.goal_velocity.x > 0 && motion.location != LOCATION::ON_CLIMBABLE) movement = 2; // right
	if ((motion.location == LOCATION::NORMAL) && motion.position.y != motion.prev_position.y) movement = 3; // falling
	if (motion.location == LOCATION::ON_CLIMBABLE && motion.goal_velocity.y != 0) movement = 4; // climb
	if (motion.location == LOCATION::ON_CLIMBABLE && motion.goal_velocity.y == 0) movement = 5; // stop on the ladder
	glUniform1i(movement_uloc, movement);
	gl_has_errors();

	// ladder uniforms:
	GLint is_ladder_uloc = glGetUniformLocation(program, "is_ladder");
	glUniform1i(is_ladder_uloc, registry.climbables.has(entity));
	GLint ladder_height_uloc = glGetUniformLocation(program, "ladder_height");
	glUniform1i(ladder_height_uloc, registry.climbables.has(entity) ? round(registry.motions.get(entity).scale.y / 100.f) : 0);
	gl_has_errors();

	// energy/health uniforms:
	GLint is_bar_uloc = glGetUniformLocation(program, "is_bar");
	glUniform1i(is_bar_uloc, registry.energyBars.has(entity) || registry.healthBars.has(entity));
	GLint bar_uloc = glGetUniformLocation(program, "bar");
	glUniform1i(bar_uloc, (registry.energyBars.has(entity) || registry.healthBars.has(entity)) ? ceil(registry.motions.get(entity).scale.x / 20.f) : 0);
	gl_has_errors();

	// hit by an attack uniform
	GLint hit_effect_uloc = glGetUniformLocation(program, "hit_effect");
	glUniform1i(hit_effect_uloc, registry.hitEffects.has(entity));
	gl_has_errors();

	// disabled button uniform
	GLint disabled_uloc = glGetUniformLocation(program, "disabled");
	glUniform1i(disabled_uloc, (registry.clickables.has(entity) && registry.clickables.get(entity).disabled));
	gl_has_errors();

	// Cooldown uniform
	GLint cooldown_uloc = glGetUniformLocation(program, "on_cooldown");
	glUniform1i(cooldown_uloc, (registry.clickables.has(entity) && registry.clickables.get(entity).on_cooldown));
	gl_has_errors();

	// pass a time uniform
	GLint time_uloc = glGetUniformLocation(program, "time");
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
	gl_has_errors();

	GLint is_enemy_uloc = glGetUniformLocation(program, "is_enemy");
	glUniform1i(is_enemy_uloc, registry.enemies.has(entity));
	gl_has_errors();

    GLint timer_uloc = glGetUniformLocation(program, "blink");
    glUniform1f(timer_uloc, registry.blinkTimers.size() > 0 ? registry.blinkTimers.components[0].timer : 0);
    gl_has_errors();

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(program, "fcolor");
	const vec3 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec3(1);
	glUniform3fv(color_uloc, 1, (float *)&color);
	gl_has_errors();

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	GLsizei num_indices = size / sizeof(uint16_t);
	// GLsizei num_triangles = num_indices / 3;

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
	// Setting uniform values to the currently bound program
	GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
	glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float *)&transform.mat);
	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float *)&projection);
	gl_has_errors();
	// Drawing of num_indices/3 triangles specified in the index buffer
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	gl_has_errors();
}

// draw the intermediate texture to the screen, with some distortion to simulate
// wind
void RenderSystem::drawToScreen()
{
	// Setting shaders
	// get the wind texture, sprite mesh, and program
	glUseProgram(effects[(GLuint)EFFECT_ASSET_ID::POST_PROCESS]);
	gl_has_errors();
	// Clearing backbuffer
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearColor(1.f, 0, 0, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_has_errors();
	// Enabling alpha channel for textures
	glDisable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Draw the screen texture on the quad geometry
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]);
	glBindBuffer(
		GL_ELEMENT_ARRAY_BUFFER,
		index_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]); // Note, GL_ELEMENT_ARRAY_BUFFER associates
																	 // indices to the bound GL_ARRAY_BUFFER
	gl_has_errors();
	const GLuint post_program = effects[(GLuint)EFFECT_ASSET_ID::POST_PROCESS];
	// Set clock
    GLuint time_uloc = glGetUniformLocation(post_program, "time");
    GLuint dead_timer_uloc = glGetUniformLocation(post_program, "darken_screen_factor");
    glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
    ScreenState &screen = registry.screenStates.get(screen_state_entity);
    glUniform1f(dead_timer_uloc, screen.darken_screen_factor);
    gl_has_errors();
	// Set the vertex position and vertex texture coordinates (both stored in the
	// same VBO)
	GLint in_position_loc = glGetAttribLocation(post_program, "in_position");
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);
	gl_has_errors();

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
	gl_has_errors();
	// Draw
	glDrawElements(
		GL_TRIANGLES, 3, GL_UNSIGNED_SHORT,
		nullptr); // one triangle = 3 vertices; nullptr indicates that there is
				  // no offset from the bound index buffer
	gl_has_errors();
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw()
{
	// Getting size of window
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays
	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();
	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	glClearColor(0.674, 0.847, 1.0 , 1.0);
	glClearDepth(10.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST); // native OpenGL does not work with a depth buffer
							  // and alpha blending, one would have to sort
							  // sprites back to front
	gl_has_errors();
	mat3 projection_2D = createProjectionMatrix();

	// Sort the render requests in depth order (painter's algorithm)
	registry.renderRequests.sort(compare_depths);

	
	// Draw all textured meshes that have a position and size component
	for (Entity entity : registry.renderRequests.entities)
	{
		if (!registry.motions.has(entity))
			continue;
		if (registry.snows.has(entity)) {
			continue;
		}
		// Note, its not very efficient to access elements indirectly via the entity
		// albeit iterating through all Sprites in sequence. A good point to optimize
		drawTexturedMesh(entity, projection_2D);
	}
	drawSnow();
	
	// Truely render to the screen
	drawToScreen();

	// flicker-free display with a double buffer
	glfwSwapBuffers(window);
	gl_has_errors();
}

mat3 RenderSystem::createProjectionMatrix()
{
	// Fake projection matrix, scales with respect to window coordinates
	// affected by main camera
	Entity entity = registry.cameras.entities[0];
	vec2 offset = registry.cameras.components[0].offset;
	vec2 pos = registry.motions.get(entity).position;

	float left = pos[0] - offset[0];
	float top = pos[1] - offset[1];

	gl_has_errors();
	float right = pos[0] + offset[0];
	float bottom = pos[1] + offset[1];

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);

	return {{sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f}};
}
