#ifndef AKASHA_WORLD_BULLET_FLUIDDYNAMICS_FLUIDBODYCOLLISIONALGORITHM_HPP_
#define AKASHA_WORLD_BULLET_FLUIDDYNAMICS_FLUIDBODYCOLLISIONALGORITHM_HPP_

#include <BulletCollision/CollisionDispatch/btActivatingCollisionAlgorithm.h>
#include <BulletCollision/BroadphaseCollision/btCollisionAlgorithm.h>
#include <BulletCollision/CollisionDispatch/btCollisionCreateFunc.h>
#include "BulletCollision/CollisionDispatch/btCollisionDispatcher.h"

#include <BulletCollision/CollisionDispatch/btConvexConcaveCollisionAlgorithm.h>

#include <iostream>
class btFluidBodyCollisionAlgorithm : public btActivatingCollisionAlgorithm
{
		btCollisionObjectWrapper const* m_fluid;
		btCollisionObjectWrapper const* m_obj;

		btConvexTriangleCallback m_convexTrianglecallback;
		btPersistentManifold* m_manifold;

	public:

		btFluidBodyCollisionAlgorithm() = delete;

		btFluidBodyCollisionAlgorithm(
				btCollisionAlgorithmConstructionInfo const& ci,
				btCollisionObjectWrapper const* fluidBody,
				btCollisionObjectWrapper const* buoyantBody
				);

	virtual void processCollision (
			const btCollisionObjectWrapper* body0Wrap,
			const btCollisionObjectWrapper* body1Wrap,
			const btDispatcherInfo& dispatchInfo,
			btManifoldResult* resultOut) override;

	virtual btScalar calculateTimeOfImpact(
			btCollisionObject* body0,
			btCollisionObject* body1,
			const btDispatcherInfo& dispatchInfo,
			btManifoldResult* resultOut) override;

	virtual void	getAllContactManifolds(btManifoldArray&	manifoldArray) override;

	struct CreateFunc :public btCollisionAlgorithmCreateFunc
	{

		virtual btCollisionAlgorithm*
			CreateCollisionAlgorithm(
					btCollisionAlgorithmConstructionInfo& ci,
					btCollisionObjectWrapper const* body0,
					btCollisionObjectWrapper const* body1
					) override
			{
				using allocType = btFluidBodyCollisionAlgorithm;
				void* mem =
					ci.m_dispatcher1->allocateCollisionAlgorithm(
							sizeof(allocType)
							);
				if (!m_swapped)
					return new(mem) allocType(ci,body0, body1);
				else
					return new(mem) allocType(ci,body1, body0);
			}

	};
};
#endif /* end of include guard */
