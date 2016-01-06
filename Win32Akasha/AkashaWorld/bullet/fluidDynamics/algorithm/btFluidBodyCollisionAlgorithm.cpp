#include "btFluidBodyCollisionAlgorithm.hpp"
#include "BulletCollision/CollisionShapes/btCollisionShape.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"

#include "../buoyant/BuoyantAuthoizer.hpp"
#include "../fluid/btHfFluid.h"
#include "../../ObjectOption.hpp"

using Algorithm = btFluidBodyCollisionAlgorithm;
using akasha::bullet::buoyant::BuoyantParameter;
using akasha::bullet::buoyant::BuoyantAuhoizer;

Algorithm::btFluidBodyCollisionAlgorithm
(
 btCollisionAlgorithmConstructionInfo const& ci,
 btCollisionObjectWrapper const* fluidBody,
 btCollisionObjectWrapper const* buoyantBody
 ) :
	btActivatingCollisionAlgorithm(ci),
	m_fluid(fluidBody),
	m_obj(buoyantBody),
	m_convexTrianglecallback(
			ci.m_dispatcher1,fluidBody,buoyantBody,false)
{
	m_manifold = m_convexTrianglecallback.m_manifoldPtr;
	m_manifold->setBodies(m_fluid->getCollisionObject(), m_obj->getCollisionObject());
}

//fwd
void fluidGroundProcess(
		btHfFluid const* fluidBody,
		btRigidBody const* body,
		btDispatcherInfo const&,
		btCollisionObjectWrapper const* fluid,
		btCollisionObjectWrapper const* obj,
		btManifoldResult* resultOut,
		btConvexTriangleCallback& triCallback
		);
btScalar processFluid(
		btHfFluid const*,
		btRigidBody const*,
		btConvexTriangleCallback&,
		btDispatcherInfo const&,
		btScalar const density,
		btScalar const floatness,
		btVector3& resultImpluse,
		btVector3& resultTorqueImpluse
		);
std::pair<btVector3, btVector3> applyFluidFriction(
		btRigidBody const* body,
		btScalar const mu,
		btScalar submergedPercent
		);
void Algorithm::processCollision (
		const btCollisionObjectWrapper* body0Wrap,
		const btCollisionObjectWrapper* body1Wrap,
		const btDispatcherInfo& dispatchInfo,
		btManifoldResult* resultOut)
{
	fluidGroundProcess(
			btHfFluid::upcast(m_fluid->getCollisionObject()),
			btRigidBody::upcast(m_obj->getCollisionObject()),
			dispatchInfo,
			body0Wrap,
			body1Wrap,
			resultOut,
			m_convexTrianglecallback);

	void* po =m_obj->getCollisionShape()->getUserPointer();

	btRigidBody const* body =
		btRigidBody::upcast(m_obj->getCollisionObject());

	if (po && body)
	{
		void* object = body->getUserPointer();
		if (!object) return;

		BuoyantParameter const& param =
			BuoyantAuhoizer::getParameter( *(m_obj->getCollisionShape())) ;
		btScalar const mass = btScalar(1.0f) / body->getInvMass();
		btScalar const volume = param.totalVolume_;
		btScalar const density = mass/volume;
		btScalar const floatness = param.floatyness_;

		btVector3 resultImpluse;
		btVector3 resultTorqueImpluse;
		btScalar const submergedVolume = processFluid(
				btHfFluid::upcast(m_fluid->getCollisionObject()),
				body,
				m_convexTrianglecallback,
				dispatchInfo,
				density,
				floatness,
				resultImpluse,
				resultTorqueImpluse);
		if (submergedVolume > btScalar(0.0))
		{
			btScalar const submergedPer = submergedVolume / volume;
			auto result = applyFluidFriction(
					body,
					btHfFluid::upcast(m_fluid->getCollisionObject())->getMu(),
					submergedPer);

			resultImpluse += result.first;
			resultTorqueImpluse += result.second;
		}

		auto* oo = static_cast<akasha::bullet::ObjectOption*>(object);
		oo->m_impluse = resultImpluse;
		oo->m_torqueImpluse = resultTorqueImpluse;
	}


}

btScalar Algorithm::calculateTimeOfImpact(
		btCollisionObject* body0,
		btCollisionObject* body1,
		const btDispatcherInfo& dispatchInfo,
		btManifoldResult* resultOut)
{
	return btScalar(0);
}

void Algorithm::getAllContactManifolds(btManifoldArray&	manifoldArray)
{
	manifoldArray.push_back(
			m_convexTrianglecallback.m_manifoldPtr
			);
}


//////////////////////////////////////////////////////////////////////////

void fluidGroundProcess(
		btHfFluid const* fluidBody,
		btRigidBody const* body,
		btDispatcherInfo const& dispatchInfo,
		btCollisionObjectWrapper const* fluid,
		btCollisionObjectWrapper const* obj,
		btManifoldResult* resultOut,
		btConvexTriangleCallback& triCallback
		)
{
	btScalar const margin = body->getCollisionShape()->getMargin();
	resultOut->setPersistentManifold (triCallback.m_manifoldPtr);
	triCallback.setTimeStepAndCounters(
			margin,
			dispatchInfo,
			fluid,
			obj,
			resultOut
			);
	fluidBody->foreachGroundTriangle(&triCallback,
			triCallback.getAabbMin(), triCallback.getAabbMax());
	resultOut->refreshContactPoints();

}
btScalar processFluid(
		btHfFluid const* fluid,
		btRigidBody const* body,
		btConvexTriangleCallback& triCallback,
		btDispatcherInfo const& dispatchInfo,
		btScalar const density,
		btScalar const floatness,
		btVector3& resultImpluse,
		btVector3& resultTorqueImpluse
		)
{
	assert(fluid);
	assert(body);
	btHfFluidColumnRigidBodyCallback callback(
			body,
			dispatchInfo.m_debugDraw,
			density,
			floatness
			);
	fluid->foreachFluidColumn(
			&callback, triCallback.getAabbMin(),triCallback.getAabbMax());
	resultImpluse = callback.m_resultImpluse;
	resultTorqueImpluse = callback.m_resultTorqueImpluse;
	return callback.getVolume();
}

// Friction
std::pair<btVector3, btVector3> applyFluidFriction(
		btRigidBody const* body,
		btScalar const mu,
		btScalar submergedPercent
		)
{
	btScalar const unit =
		-(1.0f/60.f) * mu * submergedPercent * btScalar(0.4f);
	btVector3 const& linear_vel = body->getLinearVelocity();

	return std::make_pair(
			linear_vel * linear_vel.length2() * unit, //velocity^3 * unit
			body->getAngularVelocity() * unit
			);
}
