#pragma once

#include "SDL.h"
#include "SDL_image.h"
#include "SDLExceptions.h"

enum class Direction
{
	Nowhere = 0,
	Up,
	Down,
	Left,
	Right,
	UpLeft,
	UpRight,
	DownLeft,
	DownRight
};

class Sprite
{
public:
	explicit Sprite(const char* path);
	Sprite() = default;
	~Sprite() = default;

	void update(double deltaTime);
	void draw(SDL_Surface* windowSurface);
	void handleEvent(SDL_Event const& event);
private:
	static SDL_Surface* loadSurface(const char* path);

	SDL_Surface* m_image{};
	SDL_Rect m_position{};
	Direction m_direction{};
	double m_x{};
	double m_y{};
};