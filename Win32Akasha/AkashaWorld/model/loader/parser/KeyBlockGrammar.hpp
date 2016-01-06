/*
 * KeyBlockGrammar.hpp
 *
 *  Created on: 2011/12/25
 *      Author: ely
 */

#ifndef KEYBLOCKGRAMMAR_HPP_
#define KEYBLOCKGRAMMAR_HPP_

#include "commentSkipper.hpp"
#include "ExpressionGrammar.hpp"


namespace akasha
{
namespace model
{
namespace loader
{

template<typename Iterator, typename StringType>
	class KeyBlockGrammar :
		public qi::grammar< Iterator, std::vector<ModelKeyDefine<StringType> > (), CommentSkipper<Iterator> >
	{
		typedef CommentSkipper<Iterator> skipper_type;
		typedef ModelKeyDefine<StringType> define_type;

		qi::rule<Iterator, typename KeyBlockGrammar::start_type::attr_type (), skipper_type> start_;
		qi::rule<Iterator, define_type (), skipper_type> keyControl_;
		qi::rule<
			Iterator,
			boost::fusion::vector<StringType, std::vector<ModelExpressionDefine<StringType>>> (),
			skipper_type> valueUnit_;
		RcdExpressionGrammar<Iterator, StringType> expr_;
		RcdValueName<Iterator, StringType> valueName_;

		boost::phoenix::function< annotation_f<Iterator>> annotate;

		typename skipper_type::skipperSwitch& switch_;

	public:
		KeyBlockGrammar(skipper_type& s, Iterator fileFirst) :
			KeyBlockGrammar::base_type(start_),
			expr_(fileFirst),
			annotate(fileFirst),
			switch_(s.switch_)
		{

			switch_.spaceSkip(true);

			//valueUnit_ = +(qi::alnum || qi::char_('_')) >> qi::lit('(') >>  (expr_ % ',') >> qi::lit(')');
			valueUnit_ = valueName_ >> qi::lit('(') >>  (expr_ % ',') >> qi::lit(')');
			keyControl_ = qi::uint_ >>  qi::lit(':') >> (valueUnit_ % ',');
			start_ = *(keyControl_);

			auto locationInfo = annotate(qi::_val, qi::_1, qi::_3);
			qi::on_success(keyControl_, locationInfo);
		}
	};
}
}
}


#endif /* KEYBLOCKGRAMMAR_HPP_ */
