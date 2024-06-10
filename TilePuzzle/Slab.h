#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include "Sprite.h"

class Slab : public ExtendedSprite
{
public:
	Slab(const char* path, const Observer* observer = nullptr, const double speed = 1)
	    : ExtendedSprite(path, speed, observer) {}
    void update(double deltaTime) override;
	void handleEvent(const SDL_Event& event) override;
	
};
