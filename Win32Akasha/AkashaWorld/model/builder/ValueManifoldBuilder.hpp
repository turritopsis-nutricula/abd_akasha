#ifndef AKASHA_MODEL_VALUE_MANIFOLD_BUILDER_HPP_
#define AKASHA_MODEL_VALUE_MANIFOLD_BUILDER_HPP_

#include <boost/unordered_set.hpp>

namespace akasha {
namespace model {
namespace builder {

struct ValueManifoldBuilder
{
	typedef StringType std::string;
	typedef loader::ModelValueDefine<StringType> valueDefineType;
	typedef loader::ModelElementDefine<StringType> elementDefineType;
	typedef loader::ModelExpressionDefine<StringType> expressionType;

	typedef boost::unordered_set<StringType> valueSetType;
	struct resultType
	{
		bool isMinus_;
		StringType valueName_;
		unsigned int elementId_;
		element::parameter::parameterVariant param;
	};

	std::vector< resultType > build(
			std::vector<valueDefineType> const& values,
			std::vector<elementDefineType> const& elementDefs
			)
	{
		using boost::fusion::at_c;
		std::vectr<resultType> result;
		for(elementDefineType const& elemDef : elementDefs)
		{
			for(expressionType const& exp :
					elemDef.propaties_)
			{
				if (! exp.isNumberValue())
				{
					result.push_back(
							resultType{
							at_c<0>( *(exp.getStringValue())),
							at_c<1>( *(exp.getStringValue())),
							elemDef.getId(),
							paramTypeMapByString[ exp.getName()]});
				}
			}
		}
	}
};

} // namespace builder
} // namespace model
} // namespace akasha
#endif
