#pragma once

#include "Vector2.h"
#include "Sprite.h"
#include <unordered_set>

class Player : public Sprite
{
public:
    Player(const char* path, const Observer* observer = nullptr, const double speed = 1.0)
        : Sprite(path, observer) ,m_speed(speed) {}
    ~Player() override = default;

    void walkTo(const Vector2<int> coordinates) { m_targetPosition = coordinates; }
    void handleEvent(const SDL_Event& event);
    void update(double deltaTime) override;
    void setSpeed(const double speed) { m_speed = speed; }
    [[nodiscard]] double getSpeed() const { return m_speed; }
    [[nodiscard]] bool isDummy() const override { return false; }

private:
    Vector2<int> m_targetPosition{};
    std::unordered_set<SDL_Keycode> m_pressedKeys;
    double m_speed = 1.0;
};

