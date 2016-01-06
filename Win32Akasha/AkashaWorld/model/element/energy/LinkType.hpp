/*
 * LinkType.hpp
 *
 *  Created on: 2012/08/02
 *      Author: ely
 */

#ifndef LINKTYPE_HPP_
#define LINKTYPE_HPP_


#include <boost/graph/adjacency_list.hpp>

namespace akasha
{
namespace model
{
namespace energy
{

enum class LinkType
{
	none = 0,
	tourq,
	eretnic,
};



namespace graphTag
{
struct ConnectTag
{
	typedef boost::edge_property_tag kind;
};
struct ElementTag
{
	typedef boost::vertex_property_tag kind;
};

struct EnergyPortTag
{
	typedef boost::vertex_property_tag kind;

};

}



}
}
}

#endif /* LINKTYPE_HPP_ */
