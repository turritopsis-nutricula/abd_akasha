#include "TractionJointBehavior.hpp"

namespace element_v2 = ::akasha::model::element_v2;
using akasha::model::element_v2::behavior::detail::TractionJoint;

void TractionJoint::init(
		element::ElementData const& data,
		element_v2::ElementParameter const& param)
{

}

void TractionJoint::connect(
		element_v2::DirectType const&,
		boost::shared_ptr<btFractureBody> parentBody,
		boost::shared_ptr<btFractureBody> mineBody,
		ElementParameter const& param
		)
{
	if(!parentBody)
	{
		assert(false);
		return;
	}

	btTransform const& parentTransform = parentBody->getWorldTransform();
	btTransform const mineTransform( parentTransform );
	btTransform const joint(btQuaternion(
				btVector3(btScalar(1), btScalar(0), btScalar(0)),SIMD_HALF_PI
				), btVector3(btScalar(0),btScalar(0),btScalar(0)));

	mineBody->setWorldTransform( mineTransform );
	m_joint.constract(
			parentBody,
			mineBody,
			parentTransform * joint
			);
	{
		// constraint setting
		auto& c = m_joint.getConstraint();
		auto const tree = system::GetSettingTree(
				"MIZUNUKI.Physics.Model.Joints.HingeJoint");
		assert(tree);

		btScalar erp = tree->get<btScalar>("erp", btScalar(0.6));
		btScalar cfm = tree->get<btScalar>("cfm", btScalar(0.0));

		c.setParam(BT_CONSTRAINT_STOP_ERP, erp);
		c.setParam(BT_CONSTRAINT_STOP_CFM, cfm);
		c.setParam(BT_CONSTRAINT_CFM, 0.0f);
	}
	m_joint.connect();
}

void TractionJoint::traction(float const powerValue,float const brakeValue, float const step)
{
	btRigidBody& body0 = m_joint.getConstraint().getRigidBodyA();
	btRigidBody& body1 = m_joint.getConstraint().getRigidBodyB();

	float const accel(powerValue*step*btScalar(1.0/12.0));
	float const brake(brakeValue*step*btScalar(1.0/12.0));
	//accel
// 	body0.applyTorqueImpulse(
// 			body0.getWorldTransform().getBasis() *
// 			btVector3( btScalar(0), -accel, btScalar(0)));
// 	body1.applyTorqueImpulse(
// 			body1.getWorldTransform().getBasis() *
// 			btVector3(btScalar(0), accel, btScalar(0)));
	body0.applyTorqueImpulse(
			body0.getWorldTransform().getBasis().getColumn(1) * -accel);
	body1.applyTorqueImpulse(
			body1.getWorldTransform().getBasis().getColumn(1) * accel);
	//break
	m_joint.getConstraint().enableAngularMotor(
			true, btScalar(0), brake);
}
