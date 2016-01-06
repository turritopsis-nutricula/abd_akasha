#include "PhysicsWorld.hpp"

#include "btFractureDynamicsWorld.hpp"
#include "HfFluidDynamics.hpp"

#include "fluidDynamics/btHfFluidRigidCollisionConfiguration.h"

#include <btBulletDynamicsCommon.h>

#include <boost/make_shared.hpp>
#include <boost/scoped_ptr.hpp>

using akasha::bullet::PhysicsWorld;
using akasha::bullet::PhysicsWorldSetting;



// Setting Type Define
namespace akasha
{
namespace bullet
{
namespace detail
{
struct Setting
{
	using worldType =
		btFractureDynamicsWorld<
		HfFluidDynamicsImpl<
		btDiscreteDynamicsWorld,void>,void>;
	using solverType = btSequentialImpulseConstraintSolver;
	using broadphaseType = btAxisSweep3;
	using dispatcherType = btCollisionDispatcher;
	using configureType = btHfFluidRigidCollisionConfiguration;

	typedef worldType type;

	boost::scoped_ptr< configureType > collisionConfiguration_;
	boost::scoped_ptr< dispatcherType > dispatcher_;
	boost::scoped_ptr< broadphaseType > broadphase_;
	boost::scoped_ptr< solverType > solver_;
	boost::scoped_ptr< worldType > world_;

};
}
struct PhysicsWorldSetting
{
	typedef detail::Setting type;
	type set_;

	typename type::worldType&
		getWorld()
		{
			return *(set_.world_);
		}

};
} // namespace bullet
} // namespace akasha

//////////////////////////////////////////////////////////////
//BulletObject Deleter
// template<typename WorldType>
struct CollisionObjectDeleter
{
	using worldType = akasha::bullet::detail::Setting::worldType;
	worldType& world_;
	boost::shared_ptr<akasha::bullet::ObjectOption> authoizer_;
	CollisionObjectDeleter() = delete;
	CollisionObjectDeleter(
			worldType& w,
			boost::shared_ptr<akasha::bullet::ObjectOption> a) :
		world_(w), authoizer_(a){}

	void operator()( btFractureBody* b ) const
	{
		world_.removeRigidBody( b );
#ifndef NDEBUG
		std::cout << "delete collision object: " << b << "\n";
#endif
		delete b;
	}
};

void SettingSetup(akasha::bullet::detail::Setting&);

PhysicsWorld::PhysicsWorld() :
	m_ghostPairCallback(nullptr)
{
	m_setting.reset(
			new akasha::bullet::PhysicsWorldSetting);

	SettingSetup(m_setting->set_);

	m_setting->getWorld().setInternalTickCallback(
			PhysicsWorld::preTickCallback,
			static_cast<void*>(this),
			true
			);
	m_setting->getWorld().setInternalTickCallback(
			PhysicsWorld::tickCallback,
			static_cast<void*>(this),
			false
			);


}
PhysicsWorld::~PhysicsWorld()
{
	// 	SettingRelease(m_setting->set_);
}

void PhysicsWorld::OnPreTick(btScalar timeStep)
{
	m_preTickCallback(WorldInfo{m_setting->getWorld().getDispatcher()}, timeStep);
}
void PhysicsWorld::OnTick(btScalar timeStep)
{
	m_tickCallback(WorldInfo{m_setting->getWorld().getDispatcher()}, timeStep);
}

void PhysicsWorld::setNearCallback(btNearCallback nearCallback)
{
	static_cast<btCollisionDispatcher*>(m_setting->getWorld().getDispatcher())->
		setNearCallback( nearCallback );
}
void PhysicsWorld::setInternalGhostPairCallback(boost::shared_ptr<btOverlappingPairCallback> const& c)
{
	m_ghostPairCallback = c;
	m_setting->getWorld().getBroadphase()->getOverlappingPairCache()->
		setInternalGhostPairCallback(c.get());
}

std::pair<
boost::shared_ptr<btFractureBody>,
	boost::shared_ptr<akasha::bullet::ObjectOption>>
PhysicsWorld::createFractureBody(
		btRigidBody::btRigidBodyConstructionInfo const& info)
{

	auto* b = new btFractureBody(info);
	auto autoizer =
		boost::make_shared<akasha::bullet::ObjectOption>();
	autoizer->m_impluse.setZero();
	autoizer->m_impluse.setZero();

	b->setUserPointer( autoizer.get());
	return std::make_pair(
			boost::shared_ptr<btFractureBody>(
				b,
				CollisionObjectDeleter(m_setting->getWorld(), autoizer)),
			autoizer);
}

void PhysicsWorld::changeConstraintFunction_(changeConstraintFuncType f)
{
	m_setting->getWorld().changeConstraintFunction(std::move(f));
}
int PhysicsWorld::stepSimulation( btScalar timeStep,int maxSubSteps/*=1*/, btScalar fixedTimeStep /*btScalar(1.)/btScalar(60.)*/ )
{
	return m_setting->getWorld().stepSimulation(timeStep, maxSubSteps, fixedTimeStep);
}

btBroadphaseInterface* PhysicsWorld::getBroadphase()
{
	return m_setting->getWorld().getBroadphase();
}
btBroadphaseInterface const* PhysicsWorld::getBroadphase() const
{
	return m_setting->getWorld().getBroadphase();
}

void PhysicsWorld::addConstraint(
		boost::shared_ptr<btTypedConstraint> const& c, bool disableCollisionBetweenLinkedBodies)
{
	m_setting->getWorld().addConstraint(c, disableCollisionBetweenLinkedBodies);
}
void PhysicsWorld::addConstraint_(boost::shared_ptr<btTypedConstraint> const& c,
		bool disableCollisionBetweenLinkedBodies,
		boost::function<boost::shared_ptr<btTypedConstraint> (btTypedConstraint*, btTypedConstraint*)> f)
{
	m_setting->getWorld().addConstraint(
			c,
			disableCollisionBetweenLinkedBodies,
			std::move(f)
			);
}

void PhysicsWorld::removeConstraint(boost::shared_ptr<btTypedConstraint> const& c)
{
	m_setting->getWorld().removeConstraint(c);
}
void PhysicsWorld::removeConstraint(btTypedConstraint* c)
{
	m_setting->getWorld().removeConstraint(c);
}

void PhysicsWorld::addCollisionObject(
		btCollisionObject* o,
		short int filterGroup,
		short int filtermask
		)
{
	m_setting->getWorld().addCollisionObject(o, filterGroup,filtermask);
}
void PhysicsWorld::removeCollisionObject(btCollisionObject* o)
{
	m_setting->getWorld().removeCollisionObject(o);
}


void PhysicsWorld::addRigidBody(btRigidBody* b)
{
	m_setting->getWorld().addRigidBody(b);
}
void PhysicsWorld::addRigidBody(btRigidBody* body, short int group, short int mask)
{
	m_setting->getWorld().addRigidBody(body, group, mask);
}
void PhysicsWorld::removeRigidBody(btRigidBody* body)
{
	m_setting->getWorld().removeRigidBody(body);
}

void PhysicsWorld::debugDrawConstraint(btTypedConstraint* c)
{
	m_setting->getWorld().debugDrawConstraint(c);
}
void PhysicsWorld::debugDrawWorld()
{
	m_setting->getWorld().debugDrawWorld();
}
void PhysicsWorld::setDebugDrawer(btIDebugDraw* d)
{
	m_setting->getWorld().setDebugDrawer(d);
}
void PhysicsWorld::rayTest(
		const btVector3& rayFromWorld,
		const btVector3& rayToWorld,
		btCollisionWorld::RayResultCallback& resultCallback) const
{
	m_setting->getWorld().rayTest(rayFromWorld, rayToWorld, resultCallback);
}
void PhysicsWorld::convexSweepTest (
		const btConvexShape* castShape,
		const btTransform& from,
		const btTransform& to,
		btCollisionWorld::ConvexResultCallback& resultCallback,
		btScalar allowedCcdPenetration ) const
{
	m_setting->getWorld().convexSweepTest(castShape, from, to, resultCallback, allowedCcdPenetration);
}

void PhysicsWorld::addHfFluid(
		btHfFluid* fluid,
		short int group,
		short int mask)
{
	m_setting->getWorld().addHfFluid(fluid, group, mask);
}
void PhysicsWorld::removeHfFluid(btHfFluid* fluid)
{
	m_setting->getWorld().removeHfFluid(fluid);
}

btCollisionObjectArray& PhysicsWorld::getCollisionObjectArray()
{
	return m_setting->getWorld().getCollisionObjectArray();
}
btCollisionObjectArray const& PhysicsWorld::getCollisionObjectArray() const
{
	return m_setting->getWorld().getCollisionObjectArray();
}
btDispatcher* PhysicsWorld::getDispatcher()
{
	return m_setting->getWorld().getDispatcher();
}
btDispatcher const* PhysicsWorld::getDispatcher() const
{
	return m_setting->getWorld().getDispatcher();
}
btContactSolverInfo& PhysicsWorld::getSolverInfo()
{
	return m_setting->getWorld().getSolverInfo();
}
//////////////////////////////////////////////////////////////////////////////////
void SettingSetup(akasha::bullet::detail::Setting& s)
{

	using setting = akasha::bullet::detail::Setting;
	//Bullet Constract
	// デフォルトの衝突設定とディスパッチャの作成
	s.collisionConfiguration_.reset(
			new typename setting::configureType());


	//btCollisionDispatcher* dispatcher;
	s.dispatcher_.reset(new typename setting::dispatcherType(s.collisionConfiguration_.get()));

	//Build the broadphase
	const btScalar worldBottom = btScalar(-30 * btPow(10,3));
	const btScalar worldTop = btScalar(30 * btPow(10,3));
	const btScalar worldSize = btScalar(100 * btPow(10,3));
	btVector3 worldAabbMin(-worldSize,worldBottom,-worldSize);
	btVector3 worldAabbMax(worldSize,worldTop,worldSize);
	s.broadphase_.reset(
			new typename setting::broadphaseType(
				worldAabbMin,
				worldAabbMax,
				8182 // maxProxies
				)
			);

	//衝突解決ソルバ
	s.solver_.reset(
			new typename setting::solverType());


	// 離散動的世界の作成
	s.world_.reset( new typename setting::worldType(
				s.dispatcher_.get(),
				s.broadphase_.get(),
				s.solver_.get(),
				s.collisionConfiguration_.get() ));

	// 重力の設定
	s.world_->setGravity(btVector3(0,-9.806,0));

}
