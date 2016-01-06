/*
 * ConstraintWrapper.hpp
 *
 *  Created on: 2013/03/14
 *      Author: ely
 */

#ifndef CONSTRAINTWRAPPER_HPP_
#define CONSTRAINTWRAPPER_HPP_

//TODO : bulletディレクトリは不適
#include "../BulletWorldManager.h"

#include <BulletDynamics/ConstraintSolver/btPoint2PointConstraint.h>
#include <BulletDynamics/ConstraintSolver/btHingeConstraint.h>
#include <BulletDynamics/ConstraintSolver/btConeTwistConstraint.h>
#include <BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.h>
#include <BulletDynamics/ConstraintSolver/btGeneric6DofSpringConstraint.h>

namespace akasha
{

namespace joint
{
namespace detail
{

inline btTransform const& GetWorldTransform(btRigidBody* b){
	return b->getCenterOfMassTransform(); }
inline btTransform GetWorldTransform(btFractureBody* b){
	return helper::FractureBodyHelper{b}.getWorldTransform(); }
}

//! btTypedConstraint生成クラス
template<typename T>
struct JointDetail : T
{
};

//! Point2PointConstraint
template<>
struct JointDetail<btPoint2PointConstraint>
{
	btVector3 pivotInA_;
	btVector3 pivotInB_;

	template<typename BodyType0, typename BodyType1>
		btPoint2PointConstraint*
		operator()(BodyType0& _0, BodyType1& _1,
				const btTransform& pivotInWorld) const
		{
			btPoint2PointConstraint* n = new btPoint2PointConstraint(
					_0,
					_1,
					detail::GetWorldTransform(&_0).invXform(
						pivotInWorld.getOrigin()),
					detail::GetWorldTransform(&_1).invXform(
						pivotInWorld.getOrigin()));

			n->setParam(BT_CONSTRAINT_ERP, btScalar(0.2), -1);
			n->setParam(BT_CONSTRAINT_CFM, btScalar(0.), -1);

			return n;
		}
};

template<>
struct JointDetail<btHingeConstraint>
{

	template<typename BodyType0, typename BodyType1>
		btHingeConstraint*
		operator()(BodyType0& _0, BodyType1& _1,
				const btTransform& pivotInWorld) const
		{
			btHingeConstraint* n =  new btHingeConstraint(
					_0,
					_1,
					detail::GetWorldTransform(&_0).inverseTimes(
						pivotInWorld),
					detail::GetWorldTransform(&_1).inverseTimes(
						pivotInWorld));

			//HingeParametorの初期値を予め設定(参照:btContactSolverInfo.h)
			n->setParam(BT_CONSTRAINT_STOP_ERP, btScalar(0.2), -1);
			n->setParam(BT_CONSTRAINT_STOP_CFM, btScalar(0.), -1);
			n->setParam(BT_CONSTRAINT_CFM, btScalar(0.), -1);


			return n;
		}
};

//! btConeTwistConstraint
template<>
struct JointDetail<btConeTwistConstraint>
{

	template<typename BodyType0, typename BodyType1>
		btConeTwistConstraint*
		operator()(BodyType0& _0, BodyType1& _1,
				btTransform const& pivotInWorld) const
		{
			return new btConeTwistConstraint(
					_0,
					_1,
					detail::GetWorldTransform(&_0).inverseTimes(pivotInWorld),
					detail::GetWorldTransform(&_1).inverseTimes(pivotInWorld)
					);
		}
};

//! btGeneric6DofConstraint
template<>
struct JointDetail<btGeneric6DofConstraint>
{
	btTransform frameInA_;
	btTransform frameInB_;
	bool useLinearReferenceFrameA_;

	JointDetail() :
		useLinearReferenceFrameA_(true)
	{
	}

	JointDetail(bool useReferenFrameA) :
		useLinearReferenceFrameA_(useReferenFrameA)
	{
	}

	template<typename BodyType0, typename BodyType1>
		btGeneric6DofConstraint*
		operator()(BodyType0& _0, BodyType1& _1,
				const btTransform& pivotInWorld) const
		{
			return new btGeneric6DofConstraint(
					_0,
					_1,
					detail::GetWorldTransform(&_0).inverseTimes(
						pivotInWorld),
					detail::GetWorldTransform(&_1).inverseTimes(
						pivotInWorld),
					//useLinearReferenceFrameA_
					true);
		}
};

//! btGeneric6DofSpringConstraint
template<>
struct JointDetail<btGeneric6DofSpringConstraint>
{
	btTransform frameInA_;
	btTransform frameInB_;
	bool useLinearReferenceFrameA_;

	JointDetail() :
		useLinearReferenceFrameA_(true)
	{
	}

	JointDetail(bool useReferenFrameA) :
		useLinearReferenceFrameA_(useReferenFrameA)
	{
	}


	template<typename BodyType0, typename BodyType1>
		btGeneric6DofSpringConstraint*
		operator()(BodyType0& _0, BodyType1& _1,
				const btTransform& pivotInWorld) const
		{


			return new btGeneric6DofSpringConstraint(
					_0,
					_1,
					detail::GetWorldTransform(&_0).inverseTimes(pivotInWorld),
					detail::GetWorldTransform(&_1).inverseTimes(pivotInWorld),
					//useLinearReferenceFrameA_
					true);
		}



};


//! PhysicsEntity Joint
template<typename ConstraintType>
class ConstraintWrapper
{
	struct ConstraintDeleter
	{

		void
			operator()(ConstraintType* inst)
			{
				std::cout << "delete from EntityJointDeletor: " << inst <<std::endl;
				BulletManager::Instance().removeConstraint(inst);
				delete inst;
			}
	};
	//typedef BulletManager::hold_type::worldType::entityType worldType;
public:
	typedef joint::JointDetail<ConstraintType> ConstraintArg;

private:
	boost::shared_ptr<btRigidBody> m_object0;
	boost::shared_ptr<btRigidBody> m_object1;

	boost::shared_ptr<ConstraintType> m_constraint;
	bool m_isConnect;

public:

	ConstraintWrapper() :
		m_isConnect( false)
	{
	}

	//! 即時構築
	template<typename BodyType0, typename BodyType1>
		ConstraintWrapper(
				const BodyType0& body0
				,
				const BodyType1& body1,
				const ConstraintArg& arg,  // = ConstraintArg,
				const btTransform& pivot,
				bool isConnect = true) :
			m_object0(body0), m_object1(
					body1),  m_constraint(
						//arg(*m_object0,*m_object1),
						arg(*m_object0, *m_object1, pivot),
						ConstraintDeleter())
	{
		m_constraint->enableFeedback(true);

		if (isConnect)
			connect();
	}

	//! 即時構築
	template<typename BodyType0, typename BodyType1	>
		ConstraintWrapper(
				const BodyType0& body0,
				const BodyType1& body1,
				const btTransform& pivot,
				bool isConnect = true) :
			m_object0(body0), m_object1(
					body1), m_constraint(
						ConstraintArg()(*m_object0, *m_object1,
							pivot)/*,
									  ConstraintDeleter(
									  service.getDynamicsWorld())*/)
	{
		m_constraint->enableFeedback(true);
		if (isConnect)
			connect();
	}

	ConstraintWrapper(
			const boost::shared_ptr<btRigidBody>& body0,
			const boost::shared_ptr<btRigidBody>& body1,
			const btTransform& pivot, const ConstraintArg& arg =
			ConstraintArg(), bool isConnect = true) :
		m_object0(body0), m_object1(body1), m_constraint(
				arg(*m_object0, *m_object1, pivot), ConstraintDeleter()/*,
																							ConstraintDeleter(service.getDynamicsWorld())*/)
	{
		m_constraint->enableFeedback(true);
		if (isConnect)
			connect();
	}

	// Move Constract
	ConstraintWrapper(ConstraintWrapper&& o) :
		m_isConnect( o.m_isConnect)
	{
		m_object0.swap( o.m_object0 );
		m_object1.swap( o.m_object1 );
		m_constraint.swap( o.m_constraint );
	}

	//! btTypedConstraintを取得
	ConstraintType&
		getConstraint() const
		{
			return *m_constraint;
		}

	//debugDraw
	void
		drawDebug()
		{
			BulletManager::Instance().debugDrawConstraint(
					m_constraint.get());
		}

	//! 構築
	template<typename BodyType0, typename BodyType1>
		void
		constract(boost::shared_ptr<BodyType0> body0,
				boost::shared_ptr<BodyType1> body1,
				const btTransform& pivot, const ConstraintArg& arg =
				ConstraintArg())
		{
			m_object0 = body0;
			m_object1 = body1;
			m_constraint.reset(arg(*body0, *body1, pivot), ConstraintDeleter());
			m_constraint->enableFeedback(true);
		}

	//! 接続
	void
		connect()
		{
			if (m_constraint.get()) {
				BulletManager::Instance().addConstraint(m_constraint, true,
						[this](btTypedConstraint* o, btTypedConstraint* n)->boost::shared_ptr<btTypedConstraint>{
						std::cout << "copy:" << o << "to:" << n << std::endl;
						if (this->m_constraint.get() == o)
						{
						boost::shared_ptr<ConstraintType> p(static_cast<ConstraintType*>(n), ConstraintDeleter());
						m_constraint.swap(p);
						m_constraint->enableFeedback(true);
						return m_constraint;
						}
						else
						return boost::shared_ptr<btTypedConstraint>();
						});

				m_isConnect = true;
			}

		}

	//! 切断
	void
		disconnect()
		{
			BulletManager::Instance().removeConstraint(m_constraint.get());
			/*m_service.getDynamicsWorld().removeConstraint(
			  m_constraint.get());*/
			m_isConnect = false;
		}

	//! 接続判定
	bool
		isConnect() const
		{
			return m_isConnect;
		}
};

}
}



#endif /* CONSTRAINTWRAPPER_HPP_ */
