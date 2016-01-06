#include "BtShapeBuilder.hpp"
#include "../../../AkashaSystem/AkashaSystem.hpp"

#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <boost/make_shared.hpp>

using akasha::model::builder::BtShapeBuilder;

// 									btHfFluidBuoyantConvexShape*
// 										shape = new btHfFluidBuoyantConvexShape(
// 												creater::ShapeCreator<ShapeType>::create(p, boost::type<TagType>()));
//
// 									shape->generateShape(0.01f, 0.05f);
// 									shape->setFloatyness(0.2f);


using akasha::model::builder::shapeBase::ShapeParameter;

//Panel
	btConvexShape*
CreatePanle(ShapeParameter const& p)
{

	float s = ((float)p.size_ / 1000.f);

	auto tree = akasha::system::GetSettingTree("MIZUNUKI.Physics.Model.Shape.Panel");
	assert(tree);

	btScalar minThick = tree->get<btScalar>("min_thickness", 0.03f);
	auto* shape =
		new btBoxShape( btVector3(0.3f*s, std::max(0.02f*s, minThick), 0.3f*s));

	shape->setMargin(  tree->get<btScalar>( "margin", 0.01f ));

	return shape;
}
	btConvexShape*
CreateDisc(ShapeParameter const& p)
{
	float s = ((float)p.size_ / 1000.f);

	auto tree = akasha::system::GetSettingTree("MIZUNUKI.Physics.Model.Shape.Disc");
	assert(tree);

	btScalar minThick = tree->get<btScalar>("min_thickness", 0.03f);

	auto* shape = new btCylinderShape(
			btVector3(
				0.3f*s,
				std::max(0.02f*s, minThick),
				0.3f*s));

	shape->setMargin(  tree->get<btScalar>( "margin", 0.01f ));

	return shape;
}
	btConvexShape*
CreatePin(ShapeParameter const& p)
{

	float s = ((float)p.size_ / 1000.f);
	return new btCylinderShape(
			btVector3(0.2f*s,0.02f*s, 0.2f*s)
			);
}

//Sphere
	btConvexShape*
CreateSphere(ShapeParameter const& p)
{
	float s=((float)p.size_ / 1000.f);
	return new btSphereShape(0.3f*s);
}
namespace akasha {
namespace model {
namespace builder {
namespace detail {

template<>
	btConvexShape*
	CreateRawShape<0>(ShapeParameter const& p)
	{
		return CreatePanle(p);
	}
template<>
	btConvexShape*
	CreateRawShape<1>(ShapeParameter const& p)
	{
		return CreateDisc(p);
	}
template<>
	btConvexShape*
	CreateRawShape<2>(ShapeParameter const& p)
	{
		return CreatePin(p);
	}
template<>
	btConvexShape*
	CreateRawShape<3>(ShapeParameter const& p)
	{
		return CreateSphere(p);
	}

} // namespace detail
} // namespace builder
} // namespace model
} // namespace akasha
