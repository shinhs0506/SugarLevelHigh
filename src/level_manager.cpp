// internal
#include "common.hpp"

#include "level_manager.hpp"
#include "level_init.hpp"

LevelManager::LevelManager()
{

}

LevelManager::~LevelManager()
{

}

void LevelManager::init()
{

}

void LevelManager::load_level(int level)
{
	this->curr_level = level;
	if (level == 0) {
		Entity enemy = createEnemy(vec2(900, 500), vec2(50, 100));
		Entity player = createPlayer(vec2(300, 500), vec2(50, 100));
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
        num_playables = 2;
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
        // check if the turn has ended, advance if so
        if (should_advance_turn_order) {
            // note: clear might be more efficient than 'remove'
            // since we only have one active character
            // needs testing
            registry.initiatives.clear();

            curr_order_index += 1;  
            if (curr_order_index <= num_playables) {
                curr_order_index = 0;
            }

            registry.activeTurns.emplace(registry.initiatives.entities[curr_order_index]);
            
            should_advance_turn_order = false;
        }

	return true;
}

void LevelManager::handle_collisions()
{

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
