#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_set>
#include "../ext/stb_image/stb_image.h"

// Attached all player characters
struct Playable
{
    Entity healthBar;
};

// Attached to all Enemies
struct Enemy
{
    Entity healthBar;
};

// Attached to all playables, enemies, breakable terrains
struct Health
{
	float max_health = 100.f;
	float cur_health = 100.f;
	bool damage_per_turn = false;
	bool dead = false; // set this to true to remove the entity to all registries
	bool heal_per_turn = false;
};

struct HealthBar
{

};

struct Snow
{

};

// indicates the curr order
struct OrderIndicator
{

};

// Attached to all playables, enemies
// Indicate how far the character can move
struct Energy
{
	float max_energy = 100.f;
	float cur_energy = 100.f;
	float prev_energy = 100.f;
};

struct EnergyBar
{
	
};

// Initiative used to determine the turn order of all enemies and players
// Slower the initiative means faster the speed
// When an enemy and a playable character have the same initiative, the player move first
struct Initiative
{
	int value; // should range [0, 100]
};

// empty struct, attached to entity with current turn order
struct ActiveTurn {
    //    
};

// Represent an attack to be rendered
// Handled by collision with others
struct AttackObject
{
	float ttl_ms;
	float damage;
	Entity attacker;
	std::unordered_set<Entity, EntityHash> attacked;
};

struct AttackAbility
{
	bool activated;
	float speed; // ttl can be approvimated by range/speed
	float damage;
	float range;
	int shape; // This is the GEOMETRY_BUFFER_ID
	vec2 size;
	bool gravity_affected;
	int max_cooldown;
	int current_cooldown;
	int texture_ID;
};

struct AttackArsenal
{
	AttackAbility basic_attack;
	AttackAbility advanced_attack;
};

struct BuffAbility
{
    float movement_speed_delta;
    float health_delta;
    float damage_delta;
	int max_cooldown;
	int current_cooldown;
};

struct BuffArsenal {
    BuffAbility heal;
    // add more if needed
};

struct AttackPreview {

};

// Attached to all projectiles 
struct Projectile
{

};

// Camera is basically a 2d rectangle
struct Camera
{
	// distance of center to x/y edges
	vec2 offset;
	// limit of cameras position so it doesn't move further than limits
	// limit are composed of a coord (x, y)
	vec2 lower_limit;
	vec2 higer_limit;
};

struct PromptWithTimer
{
    float timer;
};

struct BlinkTimer
{
    float timer;
};

// Hit effect object will be created on entity that is hit by an attack
struct HitEffect
{
	float ttl_ms; // hit effect will last this long
};

struct Terrain
{
	bool breakable = false;
};

// proximity to camera
enum DEPTH {
	CAMERA = -1,
	UI_TOP = 0,
	PROMPT = 1,
	COOLDOWN = 2,
	UI = 3,
	ATTACK = 5,
	ACTIVE = 10,
	CHARACTER = 20,
	LADDER = 50,
	TERRAIN = 100,
	BACKGROUND = 1000
};


enum LOCATION {
	NORMAL = 0,
	BELOW_CLIMBABLE = NORMAL + 1,
	ON_CLIMBABLE = BELOW_CLIMBABLE + 1,
	ABOVE_CLIMBABLE = ON_CLIMBABLE + 1
};

enum DIRECTION {
	DIRECTION_UP,
	DIRECTION_DOWN,
	DIRECTION_LEFT,
	DIRECTION_RIGHT
};

// All data relevant to the shape and motion of entities
struct Motion {
	vec2 position = { 0, 0 };
	float angle = 0;
	vec2 current_velocity = { 0, 0 };
	vec2 goal_velocity = { 0, 0 };
	vec2 scale = { 10, 10 };
    vec2 original_scale = { 10, 10 };
	vec2 prev_position = { 0, 0 };
	bool gravity_affected = false;
	bool is_falling = false;
    float speed = 150;
	int depth = DEPTH::CHARACTER;
	int location = LOCATION::NORMAL;
	int slippery = false;
};

// Stucture to store collision information
struct Collision
{
	// Note, the first object is stored in the ECS container.entities
	Entity other; // the second object involved in the collision
	Collision(Entity& other) { this->other = other; };
};

// Components with callback on click
struct Clickable {
	bool (*on_click)();
	bool disabled = false;
	bool on_cooldown = false;
};

// Attached to buttons that use Buff components
struct AbilityButton {

};

// Attached to all button related to player attack/abilities
struct PlayerButton {

};

// Attached to components that are unaffected by camera
struct Overlay {
	vec2 position = { 0, 0 };
	vec2 original_position = { 0, 0 };
};

// Backgrounds
struct Background {
	float proportion_velocity = 1.0;
};

// UI components
struct UiComponent {
	
};

// Ladders
struct Climbable {

};

// Data structure for toggling debug mode
struct Debug {
	bool in_debug_mode = 0;
	bool in_freeze_mode = 0;
};
extern Debug debugging;

// Sets the brightness of the screen
struct ScreenState
{
	float darken_screen_factor = -1;
};

// A struct to refer to debugging graphics in the ECS
struct DebugComponent
{
	// Note, an empty struct has size 1
};

// Single Vertex Buffer element for non-textured meshes (coloured.vs.glsl & chicken.vs.glsl)
struct ColoredVertex
{
	vec3 position;
	vec3 color;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

// Mesh datastructure for storing vertex and index buffers
struct Mesh
{
	static bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, vec2& out_size);
	vec2 original_size = {1,1};
	std::vector<ColoredVertex> vertices;
	std::vector<uint16_t> vertex_indices;
};

struct AI
{
	// Only horizontal movement so far
	vec2 movement_direction = vec2(-1, 0);
};

struct CoolDown 
{

};

// Character states for players and enemies
// This will be later used for animation system as well

enum class CharacterState
{
	IDLE,
	MOVE_LEFT,
	MOVE_RIGHT,
	MOVE_UP,
	MOVE_DOWN,
    PERFORM_ABILITY_AUTO,
	PERFORM_ABILITY_MANUAL,
	END, // should not move to any other states from here
		 // this is set to prevent player continue to act after his turn
};

/**
 * The following enumerators represent global identifiers refering to graphic
 * assets. For example TEXTURE_ASSET_ID are the identifiers of each texture
 * currently supported by the system.
 *
 * So, instead of referring to a game asset directly, the game logic just
 * uses these enumerators and the RenderRequest struct to inform the renderer
 * how to structure the next draw command.
 *
 * There are 2 reasons for this:
 *
 * First, game assets such as textures and meshes are large and should not be
 * copied around as this wastes memory and runtime. Thus separating the data
 * from its representation makes the system faster.
 *
 * Second, it is good practice to decouple the game logic from the render logic.
 * Imagine, for example, changing from OpenGL to Vulkan, if the game logic
 * depends on OpenGL semantics it will be much harder to do the switch than if
 * the renderer encapsulates all asset data and the game logic is agnostic to it.
 *
 * The final value in each enumeration is both a way to keep track of how many
 * enums there are, and as a default value to represent uninitialized fields.
 */

enum class TEXTURE_ASSET_ID {
	PLAYER = 0,
	ENEMY = PLAYER + 1,
	TERRAIN1 = ENEMY + 1,
	TERRAIN2 = TERRAIN1 + 1,
	TERRAIN2_BREAKABLE = TERRAIN2 + 1,
	TERRAIN3 = TERRAIN2_BREAKABLE + 1,
	TERRAIN3_BREAKABLE = TERRAIN3 + 1,
	TERRAIN4 = TERRAIN3_BREAKABLE + 1,
	START_BUTTON = TERRAIN4 + 1,
	HELP_BUTTON = START_BUTTON + 1,
	EXIT_BUTTON = HELP_BUTTON + 1,
	BACK_BUTTON = EXIT_BUTTON + 1,
	SAVE_BUTTON = BACK_BUTTON + 1,
	CONFIG_BUTTON = SAVE_BUTTON + 1,
	CONFIG1 = CONFIG_BUTTON + 1,
	CONFIG2 = CONFIG1 + 1,
	CONFIG3 = CONFIG2 + 1,
	CONFIG4 = CONFIG3 + 1,
	CONFIG5 = CONFIG4 + 1,
	CONFIG6 = CONFIG5 + 1,
	BACKGROUND_MAIN = CONFIG6 + 1,
	BACKGROUND_BLANK = BACKGROUND_MAIN + 1,
	TITLE = BACKGROUND_BLANK + 1,
	HELP_IMAGE = TITLE + 1,
	ADVANCED_ATTACK_PREVIEW = HELP_IMAGE + 1,
	BASIC_ATTACK_PREVIEW = ADVANCED_ATTACK_PREVIEW + 1,
	TUTORIAL_BUTTON = BASIC_ATTACK_PREVIEW + 1,
	LEVEL_1_BUTTON = TUTORIAL_BUTTON + 1,
	LEVEL_2_BUTTON = LEVEL_1_BUTTON + 1,
	LEVEL_3_BUTTON = LEVEL_2_BUTTON + 1,
	LEVEL_4_BUTTON = LEVEL_3_BUTTON + 1,
	TUTORIAL_MOVE = LEVEL_4_BUTTON + 1,
	TUTORIAL_ATTACK_BASIC = TUTORIAL_MOVE + 1,
	TUTORIAL_ATTACK_ADVANCED = TUTORIAL_ATTACK_BASIC + 1,
	TUTORIAL_HEAL = TUTORIAL_ATTACK_ADVANCED + 1,
	TUTORIAL_DEFEAT = TUTORIAL_HEAL + 1,
	TUTORIAL_END = TUTORIAL_DEFEAT + 1,
	MELEE_ATTACK = TUTORIAL_END + 1,
	BEAR_ADVANCED_ATTACK = MELEE_ATTACK + 1,
	CHOCOLATE_ADVANCED_ATTACK = BEAR_ADVANCED_ATTACK + 1,
	BACKGROUND1 = CHOCOLATE_ADVANCED_ATTACK + 1,
	BACKGROUND11 = BACKGROUND1 + 1,
	BACKGROUND12 = BACKGROUND11 + 1,
	BACKGROUND2 = BACKGROUND12 + 1,
	BACKGROUND21 = BACKGROUND2 + 1,
	BACKGROUND22 = BACKGROUND21 + 1,
	BACKGROUND3 = BACKGROUND22 + 1,
	BACKGROUND31 = BACKGROUND3 + 1,
	BACKGROUND32 = BACKGROUND31 + 1,
	BACKGROUND4 = BACKGROUND32 + 1,
	BACKGROUND41 = BACKGROUND4 + 1,
	BACKGROUND42 = BACKGROUND41 + 1,
	LADDER = BACKGROUND42 + 1,
	HEALTH_ABILITY = LADDER + 1,
	LEVEL_WON = HEALTH_ABILITY + 1,
	LEVEL_LOST = LEVEL_WON + 1,
	TUTORIAL_FAIL = LEVEL_LOST + 1,
	STORY1 = TUTORIAL_FAIL + 1,
	STORY2 = STORY1 + 1,
	STORY3 = STORY2 + 1,
	STORY4 = STORY3 + 1,
	STORY5 = STORY4 + 1,
	STORY11 = STORY5 + 1,
	STORY12 = STORY11 + 1,
	STORY13 = STORY12 + 1,
	STORY14 = STORY13 + 1,
	STORY15 = STORY14 + 1,
	STORY16 = STORY15 + 1,
	STORY17 = STORY16 + 1,
	NEXT_BUTTON = STORY17 + 1,
	UI_LAYOUT = NEXT_BUTTON + 1,
	TURN_INDICATOR = UI_LAYOUT + 1,
	BAR = TURN_INDICATOR + 1,
	COOLDOWN1 = BAR + 1,
	COOLDOWN2 = COOLDOWN1 + 1,
	COOLDOWN3 = COOLDOWN2 + 1,
	PROMPT_SAVED = COOLDOWN3 + 1,
	PROMPT_BASIC_ATTACK = PROMPT_SAVED + 1,
	PROMPT_ADVANCED_ATTACK = PROMPT_BASIC_ATTACK + 1,
	PROMPT_HEAL_ABILITY = PROMPT_ADVANCED_ATTACK + 1,
	PROMPT_ABILITY_COOLDOWN = PROMPT_HEAL_ABILITY + 1,
	PROMPT_NO_ENERGY = PROMPT_ABILITY_COOLDOWN + 1,
	SNOW1 = PROMPT_NO_ENERGY + 1,
	SNOW2 = SNOW1 + 1,
	SNOW3 = SNOW2 + 1,
	SNOW4 = SNOW3 + 1,
	SNOW5 = SNOW4 + 1,
	SNOW6 = SNOW5 + 1,
	LEVEL_1_START = SNOW6 + 1,
	LEVEL_2_START = LEVEL_1_START + 1,
	LEVEL_3_START = LEVEL_2_START + 1,
	LEVEL_4_START = LEVEL_3_START + 1,
	BUTTON_MELEE = LEVEL_4_START + 1,
	BUTTON_ADVANCED = BUTTON_MELEE + 1,
	BUTTON_HEAL = BUTTON_ADVANCED + 1,
	LEVEL_1_LOST = BUTTON_HEAL + 1,
	LEVEL_2_LOST = LEVEL_1_LOST + 1,
	LEVEL_3_LOST = LEVEL_2_LOST + 1,
	LEVEL_4_LOST = LEVEL_3_LOST + 1,
	TEXTURE_COUNT = LEVEL_4_LOST + 1,
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID {
	COLOURED = 0,
	TEXTURED = COLOURED + 1,
	POST_PROCESS = TEXTURED + 1, // post processing
	EFFECT_COUNT = POST_PROCESS + 1
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID {
	SPRITE = 0,
	EGG = SPRITE + 1,
	SQUARE = EGG + 1,
	DEBUG_LINE = SQUARE + 1,
	SCREEN_TRIANGLE = DEBUG_LINE + 1,
	GEOMETRY_COUNT = SCREEN_TRIANGLE + 1
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

struct RenderRequest {
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};
