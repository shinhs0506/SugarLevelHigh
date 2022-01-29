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
		createEnemy(vec2(900, 500), vec2(50, 100));
		createPlayer(vec2(300, 500), vec2(50, 100));
		createTerrain(vec2(600, 600), vec2(800, 50));
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
