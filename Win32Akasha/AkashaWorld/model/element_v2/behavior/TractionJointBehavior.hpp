#ifndef AKASHA_MODEL_TRACTIONJOINTBEHAIVOR_HPP_
#define AKASHA_MODEL_TRACTIONJOINTBEHAIVOR_HPP_

#include "../event/event.hpp"
#include "../../../bullet/ConstraintWrapper.hpp"


namespace akasha {
namespace model {
namespace element_v2 {
namespace behavior {

namespace detail
{
class TractionJoint
{

	using joint_type =
		joint::ConstraintWrapper<btHingeConstraint>;

	joint_type m_joint;

public:
	void init(element::ElementData const& data, ElementParameter const& param);

	void connect(
			element_v2::DirectType const& direct,
			boost::shared_ptr<btFractureBody> parentBody,
			boost::shared_ptr<btFractureBody> mineBody,
			ElementParameter const& param
			);
	void traction(float const accel, float const brake,float step);
};
} // namespace detail

// Output to ElementPrameter
class TractionJointBehavior {

	detail::TractionJoint m_joint;

	float m_power;
	float m_brake;

public:

	template<typename T>
		void apply(T const&){ }

	void apply(phase::Initialize const& info)
	{
		m_joint.init(info.info_.data_, *(info.paramPtr_));
		m_power = 0.f;
		m_brake = 0.f;
	}

	void apply(phase::ElementConnect const& e)
	{
		this->connect(e.direct1_, e.parentBody_, e.mineBody_, e.paramRef_);
	}

	void connect(
			element_v2::DirectType const& direct,
			boost::shared_ptr<btFractureBody> parentBody,
			boost::shared_ptr<btFractureBody> mineBody,
			ElementParameter const& param
			)
	{
		m_joint.connect(direct, parentBody, mineBody, param);
	}
	void traction(float const accel, float const brake,float step)
	{
		m_joint.traction(accel, brake, step);
	}

};

} // namespace behavior

namespace handler
{
struct TractionJointHandler
{
	template<typename EventType>
		void tractionHandle(
				EventType const&,
				behavior::TractionJointBehavior&,
				ElementParameter&
				) { }

};

}
} // namespace element_v2
} // namespace model
} // namespace akasha
#endif //AKASHA_MODEL_TRACTIONJOINTBEHAIVOR_HPP_
