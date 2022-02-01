// internal
#include "common.hpp"

#include "level_manager.hpp"
#include "level_init.hpp"
#include "tiny_ecs_registry.hpp"

LevelManager::LevelManager()
{

}

LevelManager::~LevelManager()
{

}

void LevelManager::init(GLFWwindow* window)
{
	this->window = window;
}

void LevelManager::load_level(int level)
{
    this->curr_level = level;
    if (level == 0) {
        Entity enemy = createEnemy(vec2(600, 500), vec2(50, 100));
        Entity player = createPlayer(vec2(500, 500), vec2(50, 100));
        Entity terrain = createTerrain(vec2(600, 600), vec2(800, 50));

        auto compare = [](Entity& a, Entity& b) {
                Initiative& aInitiative = registry.initiatives.get(a);
                Initiative& bInitiative = registry.initiatives.get(b);
                return aInitiative.value > bInitiative.value;
        };
        registry.initiatives.sort(compare);

        // to retrieve current entity
        // registry.initiatives.entities[currOrderIndex];
        // or 
        // registry.activeTurns.entities[0] from outside level_manager
        // or might add a global Entity variable later
        curr_order_index = 0;
        registry.activeTurns.emplace(registry.initiatives.entities[curr_order_index]);
        // for now, since we have one enemy, and one player
        num_characters = registry.initiatives.size();
	}
}

void LevelManager::restart_level()
{

}

void LevelManager::abandon_level()
{

}

bool LevelManager::step(float elapsed_ms)
{

	// remove timed out attack objects
	for (uint i = 0; i < registry.attackObjects.size(); i ++) {
		Entity entity = registry.attackObjects.entities[i];
		AttackObject& obj = registry.attackObjects.get(entity);
		obj.ttl_ms -= elapsed_ms;
		if (obj.ttl_ms < 0) {
			removeAttackObject(entity);
		}
	}

	// remove dead entities (with health component and current health below 0)
	for (uint i = 0; i < registry.healths.size(); i++) {
		Entity entity = registry.healths.entities[i];
		if (registry.healths.get(entity).cur_health <= 0) {
			// check playables
            if (registry.playables.has(entity)) {
				removePlayer(entity);
            } else if (registry.enemies.has(entity)) {
				removeEnemy(entity);
            } else if (registry.terrains.has(entity) && registry.terrains.get(entity).breakable)
				removeTerrain(entity);
            num_characters = registry.initiatives.size();
		}
	}

	return true;
}

void LevelManager::handle_collisions()
{
	// handle attack objects collisions
	for (uint i = 0; i < registry.collisions.size(); i++) {
		Entity entity = registry.collisions.entities[i];
		if (registry.attackObjects.has(entity)) {
			// DO NOT use get() on collisions here!!!
			// collisions registry might have two collisions on the same object
			// using get() will always retrieve the first collision component
			Entity other_entity = registry.collisions.components[i].other;

			AttackObject& attack = registry.attackObjects.get(entity);
			bool is_player_attack = registry.playables.has(attack.attacker);

			// only be able to deal damage on entities with health
			bool damagable = registry.healths.has(other_entity);

			// only be able to deal damage if two entities are different type // (not both playables or enemies) 
			bool different_team = (is_player_attack && !registry.playables.has(other_entity)) ||
				(!is_player_attack && !registry.enemies.has(other_entity));

			// check attacked set to make sure only deal damage once
			bool attacked = attack.attacked.find(other_entity) != attack.attacked.end();

			if (damagable && different_team && !attacked) {
				Health& health = registry.healths.get(other_entity);
				health.cur_health -= attack.damage;
				attack.attacked.insert(other_entity);
			}
		}
	}
}

bool LevelManager::level_ended()
{
	return ended;
}

void LevelManager::on_key(int key, int, int action, int mod)
{

}

void LevelManager::on_mouse_move(vec2 pos)
{

}

void LevelManager::on_mouse_button(int button, int action, int mod)
{
	// tmp use left click to perform attck
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		// get character with current turn
        // there was an error with the initial implementation, we should move forward
        // in turn order whenever an attack move is made, instead of listening for events
        // in the 'step' function.
        // limitation with current implementation: 
        // even though enemy has higher turn order, player would go first
        // TODO: fix ordering
        registry.activeTurns.clear();

        curr_order_index += 1;  
        if (curr_order_index >= num_characters) {
            curr_order_index = 0;
        }

        registry.activeTurns.emplace(registry.initiatives.entities[curr_order_index]);
        Entity player = registry.activeTurns.entities[0];

		// manually calculate a world position with some offsets
		vec2 player_pos = registry.motions.get(player).position;
		double xpos, ypos;

		glfwGetCursorPos(window, &xpos, &ypos);
		vec2 direction = vec2(xpos, ypos) - player_pos;

		vec2 offset{ 75.f, 0.f }; // a bit before the character
		Transform trans;
		trans.translate(player_pos);
		trans.rotate(-atan2(direction[0], direction[1]) + M_PI/2);
		trans.translate(offset);
		
		vec2 attack_pos = trans.mat * vec3(0, 0, 1);
		createAttackObject(player, GEOMETRY_BUFFER_ID::SQUARE, 50.f, 200, 0, attack_pos, vec2(0, 0), vec2(100, 100));
	}
}
