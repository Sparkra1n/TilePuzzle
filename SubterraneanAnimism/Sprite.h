#pragma once

#include "SDL.h"
#include "SDL_image.h"
#include "SDLExceptions.h"
#include <unordered_set>

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
	virtual ~Sprite() = default;
	virtual void update(double deltaTime);
	virtual void draw(SDL_Surface* windowSurface);
	virtual void handleEvent(const SDL_Event& event);
	void setSpeed(const double speed) { m_speed = speed; }
	[[nodiscard]] double getSpeed() const { return m_speed; }
private:
	static SDL_Surface* loadSurface(const char* path);

	SDL_Surface* m_image{};
	SDL_Rect m_position{};
	std::unordered_set<SDL_Keycode> m_pressedKeys;
	double m_speed = 1.0;
	double m_x{};
	double m_y{};
};