// internal
#include "common.hpp"

#include "level.hpp"

Level::Level()
{

}

Level::~Level()
{

}

void Level::init(int level_num)
{
	this->level_num = level_num;
}

void Level::restart_level()
{

}

void Level::abandon_level()
{

}

bool Level::step(float elapsed_ms)
{
	return true;
}

void Level::handle_collisions()
{

}

bool Level::level_ended()
{
	return ended;
}

void Level::on_key(int key, int, int action, int mod)
{

}

void Level::on_mouse_move(vec2 pos)
{

}
