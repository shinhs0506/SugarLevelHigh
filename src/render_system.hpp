#pragma once

#include <array>
#include <utility>

#include "common.hpp"
#include "components.hpp"
#include "tiny_ecs.hpp"

// System responsible for setting up OpenGL and for rendering all the
// visual entities in the game
class RenderSystem {
	/**
	 * The following arrays store the assets the game will use. They are loaded
	 * at initialization and are assumed to not be modified by the render loop.
	 *
	 * Whenever possible, add to these lists instead of creating dynamic state
	 * it is easier to debug and faster to execute for the computer.
	 */
	std::array<GLuint, texture_count> texture_gl_handles;
	std::array<ivec2, texture_count> texture_dimensions;

	// Make sure these paths remain in sync with the associated enumerators.
	// Associated id with .obj path
	const std::vector < std::pair<GEOMETRY_BUFFER_ID, std::string>> mesh_paths = {};

	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, texture_count> texture_paths =
	{
		// textures_path("player.png"),
		textures_path("playable_gummy_bear.png"),
		textures_path("enemy.png"),
		textures_path("terrain.png"),
		textures_path("terrain_breakable.png"),
		textures_path("button_start.png"),
		textures_path("button_help.png"),
		textures_path("button_exit.png"),
		textures_path("back_button.png"),
		textures_path("save_button.png"),
		textures_path("basic_help.png"),
		textures_path("advanced_attack_preview.png"),
		textures_path("basic_attack_preview.png"),
		textures_path("button_tutorial.png"),
		textures_path("button_level_1.png"),
		textures_path("button_level_2.png"),
		textures_path("button_level_3.png"),
		textures_path("tutorial_move.png"),
		textures_path("tutorial_attack_basic.png"),
		textures_path("tutorial_attack_advanced.png"),
		textures_path("tutorial_cooldown.png"),
		textures_path("tutorial_complete.png"),
		textures_path("melee_slash.png"), // Credit for this image goes to Matheus Carvalho
		textures_path("bear_advanced_attack.png"),
		textures_path("chocolate_advanced_attack.png"),
		textures_path("background1.png"),
		textures_path("background11.png"),
		textures_path("background12.png"),
		textures_path("background2.png"),
		textures_path("background21.png"),
		textures_path("background22.png"),
		textures_path("ladder.png"), // https://free-game-assets.itch.io/free-swamp-2d-tileset-pixel-art
		textures_path("health_potion.png"), // Credit for this images goes to "magdum" found at https://opengameart.org/content/basic-potion-set
		textures_path("level_won.png"),
		textures_path("level_lost.png"),
		textures_path("tutorial_fail.png"),
		textures_path("story1.png"),
		textures_path("story2.png"),
		textures_path("story3.png"),
		textures_path("story4.png"),
		textures_path("story5.png"),
		textures_path("next_button.png"),
		textures_path("ui_layout.png"),
		textures_path("turn_indicator.png"), // https://pipoya.itch.io/free-popup-emotes-pack
		textures_path("bar.png"),
		textures_path("cooldown1.png"),
		textures_path("cooldown2.png"),
		textures_path("cooldown3.png"),
    textures_path("prompt_saved.png"),
    textures_path("prompt_basic_attack.png"),
    textures_path("prompt_advanced_attack.png"),
    textures_path("prompt_heal_ability.png"),
    textures_path("prompt_ability_cooldown.png"),
    textures_path("prompt_no_energy.png"),
	};

	std::array<GLuint, effect_count> effects;
	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, effect_count> effect_paths = 
	{
		shader_path("coloured"),
		shader_path("textured"),
		shader_path("post_process"),
	};

	std::array<GLuint, geometry_count> vertex_buffers;
	std::array<GLuint, geometry_count> index_buffers;
	std::array<Mesh, geometry_count> meshes;

public:
	// Initialize the window
	bool init(GLFWwindow* window);

	template <class T>
	void bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices);

	void initializeGlTextures();

	void initializeGlEffects();

	void initializeGlMeshes();
	Mesh& getMesh(GEOMETRY_BUFFER_ID id) { return meshes[(int)id]; };

	void initializeGlGeometryBuffers();
	// Initialize the screen texture used as intermediate render target
	// The draw loop first renders to this texture, then it is used for the wind
	// shader
	bool initScreenTexture();

	// Destroy resources associated to one or all entities created by the system
	~RenderSystem();

	void deleteFrameBuffer();
	void remakeFrameBuffer(GLFWwindow* window_arg, int new_width, int new_height);

	// Draw all entities
	void draw();

	mat3 createProjectionMatrix();

private:
	// Internal drawing functions for each entity type
	void drawTexturedMesh(Entity entity, const mat3& projection);
	void drawToScreen();

	// Window handle
	GLFWwindow* window;

	// Screen texture handles
	GLuint frame_buffer;
	GLuint off_screen_render_buffer_color;
	GLuint off_screen_render_buffer_depth;

	Entity screen_state_entity;
};

bool loadEffectFromFile(
	const std::string& vs_path, const std::string& fs_path, GLuint& out_program);
