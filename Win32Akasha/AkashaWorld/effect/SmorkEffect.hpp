#ifndef AKASHA_WORLD_EFFECT_SMORKEFFECT_HPP_
#define AKASHA_WORLD_EFFECT_SMORKEFFECT_HPP_

#include "../contactEvent/ContactEventManager.hpp"
#include "../../AkashaSystem/AkashaSystem.hpp"

#include <wisp_v3/wisp.hpp>
#include <wisp_v3/irrExt.hpp>

#include <ISceneManager.h>
#include <IParticleSystemSceneNode.h>

#include <boost/shared_ptr.hpp>
namespace akasha
{
namespace effect
{


class SmorkEffectManager
{
public:
	struct parameter
	{
		irr::core::vector3df pos_;
		irr::core::vector3df dir_;
		irr::f32 weight_;
		irr::f32 step_;
	};

private:
	using workArg_t = std::vector<parameter>;

	wisp::v3::WispHandle* m_client;
	wisp::ObjectID m_paricleSystem;
	workArg_t m_commands;
	boost::shared_ptr<irrExt::Task<workArg_t>> m_particleTask;
public:

	void setup(irrExt::IrrExtention&);

	void add( parameter t )
	{
		m_commands.push_back( std::move(t) );
	}
	void update( const float step );

private:
	static void WorkFunc(workArg_t const&, irrExt::TaskArg const&, boost::uuids::uuid const);
public:
	static contactEvent::ContactEventManager::excuteType
		CreateContactFunc(boost::shared_ptr<SmorkEffectManager> this_);

};



} // namespace effect
} // namespace akasha


#include <BulletCollision/CollisionDispatch/btManifoldResult.h>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

namespace akasha {
namespace effect {

inline contactEvent::ContactEventManager::excuteType
	SmorkEffectManager::CreateContactFunc(boost::shared_ptr<SmorkEffectManager> this_)
	{
		return [this_](btPersistentManifold* pt,
				btCollisionObject const* object0,
				btCollisionObject const* object1)
		{
			// BasicElement < BasicLand
			assert(btRigidBody::upcast(object0));
			btRigidBody const* b = static_cast<btRigidBody const*>(object0);

			if (
					!(b->getLinearVelocity().length2() > btPow(2.f,2.f) ||
						b->getAngularVelocity().length2() > btPow(4.f,2.f))
				) return;

			for (int i=0; i < pt->getNumContacts();++i)
			{
				auto const& p = pt->getContactPoint(i);
				if (p.getDistance() <= btScalar(1.0e-2))
				{
					bool const swaped(object1==pt->getBody0());
					btVector3 const& landNormal = swaped ? -p.m_normalWorldOnB : p.m_normalWorldOnB;
					btVector3 const& localPoint = swaped ? p.m_localPointB : p.m_localPointA;

					const btVector3 lVel = b->getVelocityInLocalPoint(
							landNormal * (b->getWorldTransform().getBasis() * localPoint).dot(landNormal));

					if(lVel.length2() > 1.f)
					{
						const btVector3 dir = p.m_normalWorldOnB.cross(lVel.cross( p.m_normalWorldOnB ))/50.f;
						const auto& pos = p.getPositionWorldOnA();
						this_->add(
								effect::SmorkEffectManager::parameter {
								irr::core::vector3df(pos.getX(),pos.getY(), pos.getZ() ),
								irr::core::vector3df(-(dir.getX()), -(dir.getY()), -(dir.getZ()) ),
								lVel.length()}
								);
					} // lVel length()
				} //if point distance()
			} //for
		};
	}
} // namespace effect
} // namespace akasha
#endif /* end of include guard */

