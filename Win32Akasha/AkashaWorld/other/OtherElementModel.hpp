/*
 * OtherElementModel.hpp
 *
 *  Created on: 2012/05/27
 *      Author: ely
 */

#ifndef OTHERELEMENTMODEL_HPP_
#define OTHERELEMENTMODEL_HPP_

#include "../model/element/ElementData.hpp"
#include "../../AkashaApplication/network/WorldData.hpp"



#include <boost/range/algorithm.hpp>
#include <boost/unordered_map.hpp>
#include <boost/type.hpp>

namespace akasha
{

namespace model
{

namespace other
{
class OtherElement
{

	int m_type;
	element::parameter::elementParameter m_Parameter;
	//boost::shared_ptr<will::id::AnimationNode> m_irrNode;

public:
	OtherElement(){


	}

	/*boost::shared_ptr<will::id::AnimationNode>&
	  getIrrNode() { return m_irrNode; }*/
	/*
		element::parameter::elementParameter&
		getParameters(){ return m_Parameter; }*/
};


namespace builder
{
//ParameterVariantに実型をセット
struct ValueHelper
{
	element::parameter::parameterVariant v_;
	template<typename Parameter>
		void
		operator()(const boost::type<Parameter>&)
		{
			v_ = Parameter();
		}
};

//! Elementをビルド
template<typename LinkMapType>
struct OtherElementBuilder
{
	std::vector<other::OtherElement>& list_;
	LinkMapType& map_;

	//次のelementIDをリターン
	template<typename ElementDataType>
		void
		constract(ElementDataType const& baseData)
		{
			{
				// 							ValueHelper valueBuilder;

				//パラメーターセット
				/*boost::for_each(baseData.propaties_,
				  [&]
				  (const loader::ModelExpressionDefine<StringType>& exp){
				  if (helper(valueBuilder, exp.name_))
				  {
				  typename LinkMapType::mapped_type p;
				  p.first = elementID;
				  p.second.swap( valueBuilder.v_);
				  this->map_.insert({exp.name_, p});
				  }
				  });*/

				list_.push_back(other::OtherElement());
				//ID
				/*list_.back().getIrrNode() =  will::IrrlichtServerInterface::Instance().getClient().regist(
				  will::server::command::AddAnimationNode{"./Resources/element/chip.x" },
				  boost::type<will::id::AnimationNode>()
				  );*/


			}
			/*
				int nextElementID = elementID+1;
				boost::for_each(baseData.children_,[&](const loader::ModelElementDefine<StringType>& d){
				OtherElementBuilder<LinkMapType> b{this->list_, this->map_};
				nextElementID = b.constract(d, nextElementID);
				});*/


		}
};
}



//! ネットワーク上の他のモデル
class OtherElementModel
{
	typedef std::string valueName_type;
	std::vector<valueName_type> m_values;
	std::vector<other::OtherElement> elements_;
	typedef boost::unordered_multimap<valueName_type,  std::pair<int, element::parameter::parameterVariant>> linkMap_type;
	linkMap_type m_valueLink;

public:
	template<typename StringType>
		void
		constract(network::OtherModelBase<StringType> const& baseData)
		{
			m_values = baseData.valueNames_;


			BOOST_FOREACH(auto const& d, baseData.elements_){
				builder::OtherElementBuilder<linkMap_type> builder{this->elements_,this->m_valueLink};
				builder.constract(d);
			}

		}

	void
		update(const  network::OtherModelUpdataer& d)
		{

			//auto& irrClient = will::IrrlichtServerInterface::Instance().getClient();

			assert(d.transform_.size()==elements_.size() );
			btScalar x,y,z;
			for (unsigned int i=0; i<d.transform_.size(); i++)
			{

				const auto& t = d.transform_.at(i);
				//const auto& p = boost::fusion::at_c<0>(t);
				const auto& r = boost::fusion::at_c<1>(t);
				btMatrix3x3( r ).getEulerYPR(y,x,z);


				/*irrClient.command( elements_.at(i).getIrrNode(),
				  will::command::NodeMove{
				  irr::core::vector3df(p.getX(),p.getY(),p.getZ()),
				  irr::core::vector3df( btDegrees(z), btDegrees(x), btDegrees(y))}
				  );*/
			}

		}
};
}
}
}


#endif /* OTHERELEMENTMODEL_HPP_ */
