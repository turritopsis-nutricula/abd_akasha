#ifndef AKASHA_WORLD_WISP_TASKS_HPP_
#define AKASHA_WORLD_WISP_TASKS_HPP_

#include <wisp_v3/wisp.hpp>
#include <wisp_v3/irrExt.hpp>

#include <LinearMath/btTransform.h>

#include <boost/uuid/uuid.hpp>

namespace
{
inline float degees(btScalar x)
{
	return x * (360.f / (2.f*3.14159));
}
}

namespace akasha
{
struct NodeUpdateArg
{
	boost::uuids::uuid nodeId_;
	irr::core::vector3df position_;
	irr::core::vector3df rotate_;
	NodeUpdateArg(
			boost::uuids::uuid id,
			float x, float y, float z,
			float rx, float ry, float rz
			) :
		nodeId_(id),position_(x,y,z), rotate_(rx,ry,rz){}
	NodeUpdateArg(NodeUpdateArg&& o) :
		position_(o.position_), rotate_(o.rotate_)
	{
		nodeId_.swap(o.nodeId_);
	}

};

using NodeUpdateArgType = std::vector<NodeUpdateArg>;

inline void UpdateTaskFunc(
		NodeUpdateArgType const& up,
		irrExt::TaskArg const& a)
{
	for (auto const& d : up)
	{
		auto* node = a.store_.find(d.nodeId_);
		if (!node) return;
		node->setPosition(d.position_);
		node->setRotation(d.rotate_);
	}
}

class NodeUpdateTask : private boost::noncopyable
{
	using NodeMotionTaskType =
		boost::shared_ptr< irrExt::Task<NodeUpdateArgType> >;
	NodeMotionTaskType m_task;
	NodeUpdateArgType m_args;

public:
	void init(irrExt::IrrExtention& irrEx)
	{
// 		auto& wisp = wisp::v3::CreateWispHandle();
		m_task = irrEx.createTask(
				UpdateTaskFunc,
				boost::type<NodeUpdateArgType>());
	}
	void apply()
	{
		m_task->swapPost( m_args );
		m_args.clear();
	}
	void add(boost::uuids::uuid const& id,btTransform const& trans)
	{
		btScalar x,y,z;
		trans.getBasis().getEulerYPR(y,x,z);
		auto const& v = trans.getOrigin();
		m_args.emplace_back(
				id,v.getX(), v.getY(), v.getZ(),
				degees( z ),degees( x ), degees(y)
				);
	}

};


} // namespace akasha
#endif /* end of include guard */
