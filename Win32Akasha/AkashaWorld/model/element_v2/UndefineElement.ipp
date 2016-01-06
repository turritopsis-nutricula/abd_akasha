#include "type_fwd.hpp"
#include "detail/ElementBase.hpp"

#include "../element/ElementData.hpp"

#include <boost/type.hpp>

namespace elem_v2 = ::akasha::model::element_v2;

class elem_v2::UndefineElement :
	public elem_v2::detail::ElementBase<elem_v2::UndefineElement,
	boost::fusion::vector<>>
{
};

template<>
	elem_v2::UndefineElement*
elem_v2::MakeElement(
		CreateInfo const& d,
		boost::shared_ptr<akasha::NodeUpdateTask> const& task,
		boost::type< elem_v2::UndefineElement > const&)
{
	assert( false && "UndefineElement Uncreateable.");
	return nullptr;
}
