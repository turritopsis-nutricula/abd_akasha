#ifndef AKASHA_WORLD_BULLET_PHYSICS_WORLD_HPP_
#define AKASHA_WORLD_BULLET_PHYSICS_WORLD_HPP_

#include "ObjectOption.hpp"
#include "btFractureBody.hpp"
#include "fluidDynamics/fluid/btHfFluid.h"

#include <BulletCollision/BroadphaseCollision/btBroadphaseInterface.h>
#include <BulletCollision/CollisionDispatch/btCollisionObject.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
#include <boost/signals2.hpp>
namespace akasha
{
namespace bullet
{
inline void AddCustomMaterialCallback(btCollisionObject* o)
{
	o->setCollisionFlags(
			o->getCollisionFlags() |
			btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

}
inline void RemoveCustomMaterialCallback(btCollisionObject* o)
{
	o->setCollisionFlags(
			o->getCollisionFlags() &
			~btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
}

//fwd
struct PhysicsWorldSetting;

class PhysicsWorld : boost::noncopyable
{

public:
	struct WorldInfo
	{
		btDispatcher* dispatcher_;
	};
	using tickCallbackType =
		boost::signals2::signal<void (WorldInfo const&, btScalar)>;
private:
	boost::shared_ptr<PhysicsWorldSetting> m_setting;
	boost::shared_ptr<btOverlappingPairCallback> m_ghostPairCallback;
	tickCallbackType m_preTickCallback;
	tickCallbackType m_tickCallback;

	using changeConstraintFuncType =
		boost::function<void (boost::unordered_map<
				btTypedConstraint*, boost::shared_ptr<btTypedConstraint>>&)>;
public:

	PhysicsWorld();
	~PhysicsWorld();

	tickCallbackType& getTickCallbackSignal(bool isPreTick=false)
	{
		if (isPreTick)
			return m_preTickCallback;
		else
			return m_tickCallback;
	}

	std::pair<
		boost::shared_ptr<btFractureBody>,
		boost::shared_ptr<ObjectOption>>
			createFractureBody(
					btRigidBody::btRigidBodyConstructionInfo const& info);

	template<typename FuncType>
		void changeConstraintFunction(FuncType f)
		{
			changeConstraintFunction_(changeConstraintFuncType(std::move(f)));
		}

	void changeConstraintFunction_(changeConstraintFuncType f);

	void setNearCallback(btNearCallback nearCallback);
	void setInternalGhostPairCallback(boost::shared_ptr<btOverlappingPairCallback> const& c);

	// Intenal Callback
	static void preTickCallback(btDynamicsWorld* w, btScalar s)
	{
		static_cast<PhysicsWorld*>(w->getWorldUserInfo())->OnPreTick(s);
	}
	static void tickCallback(btDynamicsWorld* w, btScalar s)
	{
		static_cast<PhysicsWorld*>(w->getWorldUserInfo())->OnTick(s);
	}
	void OnPreTick(btScalar timeStep);
	void OnTick(btScalar timeStep);

	//////////////////////////////////////////////////////////////
	// bullet functions
	int stepSimulation( btScalar timeStep,int maxSubSteps=1, btScalar fixedTimeStep=btScalar(1.)/btScalar(60.));


	void addConstraint(boost::shared_ptr<btTypedConstraint> const&,
			bool disableCollisionBetweenLinkedBodies=false);
	void addConstraint_(boost::shared_ptr<btTypedConstraint> const&,
			bool disableCollisionBetweenLinkedBodies,
			boost::function<boost::shared_ptr<btTypedConstraint> (btTypedConstraint*, btTypedConstraint*)> );
	template<typename Handler>
		void addConstraint(
				boost::shared_ptr<btTypedConstraint> const& c,
				bool disableCollisionBetweenLinkedBodies,
				Handler handle
				)
		{
			addConstraint_(
					c,
					disableCollisionBetweenLinkedBodies,
					boost::function<boost::shared_ptr<btTypedConstraint> (btTypedConstraint*,btTypedConstraint*)>( std::move(handle))
					);
		}
	void removeConstraint(
			boost::shared_ptr<btTypedConstraint> const&);
	void removeConstraint( btTypedConstraint*);


	void addCollisionObject(
			btCollisionObject*,
			short int filterGroup=btBroadphaseProxy::StaticFilter,
			short int filtermask = btBroadphaseProxy::AllFilter^btBroadphaseProxy::StaticFilter
			);
	void removeCollisionObject(btCollisionObject*);

	// 				void addRigidBody(boost::shared_ptr<btFractureBody> const&, short int, short int);
	void addRigidBody(btRigidBody*);
	void addRigidBody(btRigidBody*, short int, short int);
	void removeRigidBody(btRigidBody*);

	void debugDrawConstraint(btTypedConstraint*);
	void debugDrawWorld();
	void setDebugDrawer(btIDebugDraw*);

	void rayTest(
			const btVector3& rayFromWorld,
			const btVector3& rayToWorld,
			btCollisionWorld::RayResultCallback& resultCallback) const;
	void convexSweepTest (
			const btConvexShape* castShape,
			const btTransform& from,
			const btTransform& to,
			btCollisionWorld::ConvexResultCallback& resultCallback,
			btScalar allowedCcdPenetration = btScalar(0.)) const;

	void addHfFluid(
			btHfFluid* fluid,
			short int group=btBroadphaseProxy::DefaultFilter,
			short int mask=btBroadphaseProxy::AllFilter);
	void removeHfFluid(btHfFluid* fluid);

	btCollisionObjectArray& getCollisionObjectArray();
	btCollisionObjectArray const& getCollisionObjectArray() const;
	btBroadphaseInterface* getBroadphase();
	btBroadphaseInterface const* getBroadphase() const;
	btDispatcher* getDispatcher();
	btDispatcher const* getDispatcher() const;

	btContactSolverInfo& getSolverInfo();


};
} // namespace bullet
} // namespace akasha
#endif /* end of include guard */
