#ifndef AKASHA_MODEL_LOADER_PARSER_ANNOTATION_HPP_
#define AKASHA_MODEL_LOADER_PARSER_ANNOTATION_HPP_

#include "../define/ModelDefineStruct.hpp"
#include <algorithm>

namespace  {
template<typename Iterator>
	unsigned int get_sentence_line(Iterator const& first,Iterator const& pos)
	{
		bool BufCR = false;

		return std::accumulate( first, pos, 1,
				[&BufCR](unsigned int size,typename Iterator::value_type const& c){
				bool hit = (c == '\r' ||( !BufCR && c=='\n'));
				BufCR = c=='\r';
				return hit ? ++size : size;
				});

	}

template<typename Iterator>
	unsigned int get_sentence_column(Iterator const& first, Iterator const& pos)
	{

		Iterator i = pos;
		for(; i != first; --i)
			if (*i=='\n' || *i=='\r')
			{
				++i;
				break;
			}
		return std::distance(i, pos)+1;
	}
} // namespace

namespace akasha {
namespace model {
namespace loader {

template<typename Iterator>
	struct annotation_f {
		typedef void result_type;

		annotation_f(Iterator first) : first_(first){}
		Iterator const first_;

		template<typename ValType, typename FirstType, typename LastType>
			void operator()(ValType& v, FirstType& first, LastType& last) const{
				do_annotate(v,first, last, first_);
			}
	private:
		void static do_annotate( LocationInfo& info,
				Iterator begin, Iterator end, Iterator first)
		{
			info.line_ = get_sentence_line(first, begin);
			info.column_ = get_sentence_column(first,begin);
			info.length_ = std::distance(begin, end);
		}
	};

} // namespace loader
} // namespace model
} // namespace akasha
#endif
