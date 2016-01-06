/*
 * ExpressionGrammar.hpp
 *
 *  Created on: 2011/12/24
 *      Author: ely
 */

#ifndef EXPRESSIONGRAMMAR_HPP_
#define EXPRESSIONGRAMMAR_HPP_

#include "../define/ModelDefineStruct.hpp"
#include "annotation.hpp"

#include "commentSkipper.hpp"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi_no_case.hpp>
#include <boost/spirit/include/qi_omit.hpp>
#include <boost/fusion/include/struct.hpp>


namespace akasha
{
namespace model
{
namespace loader
{


namespace detail
{
//! RCDファイル右辺値
template<typename Iterator, typename ValueType>
	class RcdRightValueGrammarImpl_ :
		public qi::grammar<
		Iterator,
		typename ValueType::result_type (),
		CommentSkipper<Iterator>
		>
	{
		typedef typename ValueType::result_type value_type;
		qi::rule<Iterator, value_type (), CommentSkipper<Iterator>> value_;
		const char* spilitChar_;
	public:
		//TODO: 区切り文字の変数化
		explicit RcdRightValueGrammarImpl_() :
			RcdRightValueGrammarImpl_::base_type(value_)
			//,spilitChar_(spit)
		{
			value_ =
				( &(-qi::lit('-') >> qi::digit) >> qi::float_) |
				( qi::matches['-'] >>
				  (   (qi::lit('#') >> qi::hex) | +((qi::alnum | qi::char_('_')) -qi::lit(','))) );
		}
	};
}

template<typename StringType>
struct RcdRightValueType
{
	typedef float number_type;
	typedef unsigned int hex_type;
	typedef StringType string_type;
	typedef boost::fusion::vector< bool, boost::variant<hex_type, string_type>> non_number_variant_type;

	typedef boost::variant<number_type, non_number_variant_type> result_type;

	//typedef ModelExpressionDefined<StringType> result_type;
};
template<typename Iterator, typename String>
struct RcdRightValueGrammar :
	public detail::RcdRightValueGrammarImpl_<Iterator, RcdRightValueType<String> >
{
public:
	typedef typename RcdRightValueType<String>::result_type result_type;
	typedef typename RcdRightValueType<String>::number_type number_type;
	typedef typename RcdRightValueType<String>::non_number_variant_type non_number_variant_type;
};

//! Rcd変数式
template<typename Iterator, typename String>
class RcdExpressionGrammar :
	public qi::grammar<
	Iterator,
	ModelExpressionDefine<String> (),
	CommentSkipper<Iterator>
	>
{

	RcdRightValueGrammar<Iterator, String> rValue_;
	qi::rule<Iterator,
		typename RcdExpressionGrammar::start_type::attr_type (),
		CommentSkipper<Iterator>> expression_;

	boost::phoenix::function<annotation_f<Iterator>> annotate;

public:
	RcdExpressionGrammar(Iterator fileFirst) :
		RcdExpressionGrammar::base_type(expression_),
		annotate( fileFirst)
	{
		//expression_ = +((qi::alnum || '_') -'=') >> qi::omit[*qi::space] >> '=' >> qi::omit[*qi::space] >> rValue_;
		expression_ = +qi::alnum  >> qi::omit[*qi::space] >> '=' >> qi::omit[*qi::space] >> rValue_;

		auto locationInfo = annotate( qi::_val, qi::_1, qi::_3);
		qi::on_success(expression_, locationInfo);
	}
};


//! RcdVal変数名
template<typename Iterator, typename String>
class RcdValueName : public qi::grammar<Iterator, String ()>
{
	qi::rule<Iterator, String ()> name_;
public:
	explicit RcdValueName() : RcdValueName::base_type(name_)
	{
		name_ = +(qi::alnum | qi::char_('_'));
	}
};

}
}
}


#endif /* EXPRESSIONGRAMMAR_HPP_ */
