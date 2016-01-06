#ifndef ELEMENTCHUNKBUILDER_HPP_INCLUDED
#define ELEMENTCHUNKBUILDER_HPP_INCLUDED

#include "../element_v2/type_fwd.hpp"
#include "../element_v2/ElementBuffer.hpp"
#include "../ValueManifold.hpp"
#include "../loader/define/ModelDefineStruct.hpp"
#include "../ElementModel.hpp"
#include "../../akashaWorldCommon.h"

#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <deque>

namespace akasha
{
namespace model
{
namespace builder
{

struct UserValueManifoldBuilder
{
	boost::optional<
		std::vector<ValueManifoldConcept>&> result_;
	boost::optional<
		boost::ptr_vector<value::ModelValue<float, std::string>>&> values_;
	template<typename ParamType>
		bool add(std::string const& valueName,bool isMinus,
				int elementId, boost::type<ParamType> const&);
};

class ElementChunkBuilder : private boost::noncopyable
{
public:
	using StringType = std::string;
	using resultType = element_v2::ElementBuffer;
	using idListType = std::vector<int>;
	using elementDefType = loader::ModelElementDefine<StringType>;
	using nameAttributeBufType = boost::unordered_map<StringType, int>;
	// first: normal parent id, second: special parent id
	using parentInfoType = std::pair<int, int>;

	struct buildInfo
	{
		StringType name_;
		elementDefType const& dataRef_;
		parentInfoType parentInfo_;
		bool isAdded_; // Insert to System

		buildInfo(
				StringType const& n,
				elementDefType const& data,
				parentInfoType const& parentId,
				bool isAdded=false) :
			name_(n),
			dataRef_(data),
			parentInfo_(parentId),
			isAdded_(isAdded){}

		buildInfo(buildInfo&&) = default;
	};
	using elementBuildQueue = std::vector<buildInfo>;

private:
	irrExt::IrrExtention& m_irrEx;
	wisp::v3::WispHandle& m_client;

	builder::BtShapeBuilder& shapes_;

	ElementModel::ModelType const modelType_;

	nameAttributeBufType m_nameUserValueBuffer;
	elementBuildQueue m_buildQueue;
	UserValueManifoldBuilder m_manifoldBuilder;


public:
	using connectDirectMappingType =
		boost::unordered_map<StringType,
		element_v2::DirectType>;
	using attributeFuncMapType =
		boost::unordered_map<
		StringType,
		boost::function<void (
				element::ElementData&,
				UserValueManifoldBuilder&,
				nameAttributeBufType const&,
				loader::ModelExpressionDefine<std::string> const&)>>;
	using ElementBuildFuncMap =
		boost::unordered_map<StringType,
		boost::function<int (
				resultType&,
				elementDefType const&,
				element::ElementData const&,
				irrExt::IrrExtention&,
				wisp::v3::WispHandle&,
				bool const)>>;
	using ManifoldBuildFuncMap =
		boost::unordered_map<StringType,
		boost::function<void (
				int, UserValueManifoldBuilder&,
				loader::ModelExpressionDefine<std::string> const&
				)>>;
	using QueueMakeFuncMap =
		boost::unordered_map<StringType,
		boost::function<parentInfoType (
				elementBuildQueue&,
				elementDefType const&,
				parentInfoType
				)>>;
private:

	connectDirectMappingType m_directMapping;
	attributeFuncMapType m_attrFuncMap;
	ElementBuildFuncMap m_builderMapping;
	ManifoldBuildFuncMap m_manifoldMapping;
	QueueMakeFuncMap m_queueMakeFuncs;

public:

	ElementChunkBuilder() = delete;
	ElementChunkBuilder(
			irrExt::IrrExtention& ext,
			wisp::v3::WispHandle& c ,
			builder::BtShapeBuilder& s,
			ElementModel::ModelType t
			) :
		m_irrEx(ext), m_client(c), shapes_(s), modelType_(t)
	{ init(); }

	void init();

	void build(
			resultType& result,
			std::vector<ValueManifoldConcept>& manifoldRes,
			boost::ptr_vector<value::ModelValue<float, std::string>>& values,
			std::vector< elementDefType > const& data);

	//始点。親Entity無しで呼び出した場合
private:
	void makeQueue(std::vector< elementDefType> const&, parentInfoType const);
	void buildExecute(resultType&);

private:
	//! パラメータセットのためのヘルパー
	struct SwitchFuncter
	{
		element::ElementData& data_;
		float value_;
		template<typename T>
			void operator()(const boost::type<T>& t)
			{
				data_.setParameterValue(t,value_);
			}
		void
			operator()(const boost::type< element::parameter::nameString >&)
			{

			}
	};

};

}
}
}

#endif // ELEMENTCHUNKBUILDER_HPP_INCLUDED
