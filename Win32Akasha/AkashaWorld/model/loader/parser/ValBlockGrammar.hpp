/*
 * ValBlockGrammar.hpp
 *
 *  Created on: 2011/12/25
 *      Author: ely
 */

#ifndef VALBLOCKGRAMMAR_HPP_
#define VALBLOCKGRAMMAR_HPP_

#include "commentSkipper.hpp"
#include "ExpressionGrammar.hpp"
#include "annotation.hpp"

#include <boost/spirit/include/phoenix.hpp>

namespace akasha
{
namespace model
{
namespace loader
{

//! ValBlock
template<typename Iterator, typename String>
	class ValBlockGrammar :
		public qi::grammar<
		Iterator,std::vector<ModelValueDefine<String> >(), CommentSkipper<Iterator> >
	{

		typedef RcdRightValueGrammar<Iterator, String> rValue_rule_type;
		typedef typename rValue_rule_type::result_type rValue_result_type;

		typedef CommentSkipper<Iterator> skipper_type;
		typedef RcdExpressionGrammar<Iterator, String> expression_type;
		typedef typename expression_type::start_type::attr_type expression_result_type;

		expression_type expr_;
		RcdValueName<Iterator, String> valueName_;
		qi::rule<Iterator, std::vector<expression_result_type>(), skipper_type> exprs_;

		typedef ModelValueDefine<String> define_type;
		qi::rule<Iterator, define_type(), skipper_type> valDef_;
		qi::rule<Iterator, std::vector<define_type>(), skipper_type> start_;

		boost::phoenix::function<annotation_f<Iterator>> annotate;

	public:
		ValBlockGrammar(CommentSkipper<Iterator>& skip, Iterator fileFirst) :
			ValBlockGrammar::base_type(start_), expr_(fileFirst),annotate(fileFirst)
		{
			skip.switch_.spaceSkip(true);

			exprs_ = expr_ % ',';
			valDef_ = valueName_ >> '(' >> exprs_ >> ')';
			start_ = qi::omit[*qi::space] >> *(valDef_ >> qi::omit[*qi::space]);

			auto locationInfo = annotate(qi::_val, qi::_1, qi::_3);
			qi::on_success(valDef_, locationInfo);
			//>> qi::eoi;

			valDef_.name("valdef");
			//qi::debug(valDef_);
		}

	};

}
}
}
#endif /* VALBLOCKGRAMMAR_HPP_ */
