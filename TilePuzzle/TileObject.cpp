#include "TileObject.h"

void TileObject::update(double deltaTime)
{
    //TODO: Remove duplicate code below with the Player class
    if (m_targetPosition.x != -1)
    {
        if (std::abs(m_targetPosition.x - getCoordinates().x) > 1)
        {
            Vector2<double> coordinates = getCoordinates();
            const int sign = coordinates.x < m_targetPosition.x ? 1 : -1;
            coordinates.x += m_speed * deltaTime * sign;
            setCoordinates(coordinates);
        }
        else
        {
            setXCoordinate(m_targetPosition.x);
            m_targetPosition.x = -1;
        }
    }
    if (m_targetPosition.y != -1 && m_targetPosition.x == -1)
    {
        if (std::abs(m_targetPosition.y - getCoordinates().y) > 1)
        {
            Vector2<double> coordinates = getCoordinates();
            const int sign = coordinates.y < m_targetPosition.y ? 1 : -1;
            coordinates.y += m_speed * deltaTime * sign;
            setCoordinates(coordinates);
        }
        else
        {
            setYCoordinate(m_targetPosition.y);
            m_targetPosition.y = -1;
        }
    }
}

void TileObject::handleEvent(const SDL_Event& event)
{
    
}


