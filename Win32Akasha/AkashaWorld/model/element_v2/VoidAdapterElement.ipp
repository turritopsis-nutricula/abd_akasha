#include "type_fwd.hpp"
#include "detail/ElementBase.hpp"

#include "behavior/AdapterFiguerBehavior.hpp"

namespace elem = akasha::model::element;
namespace elem_v2 = akasha::model::element_v2;

//Adapter
using VoidAdapterBehavior =
boost::fusion::vector<behav::AdapterFiguerFiguerBehavior>;

struct AdapterHandler
{
	template<typename EventType>
		void invoke(EventType const&, VoidAdapterBehavior&, elem_v2::ElementParameter&)
		{
		}

	void invoke(
			event::StepEnd const&,
			VoidAdapterBehavior& b,
			elem_v2::ElementParameter& p
			)
	{
		boost::fusion::at_c<0>( b ).transformUpdate(p);
	}
};

class elem_v2::VoidAdapterElement :
	public elem_v2::detail::ElementBase<
	elem_v2::VoidAdapterElement,
	VoidAdapterBehavior,
	AdapterHandler>{};

template<>
elem_v2::VoidAdapterElement* elem_v2::MakeElement(
		CreateInfo const& d,
		boost::shared_ptr<akasha::NodeUpdateTask> const& task,
		boost::type< elem_v2::VoidAdapterElement> const&
		)
{
	auto* elem = new elem_v2::VoidAdapterElement();
	elem->init(d, task);

	return elem;
}

