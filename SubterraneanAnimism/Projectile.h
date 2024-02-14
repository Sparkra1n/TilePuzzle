#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include "Game.h"
#include "Sprite.h"

class Projectile : public Entity
{
public:
    Projectile(const char* path,
		int damage, Vector2<double> velocity = { 0, 0 },
        const Observer* observer = nullptr);

    ~Projectile() override = default;
    void update(double deltaTime) override;
    void setDamage(int damage);
    void setVelocity(Vector2<double> velocity);
	void cacheTexture(SDL_Renderer* renderer) override;
    [[nodiscard]] int getDamage() const;
    [[nodiscard]] Vector2<double> getVelocity() const;
    [[nodiscard]] bool hasCollisionWith(const Entity& other, Vector2<double> potentialPosition) const override;
    [[nodiscard]] SDL_Texture* getCachedTexture() const override;
    [[nodiscard]] SDL_Rect getSDLRect() const override;

protected:
    Sprite<RectangularCollision> m_sprite;
    int m_damage{};
    Vector2<double> m_velocity{};
};
