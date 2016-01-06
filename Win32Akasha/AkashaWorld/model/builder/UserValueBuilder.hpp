#ifndef AKASHA_MODEL_BUILDER_USERVALUEBUILDER_HPP_
#define AKASHA_MODEL_BUILDER_USERVALUEBUILDER_HPP_

namespace  {
template<typename T,typename MapType>
	bool getExpValue( char const* keyword, T& t, MapType const& map)
	{
		if (map.count(keyword) >0 &&
				map.at(keyword).isNumberValue())
		{
			if (auto v = map.at(keyword).getNumberValue())
			{
				t = *v;
				return true;
			}
		}

		return false;
	}
} // namespace

namespace akasha {
namespace model {
namespace builder {

template<typename string_type>
	struct UserValueBuilder
	{

		template<typename T>
			bool
			build(const std::vector<loader::ModelValueDefine<string_type>>& v,
					boost::ptr_vector<value::ModelValue<T, string_type>>& r)
			{

				typedef loader::ModelExpressionDefine<string_type> expdef_type;
				typedef std::map<string_type, const expdef_type> mapType;
				auto& logger = akasha::system::AkashaSystem::Instance().getLogger();

				for (loader::ModelValueDefine<string_type> const& define : v) {
					mapType params;

					for (expdef_type const& exp : define.propatyes_) {
						string_type const s(
								boost::algorithm::to_lower_copy(
									exp.getName()));

						typedef typename mapType::value_type valueType;
						params.insert( valueType(s, exp ));
					}

					T def = T(0), min=T(0), max=T(10000000000000), step=T(0),disp=T(0);
					if(getExpValue("default", def, params))
						params.erase("default");
					if(getExpValue("min", min, params))
						params.erase("min");
					if(getExpValue("max", max, params))
						params.erase("max");
					if(getExpValue("step", step,params))
						params.erase("step");
					if(getExpValue("disp", disp, params))
						params.erase("disp");


					r.push_back(
							new value::ModelValue<T, string_type>(
								define.valname_,
								def,min,max,step,int(disp))
							);
					for (auto const& inv : params) {
						expdef_type const& expDef = inv.second;
						string_type const& key = expDef.getName();
						if(expDef.isNumberValue())
						{
							logger(
									system::log::ModelFileWarning{
									"",
									(boost::format("Invalid keyword : %1% line:%2% :%3%") %
									 key % expDef.line_ % expDef.column_).str()});
						}
						else
						{
							logger(
									system::log::ModelFileWarning{
									"",
									(boost::format("Invalid expression : %1%=%4% line:%2% :%3%") %
									 key % expDef.line_ % expDef.column_
									 % expDef.getValueRawString()).str()});
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
