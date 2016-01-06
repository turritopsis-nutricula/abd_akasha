/*
 * Receptors.hpp
 *
 *  Created on: 2012/08/29
 *      Author: ely
 */

#ifndef RECEPTORS_HPP_
#define RECEPTORS_HPP_


#include "../entity/EntityReceptor.hpp"

namespace akasha
{

namespace model
{
//! Model Element
template<typename ElementType, int TypeID>
	struct ElementReceptor : entity::DefaultReceptorImp
{
	ElementType* this_;


	float
		shotImpact(const btVector3& pos, const btVector3& dir,
				float density)
		{
			return this_->shotHit(pos, dir, density);
		}

};

template<typename ElementType>
struct ElementReceptor<ElementType, 4> : ElementReceptor<ElementType, 0>
{
	typedef ElementReceptor<ElementType, 0> BaseType;
	btScalar
		calculateCombinedFriction(const btCollisionObject* body0,const btCollisionObject* body1, btScalar ratio)
		{
			btScalar friction = body0->getFriction() * body1->getFriction() * ratio;
			const btScalar MAX_FRICTION  = btScalar(10.);
			if (friction < -MAX_FRICTION)
				friction = -MAX_FRICTION;
			if (friction > MAX_FRICTION)
				friction = MAX_FRICTION;
			return friction;
		}
	btScalar
		calcFrictionRatio(btManifoldPoint const& p)
		{
			btRigidBody* b = BaseType::this_->getPhysicsBody().get();
			assert(b);


			btScalar t = 1.f-btFabs(p.getAppliedImpulse());
			btScalar ang = b->getLinearVelocity().normalized().dot(b->getWorldTransform().getBasis().getColumn(1));
			btScalar s = 1.f-btPow((btFabs(ang)-0.3f)*0.8f, 2.f);
			t *= s;
			btClamp(t, 0.f, 1.f);

			return t;
		}

	bool
		processContact0(btManifoldPoint& p, btCollisionObject* other)
		{
			btRigidBody* b = BaseType::this_->getPhysicsBody().get();
			p.m_combinedFriction = p.getDistance()<=1.0e-3 ? calculateCombinedFriction(b, other, calcFrictionRatio(p)) : 0.f;
			return true;
		}

	bool
		processContact1(btManifoldPoint& p, btCollisionObject* other)
		{
			btRigidBody* b = BaseType::this_->getPhysicsBody().get();
			p.m_combinedFriction = p.getDistance()<=1.0e-3 ? calculateCombinedFriction(b, other, calcFrictionRatio(p)) : 0.f;
			return true;
		}
};



}


namespace detection
{
////////////////////////////////////////////////////////////////
template<class ShotType>
	struct ShotReceptor : entity::DefaultReceptorImp
{
	ShotType* s_;

	void
		subStepContact0(btCollisionObject const* o, entity::EntityReceptor& r,
				const btPersistentManifold& m)
		{
		}
	void
		subStepContact1(btCollisionObject const* o, entity::EntityReceptor& r,
				const btPersistentManifold& m)
		{
		}

	void
		stepContact0(btCollisionObject* o, entity::EntityReceptor& r,
				const btPersistentManifold& m)
		{
			assert(s_!=NULL);
			s_->contact(o, r, m);
		}

	void
		stepContact1(btCollisionObject* o, entity::EntityReceptor& r,
				const btPersistentManifold& m)
		{
			assert(s_!=NULL);
			s_->contact(o, r, m);
		}
	float
		shotImpact(const btVector3& position, const btVector3& direct,
				float density)
		{
			return 1.0;
		}

};
}


}

#endif /* RECEPTORS_HPP_ */
