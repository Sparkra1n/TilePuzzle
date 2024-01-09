#pragma once

#include <SDL.h>
#include <functional>
#include "Entity.h"

class Observer
{
public:
    Observer() = default;
    virtual ~Observer() = default;
    [[nodiscard]] virtual bool canMoveTo(const Entity& entity) const { return true; }
};
