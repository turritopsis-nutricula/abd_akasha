
#include "PhysicsBehavior.hpp"
#include "../../../akashaWorldCommon.h"
namespace elem = akasha::model::element_v2;
namespace behav = elem::behavior;

namespace element = akasha::model::element;
namespace figuer = akasha::model::element::figuer;
namespace shapeBase = akasha::model::builder::shapeBase;
using ::akasha::BulletManager;

namespace parameter = ::akasha::model::element::parameter;
using ::akasha::model::element_v2::behavior::PhysicsBehavior;


typedef std::pair<boost::shared_ptr<btFractureBody>, boost::shared_ptr<btCollisionShape>> buildResultType;
typedef boost::mpl::string < 'c', 'h', 'i', 'p' > coreMeshName;
typedef boost::mpl::string < 'j', 'e', 't' > jetMeshName;
typedef boost::mpl::string < 'a', 'r', 'm' > armMeshName;
typedef boost::mpl::string < 'f', 'r', 'a', 'm', 'e' > frameMeshName;
typedef boost::mpl::string < 'w', 'e', 'i', 'g', 'h', 't' > weightMeshName;

template<typename MeshName, bool MeshDirect>
struct PanelEntity
{
	static std::string
		getMeshName()
		{
			return std::string(
					boost::mpl::c_str<MeshName>::value )
				+ std::string( ".x" );
			//std::string("chip.x");

		}
	static
		buildResultType
		build( element::ElementData const& data, float mass, PhysicsBehavior::MotionState* state )
		{
			boost::shared_ptr<btCollisionShape> shape =
				data.getBtShape(
						boost::type<shapeBase::Panel>(),
						shapeBase::ShapeParameter {1000}
						);

			btVector3 inner;
			shape->calculateLocalInertia( mass, inner );
			btRigidBody::btRigidBodyConstructionInfo info(
					mass,
					state,
					shape.get(),
					inner );

			auto objects =
				BulletManager::Instance().createFractureBody(info);
			auto bodyPtr = objects.first;
			BulletManager::Instance().addRigidBody(
					bodyPtr.get(), data.collisionGroup_, data.collisionMask_
					);

			// 			boost::shared_ptr<btFractureBody> bodyPtr(
			// 					new btFractureBody( info ),
			// 					//entity::btCollisionObjectDeleter(BulletManager::Instance().getWorld())
			// 					bulletObjectDeleter( BulletManager::Instance().getWorld() )
			// 					);
			// 			BulletManager::Instance().getWorld().addRigidBody(
			// 					bodyPtr.get(), data.collisionGroup_, data.collisionMask_ );

			if ( MeshDirect && data.parentId_ != -1 )
			{
				// IrrNodeを接続方向に合わせてオフセット
				btTransform mine =
					elem::direct::GetChildTransform(data.direct_);

				btVector3& dir = mine.getOrigin();
				const btVector3 b = btVector3( btScalar( 0.f ),
						btScalar( 0.f ), btScalar( 1.f ) );
				const btVector3 axis = dir.cross( b );

				if ( axis.length2() != btScalar( 0.0 ) )
					state->offset_.getBasis().setRotation(
							btQuaternion( axis, -dir.angle( b ) ) );
				else
					state->offset_.getBasis().setRotation(
							btQuaternion(
								btVector3( btScalar( 0.0 ),
									btScalar( 1.0 ),
									btScalar( 0.0 ) ),
								-dir.angle( b ) ) );
			}

			return buildResultType( bodyPtr, shape );
		}




	template<typename T>
		static void
		applyShadow( const T& )
		{
		}
};

typedef PanelEntity<coreMeshName, true> CoreEntity;
//typedef PanelEntity<frameMeshName, true> FrameEntity;
typedef PanelEntity<armMeshName, true> ArmEntity;

//! Frame
struct FrameEntity
{
	typedef PanelEntity<frameMeshName, true> BaseType;

	static std::string
		getMeshName()
		{
			return BaseType::getMeshName();
		}

	static
		buildResultType
		build( element::ElementData const& data, float mass, PhysicsBehavior::MotionState* state )
		{
			element::ElementData d( data );
			if ( auto o = data.getParameters().getValue(
						boost::type<parameter::option>() ) )
				if ( *o == btScalar( 1.0 ) )
					d.collisionMask_ = d.collisionMask_
						& ~ akasha::ColisionType::COL_TYPE_LAND;

			return BaseType::build( d, mass, state );
		}



	template<typename T>
		static void
		applyShadow( const T& )
		{
		}
};
//! Weight
struct WeightEntity
{
	typedef PanelEntity<weightMeshName, true> BaseType;

	static std::string
		getMeshName()
		{
			return BaseType::getMeshName();
		}

	static
		buildResultType
		build( element::ElementData const& data, float mass, PhysicsBehavior::MotionState* state )
		{

			if ( auto o = data.getParameters().getValue(
						boost::type<parameter::option>() ) )
			{

				mass *= ( float ) ( btClamped( btScalar( *o ),
							btScalar( 1.0 ), btScalar( 8.0 ) ) );
			}

			return BaseType::build( data, mass, state );
		}



	template<typename T>
		static void
		applyShadow( const T& )
		{
		}

};

//! Wheel
struct WheelEntity
{
	static std::string
		getMeshName()
		{
			return std::string( "wheel.x" );
		}

	static
		buildResultType
		build( element::ElementData const& data, float mass, PhysicsBehavior::MotionState* state )
		{
			boost::shared_ptr<btCollisionShape> shape =
				data.getBtShape(
						boost::type<shapeBase::Disc>(),
						// boost::type<shapeBase::Sphere>(),
						shapeBase::ShapeParameter {1000}
						);

			btVector3 inner;
			shape->calculateLocalInertia( mass, inner );
			btRigidBody::btRigidBodyConstructionInfo info(
					mass,
					state,
					shape.get(),
					inner );

			auto objects =
				BulletManager::Instance().createFractureBody(info);
			auto bodyPtr = objects.first;
			BulletManager::Instance().addRigidBody(
					bodyPtr.get(), data.collisionGroup_, data.collisionMask_
					);
			// 			boost::shared_ptr<btFractureBody> bodyPtr(
			// 					new btFractureBody( info ),
			// 					bulletObjectDeleter( BulletManager::Instance().getWorld() )
			// 					);
			// 			BulletManager::Instance().getWorld().addRigidBody(
			// 					bodyPtr.get(), data.collisionGroup_, data.collisionMask_ );

			buildResultType r = buildResultType( bodyPtr, shape );
			r.first->setFriction( 0.7f );


			return  r;
		}

	template<typename T>
		static void
		applyShadow( const T& )
		{
		}
};

struct RimEntity
{
	static std::string
		getMeshName()
		{
			return std::string( "rim.x" );
		}

	static
		buildResultType
		build( element::ElementData const& data, float mass, PhysicsBehavior::MotionState* state )
		{
			boost::shared_ptr<btCollisionShape> shape =
				data.getBtShape( boost::type<shapeBase::Pin>(), shapeBase::ShapeParameter {1000} );

			btVector3 inner;
			shape->calculateLocalInertia( mass, inner );
			btRigidBody::btRigidBodyConstructionInfo info(
					mass,
					state,
					shape.get(),
					inner );

			auto objects =
				BulletManager::Instance().createFractureBody(info);
			auto bodyPtr = objects.first;
			BulletManager::Instance().addRigidBody(
					bodyPtr.get(), data.collisionGroup_, data.collisionMask_
					);
			// 			boost::shared_ptr<btFractureBody> bodyPtr(
			// 					new btFractureBody( info ),
			// 					bulletObjectDeleter( BulletManager::Instance().getWorld() )
			// 					);
			// 			BulletManager::Instance().getWorld().addRigidBody(
			// 					bodyPtr.get(), data.collisionGroup_, data.collisionMask_ );

			return buildResultType( bodyPtr, shape );
		}


	template<typename T>
		static void
		applyShadow( const T& )
		{
		}
};

struct JetEntity
{
	static std::string
		getMeshName()
		{
			return std::string( "jet.x" );
		}

	static
		buildResultType
		build( element::ElementData const& data, float mass, PhysicsBehavior::MotionState* state )
		{
			boost::shared_ptr<btCollisionShape> shape =
				data.getBtShape( boost::type<shapeBase::Disc>(),
						shapeBase::ShapeParameter {1000}
						);

			btVector3 inner;
			shape->calculateLocalInertia( mass, inner );
			btRigidBody::btRigidBodyConstructionInfo info(
					mass,
					state,
					shape.get(),
					inner );

			auto objects =
				BulletManager::Instance().createFractureBody(info);
			auto bodyPtr = objects.first;
			BulletManager::Instance().addRigidBody(
					bodyPtr.get(), data.collisionGroup_, data.collisionMask_
					);
			// 			boost::shared_ptr<btFractureBody> bodyPtr(
			// 					new btFractureBody( info ),
			// 					bulletObjectDeleter( BulletManager::Instance().getWorld() )
			// 					);
			// 			BulletManager::Instance().getWorld().addRigidBody(
			// 					bodyPtr.get(), data.collisionGroup_, data.collisionMask_ );

			return buildResultType( bodyPtr, shape );
		}

};


//Create Panel
void behav::PhysicsBehavior::createPanel( phase::Initialize const& info)
{
	auto& parameter = *(info.paramPtr_);
	auto const& data = info.info_.data_;

	float mass = 10.f;
	if (auto m =
			parameter.getProperty("mass", boost::type<float>()))
	{
		mass = *m;
	}

	m_state.parameter_ = &parameter;
	m_state.offset_ = btTransform::getIdentity();

	//TODO : テスト実装
	boost::tie( m_body, m_shape) =
		CoreEntity::build( data, mass, &m_state);

	auto& body = *m_body;
	body.setMotionState( &m_state );

	auto tree = akasha::system::GetSettingTree(
			"MIZUNUKI.Physics.Model.Body");
	assert(tree);

	body.setActivationState( DISABLE_DEACTIVATION );
	body.setCcdMotionThreshold( tree->get<btScalar>("ccd_threshold", btScalar(4.f) ));
	body.setCcdSweptSphereRadius( tree->get<btScalar>("ccd_radius", btScalar( 0.3 )));

	//Figuer
	{
		std::string fileName;
		switch (parameter.getTypeId()) {
		case 0:
		case 1:
		case 2:
		case 6:
			fileName = "chip.x";
			break;
		case 33:
		case 34:
		case 35:
			fileName = "frame.x";
			break;
		default:
			assert(false &&"PhysicsBehavior createPanel()");
		} // switch parameter->

		m_figuer.create(
				fileName,
				info.info_.irrEx_,
				info.nodeUpdateTask_
				);
	}
	m_state.figure_ = &m_figuer;
}
//Create Rim
void behav::PhysicsBehavior::createRim( phase::Initialize const& info)
{
	auto& parameter = *(info.paramPtr_);
	auto const& data = info.info_.data_;

	float mass = 10.f;
	if (auto m = parameter.getProperty("mass",
				boost::type<float>()))
		mass = *m;

	m_state.parameter_ = &parameter;
	m_state.offset_ = btTransform::getIdentity();

	//TODO : テスト実装
	boost::tie( m_body, m_shape) =
		RimEntity::build( data, mass, &m_state);

	auto& body = *m_body;
	body.setMotionState( &m_state );

	auto tree = akasha::system::GetSettingTree(
			"MIZUNUKI.Physics.Model.Body");
	assert(tree);

	body.setActivationState( DISABLE_DEACTIVATION );
	body.setCcdMotionThreshold( tree->get<btScalar>("ccd_threshold", btScalar(4.f) ));
	body.setCcdSweptSphereRadius( tree->get<btScalar>("ccd_radius", btScalar( 0.3 )));

	//Figuer
	m_figuer.create( "rim.x", info.info_.irrEx_, info.nodeUpdateTask_ );
	m_state.figure_ = &m_figuer;
}

//Create Wheel
void behav::PhysicsBehavior::createWheel( phase::Initialize const& info)
{
	auto& parameter = *(info.paramPtr_);
	auto const& data = info.info_.data_;

	float mass = 10.f;
	if (auto m = parameter.getProperty("mass",
				boost::type<float>()))
		mass = *m;

	m_state.parameter_ = &parameter;
	m_state.offset_ = btTransform::getIdentity();

	//TODO : テスト実装
	boost::tie( m_body, m_shape) =
		WheelEntity::build( data, mass, &m_state);

	auto& body = *m_body;
	body.setMotionState( &m_state );

	auto tree = akasha::system::GetSettingTree(
			"MIZUNUKI.Physics.Model.Body");
	assert(tree);

	body.setActivationState( DISABLE_DEACTIVATION );
	body.setCcdMotionThreshold( tree->get<btScalar>("ccd_threshold", btScalar(4.f) ));
	body.setCcdSweptSphereRadius( tree->get<btScalar>("ccd_radius", btScalar( 0.3 )));

	//Figuer
	m_figuer.create( "wheel.x", info.info_.irrEx_, info.nodeUpdateTask_ );
	m_state.figure_ = &m_figuer;
}

// Create Jet
void behav::PhysicsBehavior::createJet( phase::Initialize const& info)
{
	auto& parameter = *(info.paramPtr_);
	auto const& data = info.info_.data_;

	float mass = 10.f;
	if (auto m = parameter.getProperty("mass",
				boost::type<float>()))
		mass = *m;

	m_state.parameter_ = &parameter;
	m_state.offset_ = btTransform::getIdentity();

	//TODO : テスト実装
	boost::tie( m_body, m_shape) =
		JetEntity::build( data, mass, &m_state);

	auto& body = *m_body;
	body.setMotionState( &m_state );

	auto tree = akasha::system::GetSettingTree(
			"MIZUNUKI.Physics.Model.Body");
	assert(tree);

	body.setActivationState( DISABLE_DEACTIVATION );
	body.setCcdMotionThreshold( tree->get<btScalar>("ccd_threshold", btScalar(4.f) ));
	body.setCcdSweptSphereRadius( tree->get<btScalar>("ccd_radius", btScalar( 0.3 )));

	//Figuer
	m_figuer.create( "jet.x", info.info_.irrEx_, info.nodeUpdateTask_ );
	m_state.figure_ = &m_figuer;
}

//Create Unkown
void behav::PhysicsBehavior::createUnknown( phase::Initialize const& info)
{
	auto& parameter = *(info.paramPtr_);
	auto const& data = info.info_.data_;

	float mass = 10.f;
	if (auto m = parameter.getProperty("mass",
				boost::type<float>()))
		mass = *m;

	m_state.parameter_ = &parameter;
	m_state.offset_ = btTransform::getIdentity();

	//TODO : テスト実装
	boost::tie( m_body, m_shape) =
		CoreEntity::build( data, mass, &m_state);

	auto& body = *m_body;
	body.setMotionState( &m_state );

	body.setActivationState( DISABLE_DEACTIVATION );
	body.setCcdMotionThreshold( btScalar( 4.0 ) );
	body.setCcdSweptSphereRadius( btScalar( 0.3 ) );

	//Figuer
	m_figuer.create( "unknown.x", info.info_.irrEx_, info.nodeUpdateTask_ );
	m_state.figure_ = &m_figuer;

}



void behav::PhysicsBehavior::forceMove(btTransform const& to)
{
	m_body->setWorldTransform( to );
}

void behav::PhysicsBehavior::physicsUpdate(ElementParameter& param)
{
	assert( m_baseParameterPtr );
	m_baseParameterPtr->setWorldTransform( m_body->getWorldTransform());

	param.setWorldTransform( m_body->getWorldTransform() );
	param.setVelocity( m_body->getLinearVelocity() );
	param.setAngularVelocity( m_body->getAngularVelocity() );
}
