//#pragma once
//
//#include "CollisionSprite.h"
//#include <SDL.h>
//#include <SDL_image.h>
//
//class Projectile : public CollisionSprite
//{
//public:
//	Projectile(const char* path, const int damage, const Vector2 velocity, const SDL_Color& color)
//		: CollisionSprite(path), m_damage(damage), m_velocity(velocity), m_color(color) {}
//
//	~Projectile() override = default;
//
//	void update(double deltaTime) override;
//
//	/**
//	 * @brief Set the damage value of the projectile.
//	 * @param damage
//	 */
//	void setDamage(int damage) { m_damage = damage; }
//
//	/**
//	 * @brief Get the damage value of the projectile.
//	 * @return
//	 */
//	[[nodiscard]] int getDamage() const { return m_damage; }
//
//	/**
//	 * @brief Set the velocity of the projectile.
//	 * @param velocity
//	 */
//	void setVelocity(Vector2 velocity) { m_velocity = velocity; }
//
//	/**
//	 * @brief Get the velocity of the projectile.
//	 * @return
//	 */
//	[[nodiscard]] Vector2 getVelocity() const { return m_velocity; }
//
//	/**
//	 * @brief Set the color of the projectile.
//	 * @param color
//	 */
//	void setColor(const SDL_Color& color) { m_color = color; }
//
//	/**
//	 * @brief Get the color of the projectile.
//	 * @return
//	 */
//	[[nodiscard]] SDL_Color getColor() const { return m_color; }
//
//	void handleEvent(const SDL_Event& event) override {}
//
//	bool hasCollisionWith(const Sprite& other) const override;
//
//protected:
//	int m_damage{};
//	Vector2 m_velocity{};
//	SDL_Color m_color{};
//
//	/**
//	 * @brief Moves the projectile based on its velocity and direction.
//	 * @param deltaTime
//	 */
//	void move(double deltaTime);
//};
