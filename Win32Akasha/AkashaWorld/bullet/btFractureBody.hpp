/*
 * btFractureBody.hpp
 *
 *  Created on: 2012/09/09
 *      Author: ely
 */

#ifndef BTFRACTUREBODY_HPP_
#define BTFRACTUREBODY_HPP_

#include "LinearMath/btAlignedObjectArray.h"
#include "BulletCollision/CollisionShapes/btCompoundShape.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "BulletCollision/CollisionDispatch/btUnionFind.h"

#include <boost/range/adaptor/map.hpp>
#include <boost/unordered_map.hpp>
#include <boost/optional.hpp>



#define CUSTOM_FRACTURE_TYPE (btRigidBody::CO_USER_TYPE+1)


class btFractureBody;
template<typename BaseType>
class btFractureManipulatorImpl
{
	//typedef BaseType btFractureBody;
	BaseType* body_;

public:
	btFractureManipulatorImpl() = delete;
	btFractureManipulatorImpl(BaseType* b) : body_(b){ }



	btTransform
		getWorldTransform() const
		{
			if (body_->isUnion())
			{
				BaseType* r = body_->isWrap() ? body_ : body_->getConnectionInfo().rootBody_;
				return
					r->getWorldTransform() *
					static_cast<btCompoundShape*>(r->getCollisionShape())->getChildTransform(
							body_->getConnectionInfo().childIndex_);
			}
			else
				return body_->getWorldTransform();
		}
	void
		setWorldTransform(btTransform const& t)
		{
			if (body_->isUnion())
			{
				BaseType* r = body_->isWrap() ? body_ : body_->getConnectionInfo().rootBody_;
				r->setWorldTransform(t *
						static_cast<btCompoundShape*>(r->getCollisionShape())->getChildTransform(
							body_->getConnectionInfo().childIndex_).inverse());
				r->updateChildBody();
			}
			else
				body_->setWorldTransform(t);
		}

	void
		applyCentralImpulse(const btVector3 &impulse)
		{
			getRootBody()->applyImpulse(impulse, getRelativePointInWorld());
		}

	void
		applyImpulse(const btVector3 & impulse, const btVector3 & rel_pos)
		{
			getRootBody()->applyImpulse(impulse, rel_pos+getRelativePointInWorld());
		}
	void
		applyTorqueImpulse(const btVector3 & torque)
		{
			getRootBody()->applyTorqueImpulse(torque);
		}

	btVector3
		getLinearVelocity() const
		{
			return getRootBody()->getVelocityInLocalPoint(getRelativePointInWorld());
		}
	const btVector3&
		getAngularVelocity() const
		{
			return getRootBody()->getAngularVelocity();
		}

private:
	btVector3
		getRelativePointInWorld() const
		{
			if (body_->isUnion())
			{
				BaseType* r = body_->isWrap() ? body_ : body_->getConnectionInfo().rootBody_;
				return r->getWorldTransform().getBasis() *
					static_cast<btCompoundShape*>(r->getCollisionShape())->getChildTransform(
							body_->getConnectionInfo().childIndex_).getOrigin();
			}
			else
				return btVector3(0.f,0.f,0.f);
		}
	btFractureBody*
		getRootBody() const
		{
			return body_->isUnion() ? body_->isWrap() ? body_ : body_->getConnectionInfo().rootBody_ : body_;
		}

};


class btFractureBody : public btRigidBody
{
public:
	struct connectionInfo
	{
		btFractureBody* rootBody_;
		btFractureBody* connectionBody_;
		int childIndex_;

		btScalar threadImpulse_;
		btScalar breakImpulse_;
		btScalar bufTotalImpulse_;
	};
private:


	connectionInfo m_connectionInfo;


	btAlignedObjectArray<btFractureBody*> m_childBodies;


	btCollisionShape* m_holdShape;
	btScalar m_holdMass;


	btFractureManipulatorImpl<btFractureBody> m_manipulator;
public:
	btFractureBody() = delete;
	btFractureBody(btRigidBody::btRigidBodyConstructionInfo const& constructionInfo, btScalar strength=100.f) :
		btRigidBody(constructionInfo),
		m_holdShape(constructionInfo.m_collisionShape),
		m_holdMass(constructionInfo.m_mass),
		m_manipulator(this)
	{
		m_internalType += CUSTOM_FRACTURE_TYPE;

		m_connectionInfo.rootBody_ = 0;
		m_connectionInfo.connectionBody_ = 0;
		m_connectionInfo.threadImpulse_ = strength/10.f;
		m_connectionInfo.breakImpulse_ = strength;
	}
	virtual
		~btFractureBody()
		{
			if (isWrap())
			{  //btCompuondShapeが入ってるので消去
				btCollisionShape* s = getCollisionShape();
				delete s;
				setCollisionShape(m_holdShape);
			}


		}


	//welding
	void
		resetRootBody(btFractureBody* root, btFractureBody* parent=0)
		{
			if (parent)
				internalResetRootBody(root, parent);
			else
				internalResetRootBody(root, root);
		}

	//rootから切り離す
	void
		fractBody()
		{
			if (!m_connectionInfo.rootBody_) return;

			btFractureBody* rootBody = m_connectionInfo.rootBody_;
			//rootBody->resetConnections();
			auto& childList = rootBody->m_childBodies;

			//unionFindの生成と処理
			btUnionFind u;
			btAlignedObjectArray<int> tags;
			tags.resize(childList.size());
			int index = 0;



			for (int i = 0; i < childList.size(); i++) {
#ifdef STATIC_SIMULATION_ISLAND_OPTIMIZATION
				tags[i] = index++;
#else
				tags[i] = i;
				index = i + 1;
#endif
			}
			u.reset(index);

			for (int i=0; i<childList.size(); i++)
			{
				btFractureBody* b = childList.at(i);
				auto const& c = b->getConnectionInfo();


				if (!(b ==this || b==rootBody))
				{
					u.unite(
							tags[c.childIndex_],
							tags[c.connectionBody_->getConnectionInfo().childIndex_]
							);

				}

			}
			index=0;
			for (int i=0; i< childList.size();i++)
			{
				tags[i] = u.find(index);
				//Set the correct object offset in Collision Object Array
#if STATIC_SIMULATION_ISLAND_OPTIMIZATION
				u.getElement(index).m_sz = i;
#endif //STATIC_SIMULATION_ISLAND_OPTIMIZATION
				index++;
			}

			u.sortIslands();


			//connectionListの処理
			//szにconnectionIndexが入っている//////////////////
			btAlignedObjectArray<btFractureBody*> newBodys;


			//int index;
			//indexを進める

			for (index=0; index<u.getNumElements() && u.getElement(index).m_id==0 ; index++)
			{
			}


			for(int i=index; i<u.getNumElements(); i++)
				newBodys.push_back( rootBody->getBodyPtrByShapeIndex( u.getElement(i).m_sz ) );

			rootBody->removeChild(this);
			for (int i=0; i<newBodys.size(); i++)
			{
				btFractureBody* b= newBodys.at(i);
				if (b!=this)
				{
					b->internalResetRootBody(this, b->getConnectionInfo().connectionBody_);
					b->resetConnections_();
				}
			}


			rootBody->resetConnections_();
			resetConnections_();


			assert(this->valied());
		}
	//! CompoundShapeの再計算
	void recomputeChild()
	{
		assert(valied());
		if (!isWrap()) return;

		btCompoundShape& cs = *(static_cast<btCompoundShape*>(getCollisionShape()));
		btTransform principal;
		btVector3 prnicipalIneria;
		btAlignedObjectArray<btScalar> masses;
		btScalar totalMass(0.);

		for (int i=0; i<m_childBodies.size(); i++)
		{
			masses.push_back(m_childBodies[i]->m_holdMass);
			totalMass += m_childBodies[i]->m_holdMass;
		}

		cs.calculatePrincipalAxisTransform(
				&(masses[0]), principal,prnicipalIneria);

		for (int i=0; i<m_childBodies.size(); i++)
			cs.updateChildTransform(i,principal.inverse() * cs.getChildTransform(i));

		setMassProps(totalMass, prnicipalIneria);

		btRigidBody::setWorldTransform(btRigidBody::getWorldTransform()*principal);

		assert(valied());
	}

	//! ChildBodyの情報を擬似更新
	void
		updateChildBody()
		{
			assert(valied());
			if (!isWrap()) return;
			for (int i=1; i<m_childBodies.size(); i++)
			{
				btFractureBody* b = m_childBodies.at(i);
				assert(b!=this);
				btTransform const& shift = static_cast<btCompoundShape*>(getCollisionShape())->getChildTransform(i);

				b->setInterpolationWorldTransform(getInterpolationWorldTransform()*shift);
				b->setWorldTransform(this->getWorldTransform()*shift);
				/*
					b->setInterpolationAngularVelocity(
					shift.getBasis() * this->getInterpolationAngularVelocity()
					);
					b->setInterpolationLinearVelocity(
					this->getInterpolationLinearVelocity() +
					this->getInterpolationAngularVelocity().cross(shift.getOrigin())
					);*/
				b->setAngularVelocity(this->getAngularVelocity());
				b->setLinearVelocity( this->getVelocityInLocalPoint( this->getWorldTransform().getBasis() * shift.getOrigin()));
				b->setHitFraction(this->getHitFraction());
			}
		}


	void
		resetConnections_()
		{
			if (!isWrap())
				//m_connectionInfo.rootBody_ = 0;
				return;

			auto* shape = static_cast<btCompoundShape*>(getCollisionShape());
			typedef boost::unordered_map<btCollisionShape*, int> map_type;
			map_type m;

			for(int i=0; i<shape->getNumChildShapes(); i++)
				m[shape->getChildShape(i)] = i;

			for (int i=0; i<m_childBodies.size();i++)
			{
				auto* b = m_childBodies.at(i);
				auto& c = b->m_connectionInfo;
				assert(c.rootBody_ ? c.rootBody_==this : true);
				assert(m.count(b->m_holdShape)==1);
				c.childIndex_ = m[b->m_holdShape];
			}

		}




	/////////////////////////////////////////////////

	btFractureBody*
		getBodyPtrByShapeIndex(int i) const
		{
			assert(isWrap());
			return m_childBodies.at(i);
		}
	int
		getChildShapeIndexByBodyPtr(btFractureBody* b) const
		{
			return m_childBodies.findLinearSearch(b);
		}
	bool
		isWrap() const
		{
			return getCollisionShape() != m_holdShape;
		}
	bool
		isUnion() const
		{
			return isWrap() || m_connectionInfo.rootBody_;
		}
	int
		getChildBodyCount() const
		{
			return isWrap() ? m_childBodies.size() : 0;
		}


	btScalar
		getStrength() const
		{
			return m_connectionInfo.breakImpulse_;
			//return m_weldingStrength;
		}
	void
		setStrength(btScalar s)
		{
			m_connectionInfo.threadImpulse_ = s*0.1f;
			m_connectionInfo.breakImpulse_ = s;
			//m_weldingStrength = s;
		}
	btScalar
		getMass() const
		{
			return m_holdMass;
		}

	connectionInfo&
		getConnectionInfo()
		{
			return m_connectionInfo;
		}
	void
		resetConnectionInfo()
		{
			m_connectionInfo.bufTotalImpulse_ = 0;
		}
	btFractureManipulatorImpl<btFractureBody>&
		getManipulator()
		{
			return m_manipulator;
		}
	btFractureManipulatorImpl<btFractureBody> const&
		getManipulator() const
		{
			return m_manipulator;
		}

	static
		btFractureBody*
		upcast(btCollisionObject* o)
		{
			if (o->getInternalType() & CUSTOM_FRACTURE_TYPE)
				return (btFractureBody*)o;

			return 0;
		}
	static
		const btFractureBody*
		upcast(const btCollisionObject* o)
		{
			if (o->getInternalType() & CUSTOM_FRACTURE_TYPE)
				return (const btFractureBody*)o;

			return 0;
		}
private:
	//デバック関数
	bool
		valied() const
		{

			bool v;

			if (m_connectionInfo.rootBody_)
			{//子の場合
				v = m_childBodies.size()==0;
			}
			else
			{//代表の場合
				if (isWrap())
				{//子持ちの場合
					v = m_childBodies.size() == static_cast<const btCompoundShape*>(getCollisionShape())->getNumChildShapes() &&
						//子リストの一番目は自身
						m_childBodies[0]== this;
				}
				else
				{//単体の場合
					v = m_connectionInfo.rootBody_==0 &&
						m_childBodies.size()==0;
				}
			}

			return  v;

		}

	void
		internalResetRootBody(btFractureBody* root, btFractureBody* parent)
		{
			assert(root);
			assert(root!=this);

			//rootが根持ちだった場合//////////////////////////////////////////////
			if (root->m_connectionInfo.rootBody_)
			{
				internalResetRootBody(root->m_connectionInfo.rootBody_, parent);

				return;
			}

			//以下rootが新しい根になる場合//////////////////////////////////////////
			if (root!=m_connectionInfo.rootBody_)
			{

				if(isWrap())
				{  //今まで自身が親だった
					//TODO : 後のaddChild時にWorldTransformがズレるため力技
					btTransform buf( getWorldTransform()*static_cast<btCompoundShape*>(getCollisionShape())->getChildTransform(0));
					for (int i=0; i<m_childBodies.size(); i++)
					{
						btFractureBody* b = m_childBodies[i];
						if(b!=this)
							b->internalResetRootBody(root, b->getConnectionInfo().connectionBody_);
					}
					setWorldTransform(buf);
				}

				if (m_connectionInfo.rootBody_)
				{  //Rootの挿げ替え
					btFractureBody* old = m_connectionInfo.rootBody_;
					old->removeChild( this );
					old->resetConnections_();//oldRootの更新
				}

				m_connectionInfo.rootBody_ = root;

				m_connectionInfo.rootBody_->addChild(this);



				m_connectionInfo.rootBody_ = root;
				m_connectionInfo.connectionBody_=parent;


			}
			assert(valied());
		}

	void
		addChild(btFractureBody* f)
		{
			assert(f->m_connectionInfo.rootBody_ == this);
			assert(!f->isWrap());

			if (!isWrap())
			{
				auto* n = new btCompoundShape(true);
				setCollisionShape(n);
				n->addChildShape(btTransform::getIdentity(), m_holdShape);
				m_childBodies.push_back(this);
			}

			btCompoundShape* s = static_cast<btCompoundShape*>(getCollisionShape());


			s->addChildShape(
					getWorldTransform().inverse() * f->getWorldTransform(),
					f->m_holdShape);
			m_childBodies.push_back(f);
			f->m_connectionInfo.rootBody_ = this;
			f->m_connectionInfo.childIndex_ = s->getNumChildShapes()-1;

			recomputeChild();
		}
	void
		removeChild(btFractureBody* f)
		{
			assert(isWrap());
			assert(f != this);

			m_childBodies.remove(f);

			btCompoundShape* s = static_cast<btCompoundShape*>(getCollisionShape());
			s->removeChildShape(f->m_holdShape);

			if (m_childBodies.size()==1)  //自身のみ
			{
				assert(m_childBodies[0]==this);
				setWorldTransform(getWorldTransform() * s->getChildTransform(0));
				setCollisionShape( m_holdShape );
				m_childBodies.clear();
				delete s;
			}

			f->m_connectionInfo.rootBody_ = 0;

			recomputeChild();
			resetConnections_();

		}
};

namespace helper
{

struct FractureBodyHelper
{
	btFractureBody* body;

	btFractureBody*
		getRootBody() const
		{
			return (body->isUnion() && !body->isWrap()) ? body->getConnectionInfo().rootBody_ : body;
		}
	std::vector<btFractureBody*>
		getUnionBodiesList() const
		{
			std::vector<btFractureBody*> res;
			btFractureBody* root = getRootBody();
			res.push_back(root);
			for (int i=1; i<root->getChildBodyCount();i++)
				res.push_back(root->getBodyPtrByShapeIndex(i));

			return res;
		}
	btCompoundShape*
		getCompondShape() const
		{
			return body->isWrap() ?
				static_cast<btCompoundShape*>(body->getCollisionShape()) :
				body->isUnion() ?
				FractureBodyHelper{body->getConnectionInfo().rootBody_}.getCompondShape() : 0;
		}

	boost::optional<btTransform>
		getCompondTransform() const
		{
			auto* s = getCompondShape();
			if (s)
				return s->getChildTransform(getRootBody()->getChildShapeIndexByBodyPtr(body));

			return boost::none;
		}
	btTransform
		getWorldTransform() const
		{
			FractureBodyHelper root{getRootBody()};
			if (auto ct = getCompondTransform())
				return root.body->getWorldTransform() * (*ct);
			return body->getWorldTransform();
		}
};



}

typedef btFractureManipulatorImpl<btFractureBody> btFractureManipulator;



#endif /* BTFRACTUREBODY_HPP_ */
