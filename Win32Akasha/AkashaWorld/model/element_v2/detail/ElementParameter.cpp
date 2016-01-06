#include "ElementParameter.hpp"
#include "AdapterParameter.hpp"

#include <boost/fusion/include/for_each.hpp>

namespace elem = akasha::model::element_v2;
namespace param = akasha::model::element::parameter;

void elem::ElementParameter::init(int const typeId,bool const isAdapter, CreateInfo const& info)
{
	m_typeId = typeId;
	m_connectStatus = false;

	if (isAdapter)
		m_adapterParam = boost::make_shared<elem::AdapterParameter>();

	element::ElementData const& data = info.data_;
	m_id = data.id_;
	m_parentId = data.parentId_;
	this->damage(1000);
	m_connectDirect = data.direct_;
	m_connectStatus = m_parentId != -1;

	if (auto const& n = data.getParameterValue( boost::type<param::nameString>() ) )
		m_nameString = *n;

	//デフォルト値の設定
	param::elementParameter defMap;
	defMap.setValue( boost::type<param::angle>(), 0.f );
	defMap.setValue( boost::type<param::color>(), 0xFFFFFF );
	defMap.setValue( boost::type<param::spring>(), 1.0f );
	defMap.setValue( boost::type<param::damper>(), 0.5f );
	defMap.setValue( boost::type<param::link>(), -1 );
	defMap.setValue( boost::type<param::linkNumber>(), 0 );

	typedef typename param::elementParameter::type mapType;
	boost::fusion::for_each(
			data.params_.getFusion(),
			param::deOptional<mapType>(
				boost::ref( m_parametors.getFusion() ),
				defMap.getFusion() ) );

}
