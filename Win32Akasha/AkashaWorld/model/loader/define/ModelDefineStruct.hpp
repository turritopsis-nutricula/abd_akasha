/*
 * ModelDefineStruct.hpp
 *
 *  Created on: 2011/12/25
 *      Author: ely
 */

#ifndef MODELDEFINESTRUCT_HPP_
#define MODELDEFINESTRUCT_HPP_

#include <boost/variant.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/at_c.hpp>
#include <boost/optional.hpp>
#include <boost/fusion/include/make_vector.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>

#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/serialization/variant.hpp>

//#include <boost/fusion/include/

namespace boost
{
namespace serialization
{
template<typename Archive, typename StringType>
	void
	serialize(Archive& ar, boost::fusion::vector<bool, boost::variant<unsigned int, StringType> >& v, unsigned int const var )
{
	ar & boost::fusion::at_c<0>(v) & boost::fusion::at_c<1>(v);
}
}
}
namespace akasha
{
namespace model
{
namespace loader
{
struct LocationInfo {
	unsigned int line_, column_, length_;
};


template<typename String>
struct ModelExpressionDefine : LocationInfo
{

	//struct fileRange{ typename String::iterator start_, end_;};

	typedef String string_type;
	typedef boost::variant<
		float,
		boost::fusion::vector<bool,
		boost::variant<unsigned int, string_type> > > adaptType;
	typedef boost::fusion::vector<bool, string_type> string_return_type;
	typedef boost::variant<float, string_return_type> return_type;


	string_type name_;
	adaptType value_;

	const string_type&
		getName() const
		{
			return name_;
		}

	void
		getValue(return_type& r) const
		{
			if (value_.which() == 0)
			{
				r = boost::get<float>(value_);
			}
			else
			{
				typedef boost::fusion::vector<bool,
						  boost::variant<unsigned int, string_type>> innerType;
				const auto& i = boost::get<innerType>(value_);
				if (boost::fusion::at_c<1>(i).which() == 0)
					r = (boost::fusion::at_c<0>(i) ? -1.f : 1.f)
						* boost::get<unsigned int>(boost::fusion::at_c<1>(i));
				else
				{
					r = string_return_type(
							boost::fusion::at_c<0>(i) ,
							boost::get<string_type>( boost::fusion::at_c<1>(i) )
							);
				}
			}
		}

	bool
		isNumberValue() const
		{
			typedef boost::fusion::vector<bool,
					  boost::variant<unsigned int, string_type>> innerType;
			return value_.which() == 0 ||
				boost::fusion::at_c<1>( boost::get<innerType>(value_) ).which()==0;
		}
	boost::optional<float>
		getNumberValue() const
		{
			return_type r;
			getValue(r);
			if (isNumberValue())
				return boost::get<float>(r);
			else
				return boost::none;
		}
	boost::optional<boost::fusion::vector<bool, string_type>>
		getStringValue() const
		{
			return_type r;
			getValue(r);
			if (!isNumberValue())
				return boost::get<string_return_type>(r);
			else
				return boost::none;
		}

	std::string getValueRawString() const
	{
		return_type r;
		getValue( r );
		if (!isNumberValue())
		{
			auto const& value = boost::get<string_return_type>(r);
			return
				(boost::format("%1%%2%") %
				 (boost::fusion::at_c<0>(value) ? "-"  : "") %
				 boost::fusion::at_c<1>(value)).str();
		}
		else
		{
			return (boost::format("%1%") % boost::get<float>(r)).str();
		}
	}

private:
	friend class boost::serialization::access;
	template<typename Archive>
		void
		serialize(Archive& ar, unsigned int const var)
		{
			ar & name_ & value_;
		}
};

}

}
}
template<typename S>
	std::ostream&
operator<<(std::ostream& os,
		const akasha::model::loader::ModelExpressionDefine<S>& o)
{
	os << o.name_ << "=" << o.getValueRawString();
	return os;
}

namespace akasha
{
namespace model
{
namespace loader
{
template<typename StringType>
	struct ModelValueDefine : LocationInfo
{
	typedef StringType string_type;
	string_type valname_;
	std::vector<ModelExpressionDefine<StringType> > propatyes_;
};

}
}
}
template<typename S>
	std::ostream&
operator<<(std::ostream& os,
		const akasha::model::loader::ModelValueDefine<S>& o)
{
	os << o.valname_ << "(";
	BOOST_FOREACH(const auto& v, o.propatyes_)
	{
		os << v << " ";
	}
	os << ")";
	return os;
}

namespace akasha
{
namespace model
{
namespace loader
{
template<typename StringType>
	struct ModelKeyDefine : LocationInfo
{

	unsigned int keyNumber_;
	typedef boost::fusion::vector<StringType,
			  std::vector<ModelExpressionDefine<StringType> >> control_type;
	std::vector<control_type> controlValues_;
};
}
}
}
template<typename S>
	std::ostream&
operator<<(std::ostream& os,
		const akasha::model::loader::ModelKeyDefine<S>& o)
{
	os << o.keyNumber_ << " : ";
	BOOST_FOREACH(const auto& v, o.controlValues_)
	{
		os << boost::fusion::at_c<0>(v) << '(';
		BOOST_FOREACH(const auto& u, boost::fusion::at_c<1>(v) )
			os << u << " ";
		os << ')';
	}

	return os;
}

namespace akasha
{
namespace model
{
namespace loader
{
template<typename StringType>
	struct ModelElementDefine : LocationInfo
{
	StringType first_;
	boost::optional<StringType> second_;
	typedef ModelExpressionDefine<StringType> propaty_type;
	std::vector<propaty_type> propaties_;
	std::vector<ModelElementDefine<StringType>> children_;

	boost::optional<unsigned int> elementID_;


public:

	const StringType&
		getTypeName() const
		{
			if (const auto s = second_)
				return *s;
			else
				return first_;
		}

	boost::optional<StringType>
		getDirectName() const
		{
			//TODO : å¥àˆïsñæÇÃoptionalì‡Ç≈assert
			if (second_)
				return first_;
			else
				return boost::none;
		}

	const boost::optional<StringType>&
		getSecondField() const
		{
			//TODO : Å™Ç™égÇ¶Ç»Ç¢ÇΩÇﬂãŸã}ë[íu
			return second_;
		}

	unsigned int getId() const
	{
		assert(elementID_);
		return *elementID_;
	}
	void IDCountup(unsigned int& i)
	{
		elementID_ = i++;
		BOOST_FOREACH(auto& d, children_)
			d.IDCountup(i);
	}
private:
	friend class boost::serialization::access;
	template<typename Archive>
		void
		serialize(Archive& ar, unsigned int const var)
		{
			ar & first_ & second_ & propaties_ & children_ & elementID_;
		}
};
}
}
}
template<typename S>
	std::ostream&
operator<<(std::ostream& os,
		const akasha::model::loader::ModelElementDefine<S>& o)
{

	os << o.first_;
	if (auto s = o.second_)
	{
		os << "=:=" << *s;
	}
	os << "(";
	BOOST_FOREACH(const auto& v, o.propaties_)
	{
		os << v << " ";
	}
	os << "){";
	BOOST_FOREACH(const auto& v, o.children_)
	{
		os << "\n" << v;
	}
	os << "}";

	return os;
}

namespace akasha
{
namespace model
{
namespace loader
{
template<typename StringType>
	struct ModelDefine
{
	std::vector< ModelValueDefine<StringType>>  values_;
	std::vector< ModelKeyDefine<StringType>>     keys_;
	std::vector< ModelElementDefine<StringType>> elements_;
	StringType luaCode_;
	StringType modelFileFullPath_;
};
}
}
}
#endif /* MODELDEFINESTRUCT_HPP_ */
