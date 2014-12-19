#pragma once

#include "World.h"

class							TransformSystem
{
public:
	static void					update(World &world, const sf::Time &elapsed);
private:
	TransformSystem();
	TransformSystem(const TransformSystem &rhs);
	TransformSystem &operator=(const TransformSystem &rhs);

	static void					updatePosition(TransformComponent *xform, MovementComponent *mov, const sf::Time &elapsed);
	static void					computeTransform(TransformComponent *xform);
};