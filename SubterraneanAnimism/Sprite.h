#pragma once

#include "SDL.h"
#include "SDL_image.h"
#include "SDLExceptions.h"
#include "CollisionObserver.h"
#include <unordered_set>

class CollisionObserver;

class Sprite
{
public:
	/**
	 * @brief Create sprite from file path
	 * @param path - relative or absolute path
	 */
	Sprite(const char* path);

	Sprite() = default;

	virtual ~Sprite() = default;

	/**
	 * @brief Virtual function for updating the sprite's state
	 * @param deltaTime time elapsed for each frame
	 */
	virtual void update(double deltaTime) = 0;

	/**
	 * @brief Virtual function for drawing the sprite onto the specified SDL surface
	 * @param windowSurface 
	 */
	virtual void draw(SDL_Surface* windowSurface);

	/**
	 * @brief Virtual function for handling SDL events
	 * @param event 
	 */
	virtual void handleEvent(const SDL_Event& event) = 0;

	/**
	 * @brief Returns the position (SDL_Rect) of the sprite.
	 * @return 
	 */
	[[nodiscard]] const SDL_Rect* getPosition() const { return &m_position; }

	/**
	 * @brief determine if sprite is a CollisionSprite
	 * @return 
	 */
	virtual bool isCollisionSprite() { return false; }

	/**
	 * @brief determine if this CollisionSprite collides with another CollisionSprite
	 * @param other
	 * @return bool
	 */
	[[nodiscard]] virtual bool hasCollisionWith(const Sprite& other) { return false; }
protected:
	static SDL_Surface* loadSurface(const char* path);
	SDL_Surface* m_image{};
	SDL_Rect m_position{};
};

class CollisionSprite : public Sprite
{
public:
	CollisionSprite(const char* path) : Sprite(path) { m_observer = nullptr; }

	virtual void handleEvent(const SDL_Event& event) override {};

	virtual void update(double deltaTime) override {};

	/**
	 * @brief determine if sprite is a CollisionSprite
	 * @return
	 */
	bool isCollisionSprite() override { return true; }

	/**
	 * @brief determine if this CollisionSprite collides with another CollisionSprite
	 * @param other 
	 * @return bool
	 */
	[[nodiscard]] bool hasCollisionWith(const Sprite& other) const;

	/**
	 * @brief Sets the observer for the CollisionSprite, allowing it to receive collision notifications.
	 * @param observer A reference to the CollisionObserver to be set as the observer.
	 */
	void setObserver(const CollisionObserver* observer) { m_observer = observer; }

protected:
	const CollisionObserver* m_observer; // Class that will determine collisions
};

class PlayerSprite : public CollisionSprite
{
public:
	PlayerSprite(const char* path, const double speed = 1.0) : CollisionSprite(path), m_speed(speed) {}
	~PlayerSprite() override = default;
	void update(double deltaTime) override;

	/**
	 * @brief Handles player events such as keypresses.
	 * @param event
	 */
	virtual void handleEvent(const SDL_Event& event) override;

	/**
	 * @brief Sets the speed of the player sprite.
	 * @param speed
	 */
	void setSpeed(const double speed) { m_speed = speed; }
	
	/**
	 * @brief Returns the speed of the player sprite.
	 * @return
	 */
	[[nodiscard]] double getSpeed() const { return m_speed; }
protected:
	std::unordered_set<SDL_Keycode> m_pressedKeys;
	double m_speed = 1.0;
	double m_x{};
	double m_y{};
};
