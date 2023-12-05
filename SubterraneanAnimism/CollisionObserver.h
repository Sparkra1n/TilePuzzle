#pragma once

class CollisionSprite;

class CollisionObserver
{
public:
    CollisionObserver() {}
    [[nodiscard]] virtual bool canMoveTo(CollisionSprite& collisionSprite, double potentialX, double potentialY) const;
    virtual ~CollisionObserver() = default;
};

