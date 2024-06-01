#pragma once
//
//#include <SDL.h>
//#include <SDL_image.h>
//#include "Game.h"
//#include "Sprite.h"
//
//class TileObject : public Entity
//{
//public:
//    TileObject(const char* path,
//		int damage, Vector2<double> velocity = { 0, 0 },
//        const Observer* observer = nullptr);
//
//    ~TileObject() override = default;
//    void update(double deltaTime) override;
//	void cacheTexture(SDL_Renderer* renderer) override;
//    [[nodiscard]] Vector2<double> getVelocity() const;
//    [[nodiscard]] bool hasCollisionWith(const Entity& other, Vector2<double> potentialPosition) const override;
//    [[nodiscard]] SDL_Texture* getCachedTexture() const override;
//    [[nodiscard]] SDL_Rect getSdlRect() const override;
//
//protected:
//    Sprite<RectangularCollision> m_sprite;
//    Vector2<double> m_velocity{};
//};
