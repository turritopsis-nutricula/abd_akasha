/*
 * commentSkipper.hpp
 *
 *  Created on: 2011/12/24
 *      Author: ely
 */

#ifndef COMMENTSKIPPER_HPP_
#define COMMENTSKIPPER_HPP_

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi_no_case.hpp>
#include <boost/spirit/include/qi_omit.hpp>
#include <boost/fusion/include/struct.hpp>
#include <boost/bind.hpp>
#include <boost/logic/tribool.hpp>


namespace akasha
{
namespace model
{
namespace loader
{
namespace qi = boost::spirit::qi;
namespace fusion = boost::fusion;
namespace logic = boost::logic;

//! Rcdファイルコメント
template<typename Iterator>
	class CommentSkipper : public qi::grammar<Iterator>
{

public:
	struct skipperSwitch
	{
		//boost::function< void (bool)> func_;
		boost::function< void (logic::tribool, logic::tribool)> func_;
		void call(bool doluaComment) const
		{
			func_(doluaComment, logic::indeterminate);
		}
		void luaComment() const
		{
			func_(true, logic::indeterminate);
		}
		void rcdComment() const
		{
			func_(false, logic::indeterminate);
		}
		void spaceSkip(bool b) const
		{
			func_(logic::indeterminate, b);
		}
	};

private:
	qi::rule<Iterator> entity_;
	qi::rule<Iterator> comment_;


public:
	skipperSwitch switch_;
	bool spaceSkip_;

public:
	explicit CommentSkipper() :
		CommentSkipper::base_type(comment_),
		spaceSkip_(false)
	{
		switch_.func_ = boost::bind(&CommentSkipper::switchType, this, ::_1, ::_2 );
		entity_ = qi::lit("//") >> *(qi::char_ - '\n') >> '\n';
		comment_ = entity_;
	}


	void switchType(logic::tribool doLuaComment, logic::tribool doSpace)
	{
		if(doLuaComment) //luaコメントスキッパーセット
			entity_ = ("--[[" >> *(qi::char_ - qi::lit("]]")) >> "]]") |
				("--" >> !(qi::lit("[[")) >> *(qi::char_ - '\n') >> '\n');
		else //Rcdファイルスキッパーセット
			entity_ = qi::lit("//") >> *(qi::char_ - '\n') >> '\n';

		if (doSpace)
			spaceSkip_ = true;
		else
			spaceSkip_ = false;

		if (spaceSkip_)
			comment_ =  entity_|qi::space;
		else
			comment_ = entity_;
	}

};

}
}
}


#endif /* COMMENTSKIPPER_HPP_ */
