#include "RCFileLoader.hpp"

//boost
#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

#include <boost/unordered_map.hpp>
#include <boost/algorithm/string/case_conv.hpp>



#include "define/ModelDefineStruct.hpp"
#include "define/FusionAdapt.hpp"

#include "parser/commentSkipper.hpp"
#include "parser/BlockSplitGrammar.hpp"
#include "parser/ValBlockGrammar.hpp"
#include "parser/KeyBlockGrammar.hpp"
#include "parser/BodyBlockGrammar.hpp"

//std
#include <tchar.h>
using namespace akasha::model::loader;
using namespace boost;

namespace
{

template<typename StringType>
	struct loadFunctor
{
	bool
		operator()(irr::io::IFileSystem& m_fileSystem, const irr::io::path& p,
				akasha::model::loader::ModelDefine<StringType>& defineResult)
		{
			using boost::fusion::at_c;
			using boost::spirit::qi::phrase_parse;

			//ファイルの存在チェック
			if (!m_fileSystem.existFile(p))
				return false;

			typedef boost::scoped_ptr<irr::io::IReadFile> pIReadFile;
			pIReadFile f(m_fileSystem.createAndOpenFile(p));

			const long size = f->getSize();
			//ファイルのサイズチェック
			if (size < 0) return false;

			//ファイルリード
			//TODO: 読み込みメモリのチェック
			boost::scoped_array<char> data8(new char[size]);
			if (!f->read((void*)(data8.get()), (irr::u32)size))
			{
				return false;
			}

			StringType const fileString(data8.get(), size);

			typedef StringType string_type;
			typedef typename StringType::const_iterator iterator_type;
			iterator_type const fileFirstIter = fileString.cbegin();
			//typedef std::string::iterator iterator_type;

			//ファイルをブロックで切り分ける
			typedef boost::fusion::vector<string_type,
					  boost::iterator_range<iterator_type>> block_type;
			typedef boost::variant<block_type, boost::iterator_range<iterator_type> > item_type;
			boost::unordered_map<string_type, boost::iterator_range<iterator_type>> blockMap;

			{
				typename BlockSplitGrammar<iterator_type, string_type>::result_type result;
				CommentSkipper<iterator_type> skipper;
				BlockSplitGrammar<iterator_type, StringType> parser(skipper);

				iterator_type i = fileString.cbegin();
				if (!qi::phrase_parse(i, fileString.end(), parser, skipper, result))
				{
					//TODO::文法エラー
					std::cout << "failed" << std::endl;
					std::cout << StringType(i, fileString.end()) << std::endl;
					return false;
				}

				BOOST_FOREACH(const item_type& i, result)
				{ //std::cout << i << std::endl;
					if (i.which() == 0)
					{
						string_type s(at_c<0>(boost::get<block_type>(i)));
						boost::to_upper(s);
						blockMap[s] = at_c<1>(boost::get<block_type>(i));
					}
					else
					{
						blockMap["LUA"] = boost::get<
							boost::iterator_range<iterator_type>>(i);
					}
				}

			}

			//パース結果
			unordered_map<string_type, bool> parserReslut;

			typedef iterator_range<iterator_type> fileRange;
			if (blockMap.count("VAL") > 0)
			{
				fileRange const& r = blockMap["VAL"];
				iterator_type iter = r.begin();

				CommentSkipper<iterator_type> skipper;
				ValBlockGrammar<iterator_type, string_type> valpaser(skipper, fileFirstIter);

				parserReslut["VAL"] = spirit::qi::phrase_parse(iter, r.end(),
						valpaser, skipper, defineResult.values_);

				//デバック
				/*BOOST_FOREACH(const auto& d, defineResult.values_)
				  {
				  std::cout << d << std::endl;
				  }*/

			}

			if (blockMap.count("KEY") > 0)
			{
				const fileRange& r = blockMap["KEY"];
				auto i = r.begin();

				CommentSkipper<iterator_type> skipper;
				KeyBlockGrammar<iterator_type, string_type> keyparser(skipper, fileFirstIter);

				parserReslut["KEY"] = spirit::qi::phrase_parse(i, r.end(),
						keyparser, skipper, defineResult.keys_);

				/*BOOST_FOREACH(const auto& d, defineResult.keys_)
				  {
				  std::cout << d <<  std::endl;
				  }*/
			}

			if (blockMap.count("BODY") > 0)
			{
				const fileRange& r = blockMap["BODY"];
				auto i = r.begin();
				CommentSkipper<iterator_type> skipper;

				BodyBlockGrammar<iterator_type, string_type> bodyparser(skipper, fileFirstIter);


				parserReslut["BODY"] = spirit::qi::phrase_parse(i, r.end(),
						bodyparser, skipper, defineResult.elements_);
				/*
					std::cout << defineResult.elements_.size() << std::endl;

					BOOST_FOREACH(const auto& d, defineResult.elements_) {
					std::cout << d << std::endl;
					}
					std::cout << std::flush;
					*/

			}

			if (blockMap.count("LUA") > 0)
			{
				const fileRange& r = blockMap["LUA"];
				defineResult.luaCode_ = StringType(r.begin(), r.end());
			}

			/*
				typedef akasha::model::loader::RcdFileFormat<StringType> result_type;
				typedef std::string::const_iterator iter_type;
				typedef akasha::model::loader::RcdFileSkipper<iter_type> skipper_type;
				typedef akasha::model::loader::LuaCodeComment<iter_type> luaSkipper_type;

				akasha::model::loader::RcdFileParser<
				StringType,
				iter_type,
				skipper_type,
				luaSkipper_type
				> parser;

				skipper_type skipper;

			//std::string s1 = "VAL\r\n{\nTEST(default=0) } KEY{ } \nBODY{ E:Test(color=#tes){ S:Core(t=s){ } N:Chip(0=1){ } } } LUA{ }";


			//if (!qi::parse(s.begin(), s.end(), p, skipper,v))
			if (!qi::phrase_parse(fileString.begin(), fileString.end(), parser, skipper, result))
			{
			std::cout << "failed" << std::endl;
			if (auto c = parser.errorFunctor_.lineNumber_)
			{
			OUT_CONSOL(
			boost::lexical_cast<akasha::tstring>(c)
			);
			}
			return false;
			}
			std::cout << "succcess" << std::endl;
			*/

			//element内のIDCountを初期化
			unsigned int elemId = 0;
			BOOST_FOREACH( auto& e, defineResult.elements_)
			{
				e.IDCountup(elemId);
			}

			bool resultFlg(true);
			for (auto const& r : parserReslut)
			{
				resultFlg = resultFlg && r.second;
			}
			return resultFlg;

		}
};
}

	bool
RCFileLoader::loadFromFile(
		irr::io::IFileSystem& sys,
		const irr::io::path& p,
		akasha::model::loader::ModelDefine<std::string>& result
		)
{
	return loadFunctor<std::string>()(sys, p, result);
}
