/*
 * BlockSplitGrammar.hpp
 *
 *  Created on: 2011/12/24
 *      Author: ely
 */

#ifndef BLOCKSPLITGRAMMAR_HPP_
#define BLOCKSPLITGRAMMAR_HPP_

#include "commentSkipper.hpp"


namespace akasha
{
namespace model
{
namespace loader
{

template<typename Iterator,typename String>
	struct RcdFileBlockType
{
public:
	typedef String string_type;
	typedef boost::fusion::vector< string_type, boost::iterator_range<Iterator>>   block_type;
	typedef boost::iterator_range<Iterator>                                   luaBlock_type;
	typedef boost::variant<luaBlock_type, block_type>                         value_type;
	typedef std::vector< value_type >                                         result_type;
};

template <typename Iterator,typename String>
class BlockSplitGrammar :
	public qi::grammar<
	Iterator,
	typename RcdFileBlockType< Iterator, String>::result_type (),
	CommentSkipper<Iterator>
	>
{
private:
	typedef RcdFileBlockType<Iterator, String> TypeHolder;
public:
	typedef typename TypeHolder::string_type string_type;
	typedef typename TypeHolder::block_type block_type;
	typedef typename TypeHolder::luaBlock_type luaBlock_type;
	typedef typename TypeHolder::value_type value_type;
	typedef typename TypeHolder::result_type result_type;

	typedef CommentSkipper<Iterator> skipper_type;
	typedef typename skipper_type::skipperSwitch switch_type;
private:
	switch_type& switch_;
	qi::rule<Iterator, string_type (), CommentSkipper<Iterator> > brace_;
	qi::rule<Iterator, luaBlock_type (), CommentSkipper<Iterator>  >    lua_block_;
	qi::rule<Iterator, block_type (), CommentSkipper<Iterator> >    other_block_;
	qi::rule<Iterator, result_type (), CommentSkipper<Iterator>>    start_;


public:
	explicit BlockSplitGrammar(CommentSkipper<Iterator>& skipper) :
		BlockSplitGrammar::base_type(start_),
		switch_(skipper.switch_)
	{
		brace_ = *(qi::char_-'{'-'}') >>
			*( '{' >> brace_ >> '}' >> *(qi::char_-'{'-'}') );
		//TODO: ブロックの閉じ間際にコメントがあるとスキップされたイテレータが入ってしまう
		lua_block_ = qi::no_case[qi::lit("LUA")] >>
			//qi::no_case[qi::lit("LUA") ] >>
			qi::omit[ *qi::space  >>
			qi::lit('{')[ boost::bind(&switch_type::luaComment, &switch_) ] ] >>
			qi::raw[ brace_ ] >>
			qi::omit[ qi::lit('}')[ boost::bind(&switch_type::rcdComment, &switch_) ]];
		other_block_ = *(qi::alpha) >>
			qi::omit[*qi::space  >>
			qi::lit('{')[ boost::bind(&switch_type::rcdComment, &switch_) ] ] >>
			qi::raw[ brace_ ] >> '}';
		start_ = qi::omit[*qi::space] >> *( (lua_block_ | other_block_) >> qi::omit[*qi::space]);

		start_.name("start");
		other_block_.name("block");
		lua_block_.name("luaBlock");
		brace_.name("brance");

		//qi::debug(start_);
		//          qi::debug(other_block_);
		//          qi::debug(lua_block_);
		//          qi::debug(brace_);

	}
};




}
}
}



#endif /* BLOCKSPLITGRAMMAR_HPP_ */
