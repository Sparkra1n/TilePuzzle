#pragma once

#include "Vector2.h"
#include "Sprite.h"
#include <unordered_set>

class Player: public Entity
{
public:
    Player(const char* path, const Observer* observer = nullptr, const double speed = 1.0)
        : m_sprite(path, observer), m_speed(speed) {}
    ~Player() override = default;

    void handleEvent(const SDL_Event& event);
    void update(double deltaTime) override;
    void setSpeed(double speed);
    void setCoordinates(Vector2<double> coordinates);
    void cacheTexture(SDL_Renderer* renderer) override;
    void clearRenderFlag() override;
    void setRenderFlag() override;

    //TODO: refactor player functions such as this one
    [[nodiscard]] Sprite<PolygonCollision>* getSprite();

    [[nodiscard]] SDL_Rect getSDLRect() const override;

    [[nodiscard]] SDL_Surface* getSDLSurface() const;

    [[nodiscard]] Vector2<double> getCoordinates() const;

    [[nodiscard]] double getSpeed() const;

    [[nodiscard]] bool isSpecializedSprite() const override;

    [[nodiscard]] bool hasCollisionWith(const Entity& other, Vector2<double> potentialPosition) const override;

    [[nodiscard]] SDL_Texture* getCachedTexture() const override;

    [[nodiscard]] bool getRenderFlag() const override;

private:
    std::unordered_set<SDL_Keycode> m_pressedKeys;
    Sprite<PolygonCollision> m_sprite;
    double m_speed = 1.0;
};

