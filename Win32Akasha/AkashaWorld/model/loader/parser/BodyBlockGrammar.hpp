/*
 * BodyBlockGrammar.hpp
 *
 *  Created on: 2011/12/25
 *      Author: ely
 */

#include "commentSkipper.hpp"
#include "ExpressionGrammar.hpp"

#ifndef BODYBLOCKGRAMMAR_HPP_
#define BODYBLOCKGRAMMAR_HPP_


namespace akasha
{
namespace model
{
namespace loader
{
template<typename Iterator, typename StringType>
	class BodyBlockGrammar : public qi::grammar<Iterator,
	std::vector< ModelElementDefine<StringType> > (),
	CommentSkipper<Iterator>>
	{
		typedef CommentSkipper<Iterator> skipper_type;
		typename BodyBlockGrammar::start_type start_;


		// element unit rule
		qi::rule<Iterator, ModelElementDefine<StringType> (), skipper_type> element_;
		// expression rule
		RcdExpressionGrammar<Iterator, StringType> expr_;
		// property rule
		typedef std::vector<ModelExpressionDefine<StringType>> propaty_rule;
		qi::rule<Iterator, propaty_rule (), skipper_type>  proparty_;

		boost::phoenix::function< annotation_f<Iterator> > annotate;

	public:
		BodyBlockGrammar(CommentSkipper<Iterator>& skipper, Iterator fileFirst) :
			BodyBlockGrammar::base_type(start_),
			expr_(fileFirst),
			annotate( fileFirst )
		{
			skipper.switch_.spaceSkip(true);

			proparty_ =  '(' >> -(expr_ % qi::lit(',')) >> ')';
			element_ =
				(+ (qi::alnum | qi::char_('_'))) >> // first word
				-( qi::omit[qi::lit(':')] >> + (qi::alnum | qi::char_('_'))) >> // second word
				proparty_ >> qi::lit('{') >> *element_ >> qi::lit('}');;

			start_ = +(element_);

			auto locationInfo = annotate( qi::_val, qi::_1, qi::_3);
			qi::on_success( element_, locationInfo);

			proparty_.name("exp");
			element_.name("element");

			// TODO:デバック表示
			//            qi::debug(proparty_);
			//            qi::debug(element_);


		}

	};
}
}
}

#endif /* BODYBLOCKGRAMMAR_HPP_ */
