/*
 * Toy.hpp
 *
 *  Created on: 2012/01/14
 *      Author: ely
 */

#ifndef TOY_HPP_
#define TOY_HPP_

/*#include "../entity/EntitySkeleton.hpp"
#include "../entity/btShapes.hpp"
#include "../entity/IrrMeshs.hpp"
#include "../entity/PresetEntityBases.hpp"
*/
#include "../../AkashaSystem/AkashaSystem.hpp"
#include <boost/ptr_container/ptr_vector.hpp>


namespace akasha
{
namespace toy
{

class BallToy
{
	/*typedef entity::EntityBase<entity::SphereShapeBase,
	  entity::SphereMesh> base_type;

	  typedef entity::PhysicEntity entity_type;
	  entity_type m_entity;*/
public:
	/*entity_type&
	  getEntity()
	  {
	  return m_entity;
	  }*/
};

class BoxToy
{
	/*typedef entity::PhysicEntity entity_type;
	  entity_type m_entity;
	  public:
	  entity_type&
	  getEntity()
	  {
	  return m_entity;
	  }*/

};

class FractureToy
{
	/*typedef entity::FractureEntity entity_type;
	  entity_type m_entity;*/
public:
	/*entity_type&
	  getEntity()
	  {
	  return m_entity;
	  }*/
};

class ToyManager : boost::noncopyable
{
	/*entity::EntityService& m_service;*/
	boost::ptr_vector<BallToy> m_balls;
	boost::ptr_vector<BoxToy> m_boxs;
	boost::ptr_vector<FractureToy> m_fracture;

	short int m_collisionGroup;
	short int m_collisionMask;
public:
	ToyManager(/*entity::EntityService& s*/) :
		/*m_service(s),*/ m_collisionGroup(COL_TYPE_TOY), m_collisionMask(
				COL_TYPE_LAND | COL_TYPE_MODEL | COL_TYPE_TOY)
	{

	}
	/*
		void
		addBallToy(const entity::EntityConstructionInfo& inInfo,
		const btScalar size)
		{
		entity::EntityConstructionInfo info(inInfo);

		akasha::system::AkashaSystem::Instance().getLogger()(
		akasha::system::log::ApplicationInfo { "BallToy Add." });

	//コリジョングループ上書き
	info.m_collisonGroup = m_collisionGroup;
	info.m_collisonMask = m_collisionMask;

	m_balls.push_back(new BallToy());

	entity::SphereBase base(size);

	BallToy& ball = m_balls.back();
	ball.getEntity().constract(m_service, base, info);
	ball.getEntity().attach(m_service);
	}

	void
	addBoxToy(const entity::EntityConstructionInfo& inInfo,
	const btVector3 size)
	{
	entity::EntityConstructionInfo info(inInfo);

	akasha::system::AkashaSystem::Instance().getLogger()(
	akasha::system::log::ApplicationInfo { "BoxToy Add." });

	//コリジョングループ上書き
	info.m_collisonGroup = m_collisionGroup;
	info.m_collisonMask = m_collisionMask;

	m_boxs.push_back(new BoxToy());

	entity::BoxBase base(size);

	BoxToy& toy = m_boxs.back();
	toy.getEntity().constract(m_service, base, info);
	toy.getEntity().attach(m_service);
	}

	void
	addFractureToy(const entity::EntityConstructionInfo& inInfo,
	const btVector3 size)
	{
	entity::EntityConstructionInfo info(inInfo);

	akasha::system::AkashaSystem::Instance().getLogger()(
	akasha::system::log::ApplicationInfo {
	"FractureToy Add." });

	//コリジョングループ上書き
	info.m_collisonGroup = m_collisionGroup;
	info.m_collisonMask = m_collisionMask;// & ~COL_TYPE_TOY;
	//info.m_collisonGroup = 0;
	//info.m_collisonMask = 0;



	entity::BoxBase base(size);

	{
	auto& world = BulletManager::Instance().getWorld();

	btVector3& v = info.m_startWorldTransform.getOrigin();

	m_fracture.push_back(new FractureToy());
	FractureToy& toy = m_fracture.back();
	toy.getEntity().constract(m_service, base, info);
	toy.getEntity().attach(m_service);

	v.setX( v.getX()+size.getX()*2 );

	m_fracture.push_back(new FractureToy());
	FractureToy& toy1 = m_fracture.back();
	toy1.getEntity().constract(m_service, base, info);
	toy1.getEntity().attach(m_service);


	v.setX( v.getX()+ size.getX()*2);
	world.addWeld((toy.getEntity().getBody()), (toy1.getEntity().getBody()));

	m_fracture.push_back(new FractureToy());
	FractureToy& toy2 = m_fracture.back();
	toy2.getEntity().constract(m_service, base, info);
	toy2.getEntity().attach(m_service);

	//m_service.getDynamicsWorldPtr()->addRigidBody(NULL);

}
}
*/

};

}
}

#endif /* TOY_HPP_ */
