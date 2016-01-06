#ifndef AKASHA_MODEL_BUILDER_CONTROLKEYBUILDER_HPP_
#define AKASHA_MODEL_BUILDER_CONTROLKEYBUILDER_HPP_

#include <boost/unordered_set.hpp>
#include <boost/range/algorithm/find_if.hpp>
namespace akasha {
namespace model {
namespace builder {

////////////////////////////////////////////////////////////////
//コントロールビルダー
template<typename NumberType, typename StringType>
	class ControlKeyBuilder
	{
		typedef value::ModelValue<NumberType, StringType> modelValueType;
		typedef boost::ptr_vector<modelValueType> valuesVectorType;

		valuesVectorType& values_;

	public:
		ControlKeyBuilder( valuesVectorType& v) :
			values_(v)
		{
		}

		template<typename V>
			bool
			build(const std::vector<loader::ModelKeyDefine<StringType>>& def,
					boost::ptr_vector<V>& vec)
			{
				//TODO : 要リファクタリング
				using boost::fusion::at_c;
				typedef loader::ModelKeyDefine<StringType> keydefine_type;
				typedef typename keydefine_type::control_type crtl_type;
				boost::unordered_set<StringType> const
					valueNameBuffer =
					[&](){
						boost::unordered_set<StringType> res;
						for(const keydefine_type& d: def)
						{
							for(const crtl_type& c: d.controlValues_)
								res.insert(boost::fusion::at_c<0>(c));
						}
						return std::move(res);
					}();
				for(auto const& n : valueNameBuffer)
				{
					auto i =
						boost::find_if(values_,
								[&n](modelValueType const& v){
								return n==v.getName();
								});
					if(i!=values_.end())
						vec.push_back( new V( *i ));
					// 								for(modelValueType& v: values_)
					// 								{
					// 									if (n== v.getName())
					// 										vec.push_back( new V(v) );
					// 								}
				}

				for(const loader::ModelKeyDefine<StringType>& d: def)
				{
					int const keyNum = d.keyNumber_;

					typedef typename loader::ModelKeyDefine<StringType>::control_type control_type;
					auto& logger = system::AkashaSystem::Instance().getLogger();
					for(const control_type& c: d.controlValues_)
					{
						StringType const valueName(at_c<0>(c));
						boost::optional<NumberType> stepValue(boost::none);

						for(const loader::ModelExpressionDefine<StringType>& e: at_c<1>(c))
						{
							StringType s(e.getName());
							boost::algorithm::to_lower(s);
							if(s == "step" && e.isNumberValue())
							{
								stepValue = e.getNumberValue();
								// break;
							}
							else
							{
								//step以外のパラメータ警告
								logger(system::log::ModelFileWarning{
										"",
										(boost::format("invalid expression : %1%(...%2%=*...)") % valueName % s).str()
										});

							}

						}

						if (auto s = stepValue)
						{
							//stepValueがboost::none以外であった場合
							for(V& value: vec)
							{
								if (valueName == value.getParameterName())
								{
									value.addItem( keyNum, btScalar(*s));
									break;
								}
							}
						}

					}
				}

				return true;
			}
	};
} // namespace builder
} // namespace model
} // namespace akasha
#endif
