//#include "TileObject.h"
//
//TileObject::TileObject(const char* path, 
//                       const int damage,
//                       Vector2<double> velocity, 
//                       const Observer* observer)
//		: m_sprite(path, observer),
//          m_velocity(velocity)
//{}
//
//void TileObject::update(const double deltaTime)
//{
//	const Vector2<double> newPosition = m_sprite.getCoordinates() + m_velocity * deltaTime;
//	m_sprite.setCoordinates(newPosition);
//}
//
//bool TileObject::hasCollisionWith(const Entity& other, Vector2<double> potentialPosition) const
//{
//	return m_sprite.hasCollisionWith(other, potentialPosition);
//}
//
//Vector2<double> TileObject::getVelocity() const
//{
//	return m_velocity;
//}
//
//void TileObject::cacheTexture(SDL_Renderer* renderer)
//{
//	m_sprite.cacheTexture(renderer);
//}
//
//SDL_Texture* TileObject::getCachedTexture() const
//{
//	return m_sprite.getCachedTexture();
//}
//SDL_Rect TileObject::getSdlRect() const
//{
//	return m_sprite.getSdlRect();
//}
//
