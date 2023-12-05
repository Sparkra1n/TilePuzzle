#pragma once

class CollisionSprite;

class CollisionObserver
{
public:
    CollisionObserver() = default;
    [[nodiscard]] virtual bool canMoveTo(const CollisionSprite& collisionSprite, double potentialX, double potentialY) const;
    virtual ~CollisionObserver() = default;
};

