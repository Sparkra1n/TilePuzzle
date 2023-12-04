#pragma once

#include <iostream>
#include "Sprite.h"

class CollisionObserver
{
public:
	virtual bool canMoveTo(CollisionSprite& collisionSprite, double potentialX, double potentialY);
	virtual ~CollisionObserver() = default;
protected:

};

