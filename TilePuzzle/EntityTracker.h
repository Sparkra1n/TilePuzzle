#pragma once

#include "Sprite.h"
#include <memory>

class EntityTracker
{
public:
	EntityTracker() = default;
	~EntityTracker() = default;

	void setFocused(const std::shared_ptr<Entity>& entity)
	{
		// Skip if repeat
		if (entity == m_current)
			return;

		if (m_previous)
		{
			m_previous->blur();
			m_previous = m_current;
			m_previous->blur();
		}
		m_previous = m_current;
		entity->focus();
		m_current = entity;
	}

	[[nodiscard]] std::shared_ptr<Entity> getFocused() const { return m_current; }
	[[nodiscard]] std::shared_ptr<Entity> getBlurred() const { return m_previous; }

protected:
	std::shared_ptr<Entity> m_current;
	std::shared_ptr<Entity> m_previous;
};