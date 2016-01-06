#ifndef AVATAR_BASE__HPP
#define AVATAR_BASE__HPP

#include "../entity/EntityTypeHelper.hpp"
#include "../entity/IrrMeshs.hpp"

namespace akasha
{
namespace avatar
{
/*
	class AvatarBase
	{
	public:
	AvatarBase(
	entity::EntityService& entService
	) :
	m_entityService(entService),
	m_isRide(false)
	{

	typedef entity::CollisionShape<
	btBoxShape,
	entity::btVector3Parameter> BoxShape;

	typedef entity::EntityBase<BoxShape, entity::CubeGeometryMesh> baseType;

	float x = 0.4f ,y = 1.7f ,z = 0.2f;
	baseType base(
	entity::btVector3Parameter(x*0.5f ,y*0.5f,z*0.5f),
	boost::fusion::make_vector( irr::core::vector3df(x,y,z) )
	);

	entity::EntityConstructionInfo constInfo(30.f);
	constInfo.m_collisonGroup = COL_TYPE_AVATAR;
	constInfo.m_collisonMask = COL_TYPE_TOY | COL_TYPE_MODEL | COL_TYPE_LAND;
	m_entity.constract
	(
	m_entityService,
	base,
	constInfo
	);
	m_entity.attach(m_entityService);

	btRigidBody& btBody = m_entity.getBody();
	btBody.setActivationState(DISABLE_DEACTIVATION);
	btBody.setAngularFactor( btVector3(0.0f,0.2f,0.0f));

//âºé¿ëïÉèÅ[Év
m_entity.getBody().getWorldTransform().getOrigin() +=
btVector3(0.f, 52.f, 0.f);
}

void update()
{
if(!m_isRide)
move();
}

void move()
{
btVector3 force(0.f,0.f,0.f);
/
btRigidBody& body = m_entity.getBody();
const btMatrix3x3& basis = body.getWorldTransform().getBasis();
//ê˘âÒ
body.setAngularVelocity(btVector3(0.f, force.getX(), 0.f));

btVector3 wForce = basis * force - body.getLinearVelocity();
wForce.setY( force.getY());
body.applyCentralImpulse(wForce);
}

const btTransform& getTransform() const
{
return m_entity.getBody().getWorldTransform();
}

void rideOn()
{
m_isRide = true;
}
void getOff()
{
	m_isRide = false;
}
private:
entity::EntityService&        m_entityService;
entity::PhysicEntity          m_entity;

bool m_isRide;
};*/
}
}
#endif //AVATAR_BASE__HPP
