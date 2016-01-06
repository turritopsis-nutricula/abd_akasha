#include "type_fwd.hpp"
#include "detail/ElementBase.hpp"

#include "behavior/BatteryBehavior.hpp"
#include "behavior/PhysicsBehavior.hpp"
#include "behavior/FlexibleJointBehavior.hpp"

namespace elem = ::akasha::model::element_v2;
namespace behav = elem::behavior;
namespace fusion = boost::fusion;
using BatteryBehavior = fusion::vector<
behav::BatteryBehavior,
behav::FlexibleXJointBehavior,
	behav::PhysicsBehavior>;

class elem::BatteryElement :
	public elem::detail::ElementBase< elem::BatteryElement, BatteryBehavior>
{
};

template<>
elem::BatteryElement* elem::MakeElement(
		CreateInfo const& d,
		boost::shared_ptr<akasha::NodeUpdateTask> const& task,
		boost::type< elem::BatteryElement > const&
		)
{
	auto* element = new elem::BatteryElement();
	element->init(d, task);

	return element;
}
