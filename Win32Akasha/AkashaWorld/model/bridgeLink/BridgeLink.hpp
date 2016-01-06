#ifndef AKASHA_MODEL_BLIDGELINK_HPP_
#define AKASHA_MODEL_BLIDGELINK_HPP_

#include "../../WispTasks.hpp"
#include "../../bullet/ConstraintWrapper.hpp"

#include <wisp_v3/irrExt.hpp>
#include <LinearMath/btDefaultMotionState.h>


namespace akasha
{
namespace model
{
namespace bridgeLink
{
class BridgeLink
{
	// 				joint::ConstraintWrapper<btPoint2PointConstraint> m_constraint0;
	joint::ConstraintWrapper<btConeTwistConstraint> m_constraint0;
	joint::ConstraintWrapper<btPoint2PointConstraint> m_constraint1;

	boost::shared_ptr<btMotionState> m_state;
	boost::shared_ptr<btCollisionShape> m_shape;
	boost::shared_ptr<btFractureBody> m_linkBody; //TODO : to Rigidbody


public:
	BridgeLink() = default;


	void init(boost::shared_ptr<btRigidBody> const&,
			boost::shared_ptr<btRigidBody> const&,
			irrExt::IrrExtention&,
			boost::shared_ptr<NodeUpdateTask> const&);

	boost::shared_ptr<btFractureBody> const& getBody() const
	{
		return m_linkBody;
	}
};
} // namespace bridgeLink
} // namespace model
} // namespace aka
#endif /* end of include guard */
