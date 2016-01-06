/*
 * ConstraintHelper.hpp
 *
 *  Created on: 2012/09/20
 *      Author: ely
 */

#ifndef CONSTRAINTHELPER_HPP_
#define CONSTRAINTHELPER_HPP_

#include "BulletDynamics/ConstraintSolver/btPoint2PointConstraint.h"
#include "BulletDynamics/ConstraintSolver/btConeTwistConstraint.h"
#include "BulletDynamics/ConstraintSolver/btHingeConstraint.h"
#include "BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.h"
#include "BulletDynamics/ConstraintSolver/btGeneric6DofSpringConstraint.h"

#include "LinearMath/btSerializer.h"

namespace helper
{


template<typename C>
	static btTypedConstraint*
	copyConstraint(C* base, btRigidBody& a, btRigidBody& b)
{
	btAssert(0);
	return 0;
}

template<>
	btTypedConstraint*
copyConstraint(btPoint2PointConstraint* base, btRigidBody& a,
		btRigidBody& b)
{

	btVector3 pa = base->getPivotInA(), pb=base->getPivotInB();

	btPoint2PointConstraint* c = new btPoint2PointConstraint(a, b,pa, pb);
	c->setParam(BT_CONSTRAINT_STOP_ERP,
			base->getParam(BT_CONSTRAINT_STOP_ERP));
	c->setParam(BT_CONSTRAINT_STOP_CFM,
			base->getParam(BT_CONSTRAINT_STOP_CFM));
	c->setBreakingImpulseThreshold(base->getBreakingImpulseThreshold());

	//std::cout << c->getParam(BT_CONSTRAINT_ERP) << " "  << c->getParam(BT_CONSTRAINT_CFM) << std::endl;
	c->m_setting = base->m_setting;
	c->setEnabled(base->isEnabled());
	return c;

}
template<>
	btTypedConstraint*
copyConstraint(btHingeConstraint* base, btRigidBody& a, btRigidBody& b)
{
	/*int maxSerializeBufferSize = sizeof(btHingeConstraint);
	  btDefaultSerializer archive(maxSerializeBufferSize);
	  btHingeConstraintDoubleData data;

	  archive.startSerialization();
	  base->serialize(&data, &archive);
	  archive.finishSerialization();*/

	btHingeConstraint* c = new btHingeConstraint(a,b,base->getFrameOffsetA(),base->getFrameOffsetB());


	c->setAngularOnly( base->getAngularOnly() );
	c->setLimit(base->getLowerLimit(), base->getUpperLimit());
	c->setMaxMotorImpulse(base->getMaxMotorImpulse());

	for (int n=1; n<5; n++)
		c->setParam(n, base->getParam(n, -1), -1);

	c->setUseFrameOffset(base->getUseFrameOffset());

	//setAxis(btVector3 &axisInA)
	//setFrames(const btTransform &frameA, const btTransform &frameB)


	//c->setMotorTarget(const btQuaternion &qAinB, btScalar dt)
	//c->setMotorTarget(base->get targetAngle, btScalar dt)
	/*
		setOverrideNumSolverIterations(int overideNumIterations)	btTypedConstraint	[inline]
		setBreakingImpulseThreshold(btScalar threshold)	btTypedConstraint	[inline]
		setDbgDrawSize(btScalar dbgDrawSize)	btTypedConstraint	[inline]
		setEnabled(bool enabled)	btTypedConstraint	[inline]
		setUserConstraintId(int uid)	btTypedConstraint	[inline]
		setUserConstraintPtr(void *ptr)	btTypedConstraint	[inline]
		setUserConstraintType(int userConstraintType)*/
	return c;
}
//btGeneric6DofConstraint
template<>
	btTypedConstraint*
copyConstraint(btGeneric6DofConstraint* base, btRigidBody& a,
		btRigidBody& b)
{

	btGeneric6DofConstraint* c = new btGeneric6DofConstraint(a, b,
			base->getFrameOffsetA(), base->getFrameOffsetB(), false/*Žg‚í‚ê‚Ä‚¢‚È‚¢*/
			);

	//breakImpulse
	c->setBreakingImpulseThreshold(base->getBreakingImpulseThreshold());

	//LimitMotors
	(*(c->getTranslationalLimitMotor())) =
		(*(base->getTranslationalLimitMotor()));
	for (int i = 0; i < 3; i++)
		(*(c->getRotationalLimitMotor(i))) =
			(*(base->getRotationalLimitMotor(i)));

	c->setEnabled(base->isEnabled());

	return c;
}

//btGeneric6DofSpringConstraint
template<>
	btTypedConstraint*
copyConstraint(btGeneric6DofSpringConstraint* base, btRigidBody& a,
		btRigidBody& b)
{
	const int maxSerializeBufferSize =  1024 * 1024 * 5; //base->calculateSerializeBufferSize();
	btDefaultSerializer archive(maxSerializeBufferSize);
	btGeneric6DofSpringConstraintData data;

	archive.startSerialization();
	base->serialize(&data, &archive);
	archive.finishSerialization();

	btGeneric6DofSpringConstraint* c =
		new btGeneric6DofSpringConstraint(a, b,
				base->getFrameOffsetA(), base->getFrameOffsetB(),
				data.m_6dofData.m_useLinearReferenceFrameA);

	for (int i = 0; i < 6; i++) {
		c->setStiffness(i, data.m_springStiffness[i]);
		c->setEquilibriumPoint(i, data.m_equilibriumPoint[i]);
		c->enableSpring(i, data.m_springEnabled[i] != 0);
		c->setDamping(i, data.m_springDamping[i]);
	}
	//breakImpulse
	c->setBreakingImpulseThreshold(base->getBreakingImpulseThreshold());

	//LimitMotors
	(*(c->getTranslationalLimitMotor())) =
		(*(base->getTranslationalLimitMotor()));
	for (int i = 0; i < 3; i++)
		(*(c->getRotationalLimitMotor(i))) =
			(*(base->getRotationalLimitMotor(i)));

	c->setEnabled(base->isEnabled());

	return c;

}

	static btTypedConstraint*
changeConstraintBody(btTypedConstraint* base, btRigidBody& a,
		btRigidBody& b)
{
	switch (base->getObjectType())
	{
	case POINT2POINT_CONSTRAINT_TYPE:
		return copyConstraint(static_cast<btPoint2PointConstraint*>(base),
				a, b);
	case HINGE_CONSTRAINT_TYPE:
		return copyConstraint(static_cast<btHingeConstraint*>(base),a,b);

	case CONETWIST_CONSTRAINT_TYPE:
		break;
	case D6_CONSTRAINT_TYPE:
		return copyConstraint(static_cast<btGeneric6DofConstraint*>(base),
				a, b);
	case SLIDER_CONSTRAINT_TYPE:
		break;
	case CONTACT_CONSTRAINT_TYPE:
		break;
	case D6_SPRING_CONSTRAINT_TYPE:
		return copyConstraint(
				static_cast<btGeneric6DofSpringConstraint*>(base), a, b);
	default:
		assert(0);
		break;
	}
	return (btTypedConstraint*) 0;
}

//PivotShift
////////////////////////////////
struct FixedBodyGetter : public btTypedConstraint
{
private:
	void getInfo1 (btConstraintInfo1* info){}
	void getInfo2 (btConstraintInfo2* info){}
	void	setParam(int num, btScalar value, int axis = -1){}
	btScalar getParam(int num, int axis = -1) const {return 0;}

public:
	static
		btRigidBody& getFixedBody()
		{
			return btTypedConstraint::getFixedBody();
		}
	static
		bool isFixedBody(btCollisionObject* o)
		{
			return &getFixedBody() == o;
		}
};
	static void
pivotShift(btPoint2PointConstraint& c, btTransform const& from0, btTransform const& from1, btTransform const& to0, btTransform const& to1)
{
	btVector3 a = to0.inverse()(from0(c.getPivotInA()));
	btVector3 b = !FixedBodyGetter::isFixedBody(&(c.getRigidBodyB()))  ?
		to1.inverse()(from1(c.getPivotInB())) : c.getRigidBodyA().getWorldTransform()(a);
	c.setPivotA(a);
	c.setPivotB(b);
}
	static void
pivotShift(btPoint2PointConstraint& c, bool isB, btTransform const& from,  btTransform const& to)
{
	if (isB)
		c.setPivotB(
				!FixedBodyGetter::isFixedBody(&(c.getRigidBodyB()))  ?
				to.inverse()(from(c.getPivotInB())) :
				c.getRigidBodyA().getWorldTransform()(c.getPivotInA()));
	else
		c.setPivotA(to.inverse()(from(c.getPivotInA())));
}

template<typename ConstraintType>
	static void
transformShift1(ConstraintType& c, btTransform const& from0, btTransform const& from1, btTransform const& to0, btTransform const& to1)
{
	btTransform frameB = to1.inverse()*from1*c.getFrameOffsetB();
	c.setFrames(
			!FixedBodyGetter::isFixedBody(&(c.getRigidBodyA())) ?
			to0.inverse() * from0 * c.getFrameOffsetA() :
			c.getRigidBodyB().getWorldTransform() * frameB,
			frameB);
}

template<typename ConstraintType>
	static void
transformShift1(ConstraintType& c, bool isB, btTransform const& from,  btTransform const& to)
{
	//btTransform _t = to.inverse()*from*c.getFrameOffsetB();
	//btTransform _j = c.getFrameOffsetA();
	if (isB)
		c.setFrames(c.getFrameOffsetA(),to.inverse()*from*c.getFrameOffsetB());
	else
	{
		const btTransform frameA = to.inverse() * from * c.getFrameOffsetA();
		c.setFrames(
				!FixedBodyGetter::isFixedBody(&(c.getRigidBodyA())) ?
				frameA :
				c.getRigidBodyB().getWorldTransform() * c.getFrameOffsetB(),
				c.getFrameOffsetB());
	}

}
	static void
ConstraintPivotShift(btTypedConstraint& c, btTransform const& f0, btTransform const& f1, btTransform const& t0, btTransform const& t1)
{

	switch (c.getObjectType())
	{
	case POINT2POINT_CONSTRAINT_TYPE:
		return pivotShift(static_cast<btPoint2PointConstraint&>(c),
				f0,f1,t0,t1);
	case HINGE_CONSTRAINT_TYPE:
		return transformShift1(static_cast<btHingeConstraint&>(c),f0,f1,t0,t1);
	case CONETWIST_CONSTRAINT_TYPE:
		assert(0);
		break;
	case D6_CONSTRAINT_TYPE:
		return transformShift1(static_cast<btGeneric6DofConstraint&>(c),
				f0, f1, t0, t1);
	case SLIDER_CONSTRAINT_TYPE:
		assert(0);
		break;
	case CONTACT_CONSTRAINT_TYPE:
		break;
	case D6_SPRING_CONSTRAINT_TYPE:
		return transformShift1(
				static_cast<btGeneric6DofSpringConstraint&>(c), f0,f1,t0,t1);
	default:
		assert(0);
		break;
	}
	return;
}


	static void
ConstraintPivotShift(btTypedConstraint& c, bool isB, btTransform const& from, btTransform const& to)
{

	switch (c.getObjectType())
	{
	case POINT2POINT_CONSTRAINT_TYPE:
		return pivotShift(static_cast<btPoint2PointConstraint&>(c),isB,	from,to);
	case HINGE_CONSTRAINT_TYPE:
		return transformShift1(static_cast<btHingeConstraint&>(c),isB, from, to);
	case CONETWIST_CONSTRAINT_TYPE:
		assert(0);
		break;
	case D6_CONSTRAINT_TYPE:
		return transformShift1(static_cast<btGeneric6DofConstraint&>(c),isB, from,to);
	case SLIDER_CONSTRAINT_TYPE:
		assert(0);
		break;
	case CONTACT_CONSTRAINT_TYPE:
		break;
	case D6_SPRING_CONSTRAINT_TYPE:
		return transformShift1(
				static_cast<btGeneric6DofSpringConstraint&>(c),isB, from,to);
	default:
		assert(0);
		break;
	}
	return;
}
}


#endif /* CONSTRAINTHELPER_HPP_ */
