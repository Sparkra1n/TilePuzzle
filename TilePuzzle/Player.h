#pragma once

#include "Vector2.h"
#include "Sprite.h"
#include <unordered_set>

class Player : public GameObject
{
public:
    Player(const char* path, SDL_Renderer* cacheRenderer, const Observer* observer = nullptr, const double speed = 1)
        : GameObject(path, cacheRenderer, speed, observer) {}
    void handleEvent(const SDL_Event& event) override;
protected:
    std::unordered_set<SDL_Keycode> m_pressedKeys;
};

