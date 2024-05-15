#include "Projectile.h"

Projectile::Projectile(const char* path, 
	const int damage,
	Vector2<double> velocity, 
	const Observer* observer)
		: m_sprite(path, observer),
		  m_damage(damage),
          m_velocity(velocity)
{}

void Projectile::update(const double deltaTime)
{
	const Vector2<double> newPosition = m_sprite.getCoordinates() + m_velocity * deltaTime;
	m_sprite.setCoordinates(newPosition);
}

bool Projectile::hasCollisionWith(const Entity& other, Vector2<double> potentialPosition) const
{
	return m_sprite.hasCollisionWith(other, potentialPosition);
}

void Projectile::setDamage(const int damage)
{
	m_damage = damage;
}

void Projectile::setVelocity(const Vector2<double> velocity)
{
	m_velocity = velocity;
}

int Projectile::getDamage() const
{
	return m_damage;
}

Vector2<double> Projectile::getVelocity() const
{
	return m_velocity;
}

void Projectile::cacheTexture(SDL_Renderer* renderer)
{
	m_sprite.cacheTexture(renderer);
}

SDL_Texture* Projectile::getCachedTexture() const
{
	return m_sprite.getCachedTexture();
}
SDL_Rect Projectile::getSdlRect() const
{
	return m_sprite.getSdlRect();
}

