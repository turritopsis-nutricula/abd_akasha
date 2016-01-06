#include "ElementChunkBuilder.hpp"

#include <boost/mpl/string.hpp>
#include <boost/utility/in_place_factory.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <boost/range/algorithm_ext/erase.hpp>


using namespace akasha;
using namespace akasha::model;
using akasha::model::builder::ElementChunkBuilder;
using akasha::model::builder::UserValueManifoldBuilder;
using ::akasha::model::element_v2::ElementBuffer;


//fwd
template<typename ElementType>
int BuildElement(
		ElementBuffer& result,
		loader::ModelElementDefine<std::string> const& data,
		element::ElementData const&,
		irrExt::IrrExtention&,
		wisp::v3::WispHandle& wisp,
		bool const isSystemAdded,
		boost::type<ElementType> const&);
template<typename ElementType>
int BuildElement_NameLess(
		ElementBuffer& result,
		loader::ModelElementDefine<std::string> const& data,
		element::ElementData const&,
		wisp::v3::WispHandle& wisp,
		boost::type<ElementType> const&);
template<typename ElementType>
ElementChunkBuilder::parentInfoType
MakeBuildQueue(
		ElementChunkBuilder::elementBuildQueue& result,
		ElementChunkBuilder::elementDefType const&,
		ElementChunkBuilder::parentInfoType const parentId,
		ElementChunkBuilder::StringType const&,
		bool isSystemAdded,
		boost::type<ElementType> const&
		);

/////////////////////////////////////////////////////////////////////////////////
/// ErrorMessagePost : Systemへ直接投げる
	template<typename T>
void ErrorMessagePost(
		T const& errorValue,
		::akasha::model::loader::LocationInfo const& info)
{
	auto fstr =
		boost::format("Invalid keyword : %1% line:%2% : %3%") %
		errorValue % info.line_ % info.column_;

	system::AkashaSystem::Instance().getLogger()(
			system::log::ModelFileWarning{ "", fstr.str()});

}
	template<typename T>
void DuplicationNameMessagePost(
		T const& errorValue,
		::akasha::model::loader::LocationInfo const& info)
{
	auto fstr =
		boost::format("Duplicate Value Name : %1% line:%2% : %3%") %
		errorValue % info.line_ % info.column_;

	system::AkashaSystem::Instance().getLogger()(
			system::log::ModelFileWarning{ "", fstr.str()});

}

namespace
{
template<typename ConnectType>
constexpr int GetDirectTypeNumber()
{
	return ConnectType().getTypeNumber();
}
}
//////////////////////////////////////////////////////////////////////////////////
//ConnectMapping
struct connectMappingBuilder
{
	ElementChunkBuilder::connectDirectMappingType& directMap_;

	template<typename ConnectType>
		void operator()(ConnectType const& connect) const
		{

			element_v2::DirectType d(
					element_v2::direct::GetConnectType< GetDirectTypeNumber<ConnectType>() >());
			directMap_[ element_v2::direct::GetTypeString(d) ] = d;
		}
};
////////////////////////////////////////////////////////////////////////////////
////ParameterMapping
struct attributeMappingBuilder
{
	ElementChunkBuilder::attributeFuncMapType& map_;
	template<typename ParamType>
		void operator()( ParamType const&) const
		{
			constexpr char const* name =  boost::mpl::c_str< typename ParamType::name >::value;
			map_[name] =
				[](
						element::ElementData& data,
						UserValueManifoldBuilder& manifoldBuilder,
						ElementChunkBuilder::nameAttributeBufType const& nameAttBuf,
						loader::ModelExpressionDefine<std::string> const& def)
				{

					if (def.isNumberValue())
					{
						// NumberValue assigen
						data.setParameterValue( boost::type<ParamType>(), *(def.getNumberValue()));
					}
					else
					{
						// UserValue or NameAttributeValue assigne
						std::string const& valueName(
								boost::fusion::at_c<1>(*(def.getStringValue()))
								);
						assert( manifoldBuilder.values_ );
						auto const& userValueList = *(manifoldBuilder.values_);
						auto value = boost::find_if(userValueList,
								[&valueName](value::ModelValue<float, std::string> const& userValue){
								return userValue.getName()==valueName;
								});
						auto nameAtt =nameAttBuf.find( boost::algorithm::to_lower_copy(valueName) );
						if (value != userValueList.cend())
						{
							data.setParameterValue(boost::type<ParamType>(), value->getVolum());
						}
						else if( nameAtt != nameAttBuf.cend())
						{
							data.setParameterValue(boost::type<ParamType>(), nameAtt->second);
						}
						else
						{
							ErrorMessagePost(valueName, def);
						}

					}
				};
		}
	void operator()( element::parameter::nameString const&) const
	{
		map_[ boost::mpl::c_str< typename element::parameter::nameString::name>::value ] =
			[](
					element::ElementData& data,
					UserValueManifoldBuilder&,
					ElementChunkBuilder::nameAttributeBufType const&,
					loader::ModelExpressionDefine<std::string> const& def)
			{
				if (!def.isNumberValue())
					data.setParameterValue(
							boost::type< element::parameter::nameString >(),
							boost::fusion::at_c<1>(*(def.getStringValue())));

			};
	}
};
//////////////////////////////////////////////////////////////////////////////////
//BuilderMapping
struct builderMappingBuilder
{
	ElementChunkBuilder::ElementBuildFuncMap& map_;
	ElementChunkBuilder::QueueMakeFuncMap& funcMap_;

	template<typename ElementType>
		void operator()(boost::type<ElementType> const& ) const
		{

			using boost::algorithm::to_upper_copy;

			constexpr int typeId = element_v2::TypeToId<ElementType>::value;
			constexpr bool isCreatable = element_v2::IsCreateble<typeId>();

			std::string const n( element_v2::IdToString< typeId >());
			std::string const name( to_upper_copy(n));

			map_[ name ] =
				boost::bind(BuildElement<ElementType>, _1,_2,_3,_4,_5,_6,boost::type<ElementType>());

			// BuildQueue not Createable
			if (isCreatable)
				funcMap_[ name ] =
					boost::bind(MakeBuildQueue<ElementType>, _1,_2,_3,name,false,boost::type<ElementType>());
		}
};
//////////////////////////////////////////////////////////////////////////////////
// ManifoldMapping
struct manifoldMappingBuilder
{
	ElementChunkBuilder::ManifoldBuildFuncMap& map_;
	template<typename ParamType>
		void operator()(ParamType const&) const
		{
			constexpr char const* name =  boost::mpl::c_str< typename ParamType::name >::value;
			map_[name] =
				[]( int const elementId,
						UserValueManifoldBuilder& manifoldBuilder,
						loader::ModelExpressionDefine<std::string> const& def)
				{

					if (!def.isNumberValue())
					{
						using boost::fusion::at_c;
						manifoldBuilder.add(
								at_c<1>(*(def.getStringValue())),
								at_c<0>(*(def.getStringValue())),
								elementId, boost::type<ParamType>());
					}
				};
		}

	void operator()( element::parameter::nameString const&) const
	{
		// name is not build manifold
	}
};

////////////////////////////////////////////////////////////
//nameAttributeBuffer
struct nameAttributeBuffer
{
	using StringType = ElementChunkBuilder::StringType;

	unsigned int count_;
	ElementChunkBuilder::nameAttributeBufType& result_;
	boost::ptr_vector<value::ModelValue<float, std::string>> const& userValuesRef_;

	nameAttributeBuffer( ElementChunkBuilder::nameAttributeBufType& r,
			boost::ptr_vector<value::ModelValue<float, std::string>> const& reference) :
		count_(0), result_(r), userValuesRef_(reference){}

	void operator()(
			ElementChunkBuilder::elementBuildQueue const& buildQuene)
	{
		for (auto const& info : buildQuene)
		{
			auto const& elemData = info.dataRef_;
			int const id( count_++ );

			// 自動挿入されたQueueは対象外
			if (info.isAdded_) continue;

			auto expr = boost::find_if(elemData.propaties_,
					[](akasha::model::loader::ModelExpressionDefine<StringType> const& e){
					return boost::algorithm::to_lower_copy(e.getName()) == "name";
					});

			if (expr == elemData.propaties_.cend()) continue;

			// string value
			if ( auto v = expr->getStringValue() )
			{
				auto& str = boost::fusion::at_c<1>(*v);
				boost::algorithm::to_lower(str); // case ignore

				auto i = result_.find( str );
				auto ui = boost::find_if(userValuesRef_,
						[&str](value::ModelValue<float, std::string> const& v){
						return boost::algorithm::to_lower_copy(v.getName()) == str;
						});

				if (i == result_.end() && ui == userValuesRef_.cend())
					result_.insert( std::make_pair(str, id));
				else
					DuplicationNameMessagePost(str, *expr);

			}
		} // for buildQueue
	}
};

//////////////////////////////////////////////////////////////////////////////////
/// class ElementBuilder
void ElementChunkBuilder::init()
{
	//ConnectType
	using direct_type = element_v2::DirectType;
	using enum_type = std::underlying_type<direct_type>::type;
	for (enum_type i=static_cast<enum_type>(direct_type::Undefine);
			i < static_cast<enum_type>(direct_type::Count); ++i )
	{
		direct_type const d( static_cast<direct_type>(i));
		m_directMapping[ element_v2::direct::GetTypeString(d) ] = d;
	}

	//parameterMapping
	attributeMappingBuilder pmb{ m_attrFuncMap};
	rice::mpl::TypeTupleFold<
		element::parameter::parameterTypeTuple>().accept(pmb);

	//ElementBuildFuncMap & BuildQueueFuncs
	builderMappingBuilder bmb{ m_builderMapping, m_queueMakeFuncs };
	rice::mpl::TypeTupleFold<
		element_v2::ElementTypeTuple>().accept(bmb);

	//ValueManifold
	manifoldMappingBuilder mmb{ m_manifoldMapping };
	rice::mpl::TypeTupleFold<
		element::parameter::parameterTypeTuple>().accept(mmb);


}

void ElementChunkBuilder::build(
		resultType& result,
		std::vector<ValueManifoldConcept>& maniResult,
		boost::ptr_vector<value::ModelValue<float, std::string>>& values,
		std::vector<elementDefType> const& data)
{
	m_manifoldBuilder.result_ =boost::ref(maniResult);
	m_manifoldBuilder.values_ =boost::ref(values);

	// MakeQueue
	m_buildQueue.clear();
	this->makeQueue(data, std::make_pair(-1,-1));

	//Name Attribute Buf
	m_nameUserValueBuffer.clear();
	nameAttributeBuffer nameBuf( m_nameUserValueBuffer , values);
	nameBuf( m_buildQueue );

	//TODO : UserValue vs NameAttribute
	// Build
	this->buildExecute( result);
}

void ElementChunkBuilder::makeQueue(
		std::vector< elementDefType> const& data,
		parentInfoType const parentId)
{
	for (auto const& d : data)
	{
		StringType name( d.second_ ?  *(d.second_) : d.first_);
		parentInfoType nextParentId;
		auto iter = m_queueMakeFuncs.find( boost::algorithm::to_upper_copy(name));
		if (iter != m_queueMakeFuncs.end())
			nextParentId = iter->second(m_buildQueue, d, parentId);
		else
		{
			//TODO : ErrorMessage
			ErrorMessagePost(name, d);
			nextParentId = parentId;
		}

		this->makeQueue(d.children_, nextParentId);
	}
}

void ElementChunkBuilder::buildExecute(resultType& result)
{
	using boost::algorithm::to_upper_copy;
	using boost::fusion::at_c;
	unsigned int count(0);

	for (auto const& info : m_buildQueue)
	{
		//エレメント構築データ
		int const id(count);
		auto const& data = info.dataRef_;

		element::ElementData ed( id, shapes_, data.getTypeName());

		//数字へキャスト可能なパラメータと
		//name項を構築データに記述
		for (auto const& exp :  data.propaties_) {
			StringType const filedName( boost::algorithm::to_lower_copy(exp.getName()));
			auto map = m_attrFuncMap.find(filedName);
			if ( map != m_attrFuncMap.end())
				map->second(ed, m_manifoldBuilder, m_nameUserValueBuffer, exp);
			else
				ErrorMessagePost(exp.getName(), exp);

		}

		//接続方向 該当がない場合connectType::Undefine
		boost::optional<StringType> const& second = data.getSecondField();

		if ( second.is_initialized() )
		{
			StringType const directName(
					boost::algorithm::to_upper_copy(data.first_));

			auto i = m_directMapping.find( directName );
			if (i != m_directMapping.end())
				ed.direct_ = i->second;
			else
				ErrorMessagePost(data.first_, data);
		}

		// Set parentId
		ed.parentId_ = ed.direct_ != element_v2::DirectType::Output ?
			info.parentInfo_.first : info.parentInfo_.second;


		//Colition Type
		if (modelType_ == ElementModel::ModelType::main)
		{
			ed.collisionGroup_ = COL_TYPE_MODEL;
			ed.collisionMask_ =
				COL_TYPE_SUB_MODEL_0 |
				COL_TYPE_LAND |
				COL_TYPE_TOY |
				COL_TYPE_BOM |
				COL_TYPE_AVATAR;
		}
		else
		{//SubModel
			ed.collisionGroup_ = COL_TYPE_SUB_MODEL_0;
			ed.collisionMask_ =
				COL_TYPE_MODEL |
				COL_TYPE_LAND |
				COL_TYPE_TOY |
				COL_TYPE_BOM |
				COL_TYPE_AVATAR;
		}

		StringType const& upCaseName =  info.name_; //to_upper_copy(name);

		auto i = m_builderMapping.find(upCaseName);

		if (i != m_builderMapping.end())
		{
			i->second(result, data, ed, m_irrEx, wisp::v3::CreateWispHandle(), info.isAdded_ );
			for (auto const& exp : data.propaties_)
			{
				StringType const filedName( boost::algorithm::to_lower_copy(exp.getName()));
				auto mm = m_manifoldMapping.find( filedName );
				if (mm != m_manifoldMapping.end())
					mm->second( id, m_manifoldBuilder,exp );
			}
		}
		else
		{
			StringType const defineName( data.second_ ?  *(data.second_) : data.first_);
			ErrorMessagePost(defineName, data);
		}

		// Current Id Count up
		++count;
	}
}

//////////////////////////////////////////////////////////////////////////////////
// BuildElement Function
	template<typename ElementType>
int BuildElement(
		ElementBuffer& result,
		loader::ModelElementDefine<std::string> const& data,
		element::ElementData const& ed,
		irrExt::IrrExtention& irrEx,
		wisp::v3::WispHandle& wisp,
		bool const isSystemAdded,
		boost::type<ElementType> const&)
{
	if ( !isSystemAdded )
		return result.add(
				element_v2::CreateInfo{ed, wisp, irrEx},
				boost::type<ElementType>());
	else
	{
		namespace param = akasha::model::element::parameter;
		auto newEd = ed;
		//name field erase
		newEd.setParameterValue(boost::type<param::nameString>(), std::string(""));
		return result.add(
				element_v2::CreateInfo{newEd, wisp, irrEx},
				boost::type< ElementType >());
	}
}

////////////////////////////////////////////////////////////////////////////////
// MakeBuildQuene
template<typename ElementType>
	ElementChunkBuilder::parentInfoType
MakeBuildQueue(
		ElementChunkBuilder::elementBuildQueue& result,
		ElementChunkBuilder::elementDefType const& dataPtr,
		ElementChunkBuilder::parentInfoType const parentInfo,
		ElementChunkBuilder::StringType const& typeName,
		bool const isSystemAdded,
		boost::type<ElementType> const&
		)
{
	using parentInfoType = ElementChunkBuilder::parentInfoType;

	constexpr int typeId = element_v2::TypeToId<ElementType>::value;

	constexpr int insertId = element_v2::InsertElementID<typeId>();
	constexpr int tailElementId = element_v2::TailElementID<typeId>();


	parentInfoType currentParent( parentInfo );

	// before insert element
	if (insertId  != -1)
	{
		using parentWrapType =
			typename element_v2::IdToType< insertId >::type;

		currentParent = MakeBuildQueue(
				result,
				dataPtr,
				parentInfo,
				element_v2::IdToString< insertId >(),
				true, // isSystemAdded
				boost::type<parentWrapType>()
				);
	}

	// current element
	result.emplace_back(typeName, dataPtr, currentParent, isSystemAdded);
	parentInfoType nextParentInfo((int)result.size()-1, (int)result.size()-1);

	// tail element
	if (tailElementId != -1)
	{
		using tailType =
			typename element_v2::IdToType< tailElementId >::type;

		auto info = MakeBuildQueue(
				result,
				dataPtr,
				nextParentInfo,
				element_v2::IdToString< tailElementId >(),
				true, // isSystemAdded
				boost::type<tailType>()
				);

		// Tail Connection
		nextParentInfo.second = info.first;
	}

	return nextParentInfo;
}

////////////////////////////////////////////////////////////////////////////////////
// class UserValueManifoldBuilder
	template<typename ParamType>
bool UserValueManifoldBuilder::add(
		std::string const& valueName,
		bool isMinus,
		int elementId,
		boost::type<ParamType> const&)
{
	assert( result_ && values_ );
	typedef value::ModelValue<float, std::string> value_type;
	std::vector<ValueManifoldConcept>& result = *result_;
	boost::ptr_vector<value_type>& values = *values_;

	auto i = boost::find_if(values, [&valueName](value_type& v){return v.getName()==valueName;});
	if (i!=values.end())
	{
		UserValueManifold<ParamType> m;
		m.setUserValue( *i );
		m.setSign(isMinus);
		m.setElementId( elementId );
		result.push_back( ValueManifoldConcept{ m });
		return true;
	}
	else
	{
		return false;
	}

}

