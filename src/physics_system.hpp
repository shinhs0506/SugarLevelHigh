#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "components.hpp"
#include "tiny_ecs_registry.hpp"

bool collides(const Motion& motion1, const Motion& motion2);

// A simple physics system that moves rigid bodies and checks for collision
class PhysicsSystem
{
public:
	void step(float elapsed_ms);

	PhysicsSystem()
	{
	}

private:
	const float gravity = 0.0f; //set to 100.0f when terrain collision is implemented
};