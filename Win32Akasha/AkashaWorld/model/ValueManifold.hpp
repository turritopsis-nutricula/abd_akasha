#ifndef VALUE_MANIFOLD__HPP
#define VALUE_MANIFOLD__HPP

#include "element/ElementData.hpp"
#include "element_v2/ElementConcept.hpp"
#include "ModelValue.hpp"
#include <boost/type_erasure/any.hpp>
#include <boost/type_erasure/member.hpp>
#include <boost/ref.hpp>
#include <boost/type_traits/remove_const.hpp>

	BOOST_TYPE_ERASURE_MEMBER((has_setUserValue), setUserValue, 1)
	BOOST_TYPE_ERASURE_MEMBER((has_setSign), setSign, 1)
	BOOST_TYPE_ERASURE_MEMBER((has_setElementConcept), setElementConcept, 1)
	BOOST_TYPE_ERASURE_MEMBER((has_setElementId), setElementId, 1)
BOOST_TYPE_ERASURE_MEMBER((has_sync), sync, 1)

	namespace akasha
{
namespace model
{

template<typename ParamType>
	class UserValueManifold
{
	typedef value::ModelValue<float, std::string> userValue_type;
	userValue_type* m_valueRef;
	int elementId_;
	bool m_minus;
public:

	void setUserValue(userValue_type& v)
	{
		m_valueRef = &v;
	}
	void setSign(bool t){ m_minus = t; }
	void setElementId(int id){ elementId_ = id; }
	void setElementConcept(element_v2::ElementConcept& e)
	{
		//m_element = e;
	}

	void sync(std::vector<element_v2::ElementConcept>& elements)
	{
		elements.at(elementId_).getParameterPtr()->getAttribute().setValue(
				boost::type<ParamType>(),
				m_valueRef->getVolum() *
				(m_minus ? -1.f : 1.f));

	}
};
typedef boost::type_erasure::any<
boost::mpl::vector<
boost::type_erasure::copy_constructible<>
, boost::type_erasure::assignable<>
,::has_setSign<void (bool)>
,::has_setUserValue<void (value::ModelValue<float, std::string>&)>
//,::has_setElementConcept<void (element_v2::ElementConcept&)>
, ::has_setElementId<void (int)>
,::has_sync<void (std::vector<element_v2::ElementConcept>&)>
>, boost::type_erasure::_self> ValueManifoldConcept;
//TODO : óvç«é¿ëï



}
}
#endif //VALUE_MANIFOLD__HPP
