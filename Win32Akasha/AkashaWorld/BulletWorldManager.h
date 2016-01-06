#ifndef BULLETWORLD_MANAGER__H
#define BULLETWORLD_MANAGER__H


#include "bullet/PhysicsWorld.hpp"
#include <Riceball/singleton/SingletonHolder.hpp>
namespace akasha
{

typedef
	 singleton::SingletonHolder<
	 bullet::PhysicsWorld> BulletManager;

}
#endif //BULLETWORLD_MANAGER__H
