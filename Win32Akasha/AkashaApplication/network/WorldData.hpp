/*
 * WorldData.hpp
 *
 *  Created on: 2012/05/27
 *      Author: ely
 */

#ifndef WORLDDATA_HPP_
#define WORLDDATA_HPP_

#include "../../AkashaWorld/model/element/ElementData.hpp"
#include "../../AkashaWorld/model/loader/define/ModelDefineStruct.hpp"

//#include "LinearMath/btVector3.h"
//#include "LinearMath/btQuaternion.h"

#include <boost/unordered_map.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
/*
	namespace boost
	{
	namespace serialization
	{
//btVector3
template<typename Archive>
inline void save(Archive& ar, btVector3 const& v, const unsigned int ver)
{
ar &  v.getX() & v.getY() & v.getZ();
}
template<typename Archive>
inline void load(Archive& ar, btVector3& v, const unsigned int ver)
{
btScalar x,y,z;
ar &  x & y & z;
v.setValue(x,y,z);
}

//btQuaternion
template<typename Archive>
inline void save(Archive& ar,btQuaternion const& q, const unsigned int ver)
{
ar & q.getW() & q.getX() & q.getY() & q.getZ();
}
template<typename Archive>
inline void load(Archive& ar, btQuaternion& q, const unsigned int ver)
{
btScalar w,x,y,z;
ar & w & x & y & z;
q.setValue(x,y,z,w);
}
}
}*/

namespace akasha
{
namespace network
{



template<typename StringType>
	struct OtherModelBase
{

	struct element
	{
		int id_;
		int parentID_;
		int type_;

	private:
		friend class boost::serialization::access;
		template<typename Archive>
			void
			serialize( Archive& ar, const unsigned int ver)
			{
				ar & id_ & parentID_ & type_;
			}
	};

	std::vector<StringType> valueNames_;
	std::vector<element> elements_;
	typedef boost::unordered_multimap<int, boost::fusion::vector<int, model::element::parameter::parameterVariant>> mapType;
	mapType valueManifold_;


private:
	friend class boost::serialization::access;
	template<typename Archive>
		void
		serialize( Archive& ar, const unsigned int ver)
		{
			//TODO : valueManifold–¢ŽÀ‘•
			ar & valueNames_ & elements_;
		}

};

struct OtherModelUpdataer
{
	std::size_t modelHash_;
	std::vector<float> values_;
	typedef boost::fusion::vector<btVector3, btQuaternion> transform_type;
	std::vector<transform_type> transform_;


	/*
		private:
		friend class boost::serialization::access;
		template<typename Archive>
		void
		serialize( Archive& ar, const unsigned int ver)
		{
		ar & modelHash_ & values_ & positions_ & rotate_;
		}*/
};
}
}


#endif /* WORLDDATA_HPP_ */
