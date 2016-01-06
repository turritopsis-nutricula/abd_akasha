#include "BridgeLink.hpp"
#include "../../akashaWorldCommon.h"

#include "../../../AkashaSystem/AkashaSystem.hpp"

#include <BulletCollision/CollisionShapes/btBoxShape.h>

#include <irrlicht/IMeshSceneNode.h>
#include <irrlicht/ISceneManager.h>
#include <irrlicht/IAnimatedMeshSceneNode.h>

#include <boost/make_shared.hpp>

using akasha::model::bridgeLink::BridgeLink;

struct BridgeMotionState : btMotionState
{
	boost::shared_ptr< akasha::NodeUpdateTask > m_task;
	wisp::ObjectID m_id;

	virtual void getWorldTransform(btTransform& t)  const override
	{
		t = btTransform::getIdentity();
	}

	virtual void setWorldTransform(btTransform const& t) override
	{
		m_task->add(*m_id, t);
	}
};


void BridgeLink::init(
		boost::shared_ptr<btRigidBody> const& body0,
		boost::shared_ptr<btRigidBody> const& body1,
		irrExt::IrrExtention& irrEx,
		boost::shared_ptr<NodeUpdateTask> const& nodeTask)
{

	m_shape = boost::make_shared<btBoxShape>(
			btVector3(0.1f,0.5f,0.1f));

	{
		auto state = boost::make_shared<BridgeMotionState>();
		//Figuer
		state->m_task = nodeTask;
		state->m_id = irrEx.createObject(
				[](
					boost::uuids::uuid id,
					irr::IrrlichtDevice* irr,
					wisp::IrrObjectStore& store)
				{
				wisp::WorkingDir workingDir(irr);
				akasha::system::SetCurrentDirectory(irr);

				auto* mgr = irr->getSceneManager();

				std::string const path("Resources/element/link.x");
				auto* node = mgr->addAnimatedMeshSceneNode(
						mgr->getMesh( irr::io::path( path.c_str() ) ));
				if ( node ){
				store.store(id, node, 0);
				}

				});

		m_state = state;
	}


	btTransform const initTrans (
			[&]()->btTransform
			{//calc basis
			btVector3 const& vec0 = body0->getWorldTransform().getOrigin();
			btVector3 const& vec1 = body1->getWorldTransform().getOrigin();
			btVector3 const sub(vec1-vec0);
			btVector3 const base(btScalar(.0), btScalar(1.0), btScalar(0.));
			btScalar const angle(-btAngle(sub, base));
			btVector3 const cross(sub.cross(base));

			return btTransform(
				btQuaternion(cross,angle ),
				(vec0+vec1)*0.5f);
			}()
	);
	{
		btVector3 inner;
		btScalar mass( 1.f );
		m_shape->calculateLocalInertia( mass, inner);
		btRigidBody::btRigidBodyConstructionInfo info(
				mass,
				m_state.get(),
				m_shape.get(),
				inner
				);

		m_state->setWorldTransform( initTrans );

		auto result =
			BulletManager::Instance().createFractureBody(info);
		m_linkBody = result.first;

		m_linkBody->setWorldTransform( initTrans );

		BulletManager::Instance().addRigidBody(
				m_linkBody.get(),
				akasha::ColisionType::COL_TYPE_MODEL,
				akasha::ColisionType::COL_TYPE_LAND
				);
	}

	auto tree = akasha::system::GetSettingTree(
			"MIZUNUKI.Physics.Model.Joints.LinkJoint");
	assert(tree);

	float const efp = tree->get<float>("erp", 0.2f);
	float const cfm = tree->get<float>("cfm", 0.f);


	m_constraint0.constract(body0, m_linkBody,
			body0->getWorldTransform());
	m_constraint1.constract( body1, m_linkBody,
			body1->getWorldTransform());

	m_constraint0.getConstraint().setParam( BT_CONSTRAINT_ERP, efp, 0);
	m_constraint0.getConstraint().setParam( BT_CONSTRAINT_CFM, cfm, 0);
	m_constraint1.getConstraint().setParam( BT_CONSTRAINT_ERP, efp, -1);
	m_constraint1.getConstraint().setParam( BT_CONSTRAINT_CFM, cfm, -1);

	m_constraint0.connect();
	m_constraint1.connect();
}
