/**
 * @file Sprite.h
 * @brief SDL Sprite classes
 * @author Ani
 * @version 0.1
 * @date 2023-11-26
 */

#pragma once
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <random>
#include "SDLExceptions.h"
#include "Vector2.h"
#include "Observer.h"


/**
 * Base - 3 impl
 * Template that returns false for all other cases
 * Rectangular - Rectangular / Rectangular - Polygon
 * Polygon - Polygon
 */


// Type for specifying collision detection system
struct NoCollision;
struct RectangularCollision;
struct PolygonCollision;

template <typename T>
struct IsAllowedCollisionMethod : std::disjunction<
    std::is_same<T, NoCollision>,
    std::is_same<T, RectangularCollision>,
    std::is_same<T, PolygonCollision>
> {};

template <typename T, std::enable_if_t<IsAllowedCollisionMethod<T>::value, bool> = true>
class Sprite;

/**
 * \brief Base class for all game objects
 */
class Entity
{
public:
    Entity() = default;
    virtual ~Entity() = default;

    /**
     * \brief Performs any time-related calculations
     * \param deltaTime 
     */
    virtual void update(double deltaTime);

    /**
     * \brief Whether not the object is a Sprite
     * \returns false for any base class or class having Sprite as a member
     */
    [[nodiscard]] virtual bool isSpecializedSprite() const;

    /**
     * \brief Returns the Sprite's SDL_Rect
     * \return SDL_Rect
     */
    [[nodiscard]] virtual SDL_Rect getSDLRect() const = 0;


    [[nodiscard]] virtual SDL_Texture* getCachedTexture() const = 0;

    /**
     * \brief 
     * \param other other sprite
     * \param potentialPosition potential position vector of this sprite
     * \return bool
     */
    [[nodiscard]]
	virtual bool hasCollisionWith(const Entity& other, Vector2<double> potentialPosition) const { return false; }

    //TODO: y
//protected:
    /**
     * \brief Virtual method for collision checking
     * \param other 
     * \param potentialPosition potential position vector of the other sprite
     * \return bool
     */
    [[nodiscard]]
	virtual bool hasCollisionWithImpl(const Sprite<NoCollision>& other, Vector2<double> potentialPosition) const { return false; }

    /**
	 * \brief Virtual method for collision checking
	 * \param other
	 * \param potentialPosition potential position vector of the other sprite
	 * \return bool
	 */
    [[nodiscard]]
    virtual bool hasCollisionWithImpl(const Sprite<RectangularCollision>& other, Vector2<double> potentialPosition) const { return false; }

    /**
	 * \brief Virtual method for collision checking
	 * \param other
	 * \param potentialPosition potential position vector of the other sprite
	 * \return bool
	 */
    [[nodiscard]]
    virtual bool hasCollisionWithImpl(const Sprite<PolygonCollision>& other, Vector2<double> potentialPosition) const { return false; }
};

class SpriteBase : public Entity
{
public:
    SpriteBase(const char* path, const Observer* observer = nullptr);
    SpriteBase(SDL_Rect rect, SDL_Color color);
    SpriteBase(const SpriteBase& other);
    ~SpriteBase() override;
    virtual void setCoordinates(Vector2<double> coordinates);
    void cacheTexture(SDL_Renderer* renderer);
    [[nodiscard]] static SDL_Color generateRandomColor();
    [[nodiscard]] virtual bool isCollisionSprite() const;
    [[nodiscard]] bool isSpecializedSprite() const override;
    [[nodiscard]] uint8_t getPixelAlpha(int x, int y) const;
    [[nodiscard]] SDL_Rect getSDLRect() const override;
    [[nodiscard]] Vector2<double> getCoordinates() const;
    [[nodiscard]] SDL_Surface* getSDLSurface() const;
    [[nodiscard]] SDL_Texture* getCachedTexture() const override;
protected:
    static SDL_Surface* loadSurface(const char* path);

    SDL_Rect m_rect{};
    Vector2<double> m_coordinates{};
    SDL_Surface* m_surface{};
    SDL_Texture* m_texture;
};

template <typename T, std::enable_if_t<IsAllowedCollisionMethod<T>::value, bool>>
class Sprite : public SpriteBase {};

template <>
class Sprite<NoCollision> : public SpriteBase
{
public:
    Sprite(const char* path);
    Sprite(SDL_Rect rect, SDL_Color color);
};

template <>
class Sprite<RectangularCollision> : public SpriteBase
{
public:
    Sprite(const char* path, const Observer* observer);
    Sprite(SDL_Rect rect, SDL_Color color, const Observer* observer);
    [[nodiscard]] const Observer* getCollisionObserver() const;
    [[nodiscard]] bool isCollisionSprite() const override;

    /**
	 * \brief Whether a RectangularCollision Sprite will collide with anything
	 * \param other other sprite
	 * \param potentialPosition potential position vector of this sprite
	 * \return bool
	 */
    [[nodiscard]] bool hasCollisionWith(const Entity& other, Vector2<double> potentialPosition) const override;

protected:
    /**
	 * \brief Checks collisions between RectangularCollision and RectangularCollision
	 * \param other other sprite
	 * \param potentialPosition potential position vector of the other sprite
	 * \return bool
	 */
    [[nodiscard]] bool hasCollisionWithImpl(const Sprite<RectangularCollision>& other, Vector2<double> potentialPosition) const override;

    /**
	 * \brief Checks collisions between RectangularCollision and PolygonCollision
	 * \param other other sprite
	 * \param potentialPosition potential position vector of the other sprite
	 * \return bool
	 */
    [[nodiscard]] bool hasCollisionWithImpl(const Sprite<PolygonCollision>& other, Vector2<double> potentialPosition) const override;

    const Observer* m_observer;
};

template<>
class Sprite<PolygonCollision> : public SpriteBase
{
public:
    Sprite(const char* path, const Observer* observer);

	//void printSlices();
    [[nodiscard]] std::vector<SDL_Rect> slice(int sliceThickness) const;
    [[nodiscard]] const Observer* getCollisionObserver() const;
    [[nodiscard]] bool isCollisionSprite() const override;

    // Test function
    [[nodiscard]] std::vector<std::shared_ptr<Sprite<NoCollision>>> processSlices();

    /**
	 * \brief
	 * \param other other sprite
	 * \param potentialPosition potential position vector of this sprite
	 * \return bool
	 */
    [[nodiscard]] bool hasCollisionWith(const Entity& other, Vector2<double> potentialPosition) const override;

protected:
    /**
	 * \brief Checks collisions between PolygonCollision and PolygonCollision
	 * \param other other sprite
	 * \param potentialPosition potential position vector of the other sprite
	 * \return bool
	 */
    [[nodiscard]] bool hasCollisionWithImpl(const Sprite<PolygonCollision>& other, Vector2<double> potentialPosition) const override;

    /**
	 * \brief Checks collisions between PolygonCollision and RectangularCollision
	 * \param other other sprite
	 * \param potentialPosition potential position vector of the other sprite
	 * \return bool
	 */
	[[nodiscard]] bool hasCollisionWithImpl(const Sprite<RectangularCollision>& other, Vector2<double> potentialPosition) const override;

    const Observer* m_observer;
    std::vector<SDL_Rect> m_slices;
};