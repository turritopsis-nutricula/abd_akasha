#ifndef ELEMENT_DATA__HPP
#define ELEMENT_DATA__HPP

#include "../builder/BtShapeBuilder.hpp"
//wisp
#include <wisp_v3/wisp.hpp>

#include "ConnectType.hpp"

#include <Riceball/mpl/EasyTuple.hpp>

#include <boost/variant.hpp>
#include <boost/optional.hpp>

//#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
//#define BOOST_MPL_LIMIT_VECTOR_SIZE 10
//#define BOOST_MPL_LIMIT_LIST_SIZE 15
#include <boost/mpl/string.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/list.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/at.hpp>

//#define FUSION_MAX_LIST_SIZE 15
#include <boost/fusion/include/vector.hpp>
//#define FUSION_MAX_MAP_SIZE 15

#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/value_at_key.hpp>
#include <boost/fusion/include/mpl.hpp>

namespace akasha
{
namespace model
{
namespace element
{


//パラメータタイプ
namespace parameter
{
struct angle
{
	typedef boost::mpl::string<'a', 'n', 'g', 'l', 'e'> name;
	typedef float holderType;
};
struct brake
{
	typedef boost::mpl::string<'b', 'r', 'a', 'k', 'e'> name;
	typedef float holderType;
};
struct color
{
	typedef boost::mpl::string<'c', 'o', 'l', 'o', 'r'> name;
	typedef unsigned int holderType;
};
struct damper
{
	typedef boost::mpl::string<'d', 'a', 'm', 'p', 'e', 'r'> name;
	typedef float holderType;
};
struct effect
{
	typedef boost::mpl::string<'e', 'f', 'f', 'e', 'c', 't'> name;
	typedef float holderType;
};

struct option
{
	typedef boost::mpl::string<'o', 'p', 't', 'i', 'o', 'n'> name;
	typedef float holderType;
};
struct spring
{
	typedef boost::mpl::string<'s', 'p', 'r', 'i', 'n', 'g'> name;
	typedef float holderType;
};
struct size
{
	typedef boost::mpl::string<'s','i','z','e'> name;
	typedef float holderType;
};
struct power
{
	typedef boost::mpl::string<'p', 'o', 'w', 'e', 'r'> name;
	typedef float holderType;
};
struct nameString
{
	typedef boost::mpl::string<'n', 'a', 'm', 'e'> name;
	typedef std::string holderType;
};
struct link
{
	typedef boost::mpl::string<'l', 'i', 'n', 'k'> name;
	typedef int holderType;
};
struct linkNumber
{
	typedef boost::mpl::string<'l','i','n','k','n','u','m'> name;
	typedef int holderType;
};
using  parameterVariant = boost::variant<
std::string,  //無効なパラメータ名の場合
	angle, brake, color, damper, effect, option, size,spring,
	power, nameString, link,linkNumber>;
using parameterTypeList = boost::mpl::list<angle, brake, color, damper, effect,
		option, size,spring, power, nameString,link,linkNumber>;
using parameterTypeTuple = rice::mpl::TypeTuple<angle, brake, color, damper,
		effect, option, size,spring, power, nameString,link,linkNumber>;



namespace detail
{

struct rootTest {
	//typedef boost::fusion::vector<> res_type;
	typedef boost::mpl::vector<> res_type;
};
template<typename Base, typename Leaf>
struct node
{
	typedef typename boost::mpl::deref<Leaf>::type elem_type;
	typedef typename elem_type::holderType hold_type;
	typedef typename boost::fusion::result_of::make_pair<elem_type, hold_type>::type pair_type;

	typedef typename boost::mpl::push_back<typename Base::res_type, pair_type>::type res_type;
};

//optional付き
template<typename Base, typename Leaf>
struct optionalNode
{
	typedef typename boost::mpl::deref<Leaf>::type elem_type;
	typedef typename elem_type::holderType hold_type;
	typedef typename boost::fusion::result_of::make_pair<elem_type, boost::optional<hold_type>>::type pair_type;

	typedef typename boost::mpl::push_back<typename Base::res_type, pair_type>::type res_type;
};


//Nodetypeパラメータ
template<typename TList, template<class,class>class NodeType >
struct GGen
{
	typedef typename boost::mpl::iter_fold<
		TList,rootTest,NodeType<boost::mpl::_1, boost::mpl::_2 > >::type iterType;
	typedef typename boost::fusion::result_of::as_map<
		typename iterType::res_type>::type type;

	type param_;



	template<typename T>
		typename boost::fusion::result_of::value_at_key<type, T>::type
		getValue(const boost::type<T>&) const
		{
			return boost::fusion::at_key<T>(param_);
		}

	template<typename T>
		void
		setValue(const boost::type<T>&, const typename boost::fusion::result_of::value_at_key<type, T>::type v)
		{
			boost::fusion::at_key<T>(param_) = v;
		}
	const type& getFusion() const
	{
		return param_;
	}
	type& getFusion()
	{
		return param_;
	}

};

typedef GGen<parameterTypeList, node > normalType;
typedef GGen<parameterTypeList, optionalNode > optionalType;
}

///boost::optionalを取り去るヘルパー
/** 無効値であった場合一緒に渡したdefaultの値が適用される*/
template<typename LHMap>
struct deOptional
{
	LHMap& lh_;
	LHMap& defaultMap_;

	deOptional(LHMap& l, LHMap& d) : lh_(l), defaultMap_(d){ }

	template<typename RHMap>
		void operator ()(const RHMap& rh) const
		{
			typedef typename boost::fusion::result_of::first<RHMap>::type keyType;
			auto& lValue = boost::fusion::at_key< keyType >(lh_);

			if (rh.second)
				lValue = rh.second.get();
			else
				lValue = boost::fusion::at_key< keyType >(defaultMap_);
		}
};

///受け渡し用
//TODO : 型名を帰る
typedef detail::normalType elementParameter;
typedef detail::optionalType fileData;

}  //namespace parameter


//////////////////////////////////////////////////////////////////
// エレメント構成データ

struct ElementData
{
	typedef std::string stringType;

	//shotBaseのために残存
	builder::BtShapeBuilder& shapeCacher_;

	stringType elementName_;

	element_v2::DirectType direct_;

	unsigned int id_;  //!< id

	short int collisionGroup_;
	short int collisionMask_;  //!< 衝突判定

	//parameter::parameters params_;  //! パラメータ
	parameter::fileData params_;

	int parentId_;

	ElementData() = delete;

	ElementData(
			unsigned int id,
			builder::BtShapeBuilder& c,
			stringType const& n) :
		shapeCacher_(c),
		elementName_(n),
		direct_( element_v2::DirectType::Undefine ),
		id_(id),
		collisionGroup_(1),
		collisionMask_(-1)
	{
	}

	//! パラメータ参照
	const parameter::fileData&
		getParameters() const
		{
			return params_;
		}
	/// パラメータ値を取得
	template<typename P>
		typename boost::fusion::result_of::value_at_key<typename parameter::fileData::type, P>::type
		getParameterValue(boost::type<P> const&) const
		{
			return params_.getValue(boost::type<P>());
		}
	template<typename P>
		void
		setParameterValue(boost::type<P> const&,const typename boost::fusion::result_of::value_at_key<parameter::fileData::type, P>::type& v)
		{
			params_.setValue(boost::type<P>(), v);
		}


	template<typename ShapeTag>
		boost::shared_ptr<btCollisionShape>
		getBtShape(boost::type<ShapeTag> const&, builder::shapeBase::ShapeParameter const& p) const
		{
			return shapeCacher_.getShape(
					boost::type<ShapeTag>(),
					p
					);
		}

};
}
}
}

#endif
