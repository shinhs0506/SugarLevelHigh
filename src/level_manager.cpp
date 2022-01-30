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

        orders = {enemy, player};
        auto compare = [](Entity& a, Entity& b) {
                Initiative& aInitiative = registry.initiatives.get(a);
                Initiative& bInitiative = registry.initiatives.get(b);
                return aInitiative.value > bInitiative.value;
        };
        registry.initiatives.sort(compare);

        // to retrieve current entity
        // registry.initiatives.components[currOrderIndex];
        currOrderIndex = 0;
        // for now, since we have one enemy, and one player
        numPlayables = 2;
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
    if (shouldAdvanceTurnOrder) {
        currOrderIndex += 1;  
        if (currOrderIndex <= numPlayables) {
            currOrderIndex = 0;
        }
        
        shouldAdvanceTurnOrder = false;
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
