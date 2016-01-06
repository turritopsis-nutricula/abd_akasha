#ifndef AKASHA_WORLD_BULLET_FLUIDDYNAMICS_BUOYANT_BUORANTAUTHOIZER_HPP_
#define AKASHA_WORLD_BULLET_FLUIDDYNAMICS_BUOYANT_BUORANTAUTHOIZER_HPP_

#include "BuoyantParameter.hpp"
#include <BulletCollision/CollisionShapes/btConvexShape.h>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/unordered_map.hpp>
namespace akasha
{
namespace bullet
{
namespace buoyant {
class BuoyantAuhoizer : public boost::enable_shared_from_this<BuoyantAuhoizer>
{
	typedef
		boost::unordered_map<
		btConvexShape const* ,
						  boost::shared_ptr<BuoyantParameter>> buoyant_map_t;
	buoyant_map_t m_buoyantMap;

	struct Deleter
	{
		boost::shared_ptr<BuoyantAuhoizer> auhoizer_;
		void operator()(btConvexShape* s) const
		{
			auhoizer_->release(s);
			delete s;
		}
	};
	struct ParameterDeleter
	{
		void operator()(BuoyantParameter* p) const
		{
			if (p->voxelPositions_)
				btAlignedFree(p->voxelPositions_);
			delete p;
		}

	};

public:
	boost::shared_ptr<btCollisionShape> auhoize(btConvexShape* shape)
	{
		assert( m_buoyantMap.count(shape)==0 );
		assert( shape->getUserPointer() == nullptr );
		boost::shared_ptr<btCollisionShape> res(shape,Deleter{this->shared_from_this()});
		boost::shared_ptr<BuoyantParameter>
			param(new BuoyantParameter(),
					ParameterDeleter());
		param->voxelPositions_ = nullptr;

		//calculate
		calcParameter(*param, shape, 0.01f, 0.05f);

		m_buoyantMap[shape] = param;
		res->setUserPointer(param.get());

		return res;
	}
	static BuoyantParameter& getParameter(btCollisionShape& s)
	{
		return *(static_cast<BuoyantParameter*>(s.getUserPointer()));
	}
	static BuoyantParameter const& getParameter(btCollisionShape const& s)
	{
		return *(static_cast<BuoyantParameter const*>(s.getUserPointer()));
	}
private:
	void release(btConvexShape* s)
	{
		m_buoyantMap.erase(s);
	}
	void calcParameter(
			BuoyantParameter& result,
			btConvexShape const* shape,
			btScalar const redius,
			btScalar const gap);


};
}
} // namespace bullet
} // namespace akasha
#endif /* end of include guard */
