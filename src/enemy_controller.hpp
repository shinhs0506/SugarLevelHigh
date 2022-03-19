#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include "components.hpp"

// include all logic of enemy behaviors
// implement AI decisions in this file too
class EnemyController
{
public:
	EnemyController();
	~EnemyController();

	void step(float elapsed_ms);

	// reset system upon the start of enemy's turn
	void start_turn(Entity enemy);

	bool should_end_enemy_turn();

private:
	// A small delay before AI moves to allow for player to see AI abit easier
	// and so that AI doesnt instantly do its turn causing player to miss it
	const float DEFAULT_BEGINNING_DELAY = 1000.0f;
	float beginning_delay_counter_ms = DEFAULT_BEGINNING_DELAY;
	
	float move_counter = 0.f;

	CharacterState current_state;
	CharacterState next_state;
	Entity enemy;

	void move_to_state(CharacterState next_state);

	void make_decision();
	void move(Motion& motion, int direction, float distance);
	float cal_actual_attack_range(AttackAbility& ability);
	bool within_attack_range(float dist, AttackAbility& ability);

	Mix_Chunk* melee_attack_sound;
	Mix_Chunk* advanced_attack_sound;
};
