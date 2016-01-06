/*
 * ObjectCapture.hpp
 *
 *  Created on: 2012/01/14
 *      Author: ely
 */

#ifndef OBJECTCAPTURE_HPP_
#define OBJECTCAPTURE_HPP_

#include "../PlayerView.hpp"

#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/at_c.hpp>

namespace akasha
{
namespace detection
{
class ObjectCapture
{
	btDiscreteDynamicsWorld& m_world;
	boost::shared_ptr<PlayerView> m_viewPtr;
	boost::optional<btRigidBody&> m_captured;
	btScalar m_captureFraction;
	btVector3 m_capturePosition;
public:
	ObjectCapture(btDiscreteDynamicsWorld& world,
			boost::shared_ptr<PlayerView> view) :
		m_world(world), m_viewPtr(view), m_captureFraction(0.)
	{
	}

	void
		update(btScalar fps)
		{
			if (!m_captured)
				return;

			const auto l = m_viewPtr->getMouseCursorRayDirect();
			const btVector3 from = boost::fusion::at_c<0>(l);
			const btVector3 to = boost::fusion::at_c<1>(l)
				* m_captureFraction;

			btRigidBody& body = *m_captured;
			const btVector3 destance = (to + from)
				- body.getWorldTransform().getOrigin();
			const btVector3 untiVelocity = -body.getLinearVelocity();
			const btVector3 gravity(0.f, 9.81, 0.f);
			body.applyCentralImpulse(
					(destance * 60.f + untiVelocity * 9.f + gravity)
					* btScalar(1.0) / body.getInvMass() * fps);
			/*body.applyImpulse(
			  (destance * 60.f + untiVelocity * 10.f + gravity)
			 * btScalar(1.0) / body.getInvMass() * fps,
			 m_capturePosition);*/

		}

	void
		capture(btScalar limitDistance)
		{
			const auto l = m_viewPtr->getMouseCursorRayDirect();
			const btVector3 from = boost::fusion::at_c<0>(l);
			const btVector3 to = boost::fusion::at_c<1>(l);

			btCollisionWorld::ClosestRayResultCallback ray(from, to);
			ray.m_collisionFilterGroup = COL_TYPE_LAND;
			ray.m_collisionFilterMask = COL_TYPE_TOY;
			m_world.rayTest(from, to, ray);

			if (ray.hasHit()) {

				const btVector3& h = ray.m_hitPointWorld;

				std::stringstream ss;
				ss << "capture X:" << h.getX() << "Y:" << h.getY() << "Z:"
					<< h.getZ();
				akasha::system::AkashaSystem::Instance().getLogger()(
						system::log::ApplicationInfo( { ss.str() }));

				m_captureFraction = ray.m_closestHitFraction;

				btRigidBody const* o = btRigidBody::upcast(ray.m_collisionObject);
				if (o && !o->isStaticObject()) {
					m_capturePosition = o->getWorldTransform().invXform(h);
					// 						m_captured = *(static_cast<btRigidBody*>(o));
					m_captured = *(const_cast<btRigidBody*>(o));//TODO
					o->forceActivationState(true);

				}
			}

		}
	void
		release()
		{
			m_captured = boost::none;
		}

};
}
}

#endif /* OBJECTCAPTURE_HPP_ */
