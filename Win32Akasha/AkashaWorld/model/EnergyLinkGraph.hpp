/*
 * EnergyLinkGraph.hpp
 *
 *  Created on: 2012/08/12
 *      Author: ely
 */

#ifndef ENERGYLINKGRAPH_HPP_
#define ENERGYLINKGRAPH_HPP_

#include "element/energy/LinkType.hpp"
#include "element_v2/ElementBuffer.hpp"
#include "ModelConnectGraph.hpp"

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/filtered_graph.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/hashed_index.hpp>

#include <boost/optional.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/unordered_map.hpp>

namespace akasha
{
namespace model
{
namespace detail
{


template<typename BaseGraphType>
	struct TractionEdgeFilter
{
	BaseGraphType const* gp_;

	TractionEdgeFilter() : gp_( 0 ) {}
	TractionEdgeFilter( BaseGraphType const& o ) : gp_( &o ) {}
	TractionEdgeFilter( TractionEdgeFilter<BaseGraphType> const& o ) : gp_( o.gp_ ) {}

	template<typename EdgeType>
		bool operator()( EdgeType const& e ) const
		{

			return boost::get( energy::graphTag::ConnectTag(), *( gp_ ), e ) == energy::LinkType::tourq;
		}
};

template<typename BaseGraphType>
struct TractionVertexFilter
{

	BaseGraphType const* gp_;

	TractionVertexFilter() : gp_( 0 ) {}
	TractionVertexFilter( BaseGraphType const& o ) : gp_( &o ) { }
	TractionVertexFilter( TractionVertexFilter<BaseGraphType> const& o ) : gp_( o.gp_ ) { }

	template<typename VertexType>
		bool operator()( VertexType const& v ) const
		{
			/*typedef boost::graph_traits<BaseGraphType>::out_edge_iterator outIter;

			  boost::out_edges(v, *(gp_))*/
			return true;
		}

};

}

//ConnectEdge Propertyは Outputで-2 Inputで-1 整数はLinkTypeNumber
struct EnergyLinkGraph
{
	typedef boost::property < energy::graphTag::ConnectTag, energy::LinkType,
			  boost::property<boost::edge_color_t, boost::default_color_type>
				  >  EdgePropertyType;

	typedef boost::adjacency_list <
		boost::vecS,
		boost::vecS,
		boost::bidirectionalS,
		boost::property<energy::graphTag::ElementTag, int>,
		EdgePropertyType,
		boost::no_property > GraphType;

	typedef boost::graph_traits<GraphType>::edge_iterator edgeIteratorType;
	typedef boost::graph_traits<GraphType>::edge_descriptor edgeDescType;

	typedef boost::graph_traits<GraphType>::vertex_iterator vertexIteratorType;
	typedef boost::graph_traits<GraphType>::vertex_descriptor vertexDescType;

	typedef boost::filtered_graph <
		GraphType,
		detail::TractionEdgeFilter<GraphType>,
		detail::TractionVertexFilter<GraphType> > TractionGraphType;

	//typedef element::ElementWrapper ElementType;
	GraphType g_;

	EnergyLinkGraph()
	{
	}

	void constract( element_v2::ElementBuffer const& elements)
	{
		g_.clear();

		for (element_v2::ElementConcept const& e : elements.getList()) {
			boost::put( energy::graphTag::ElementTag(),
					g_,
					boost::add_vertex( g_ ),
					e.getId());
		}

	}

	template<typename ElementType>
		void
		constract( std::vector<ElementType> const& elementList )
		{
			g_.clear();


			// 頂点を登録
			BOOST_FOREACH( ElementType const & e, elementList )
			{
				boost::put(
						energy::graphTag::ElementTag(),
						g_,
						boost::add_vertex( g_ ),
						e.getId()
						);
			}


			//リンクを登録

			BOOST_FOREACH( ElementType const & e, elementList )
			{
				vertexDescType newVertex = this->getVertexDescript( e.getId() );

				//TODO : depro link connection
// 				edgeDescType e_d;
// 				bool is_add;
// 				if (
// 						boost::get<element::connectType::Input>( &( e.getConnectType() ) ) ||
// 						boost::get<element::connectType::Output>( &( e.getConnectType() ) )
// 					)
// 				{
// 					vertexDescType parentVertex = ( this->getVertexDescript( e.getParentId() ) );
// 					boost::tie( e_d, is_add ) = boost::add_edge( parentVertex, newVertex, g_ );
//
// 					auto const& p = elementList.at( e.getParentId() );
// 					if ( boost::get<element::connectType::Output>( &( e.getConnectType() ) ) )
// 						boost::put( energy::graphTag::ConnectTag(), g_, e_d, p.getBackLinkType() );
// 					else if ( boost::get<element::connectType::Input>( &( e.getConnectType() ) ) )
// 						boost::put( energy::graphTag::ConnectTag(), g_, e_d, p.getFaceLinkType() );
// 				}
// 				else if ( e.getLink() != -1 )
// 				{
// 					boost::tie( e_d, is_add ) = boost::add_edge(
// 							( this->getVertexDescript( e.getLink() ) ),
// 							newVertex, g_ );
//
// 					boost::put( energy::graphTag::ConnectTag(), g_, e_d,
// 							elementList.at( e.getLink() ).getLinkTypeByNumber( e.getLinkNumber() )
// 							);
// 				}

				//std::cout << "call" << std::endl;
			}
			//});

}

	void
clear()
{
	g_.clear();
}

template<typename ConnectGraphType>
	void
disconnect( int const disconnectID,
		ConnectGraphType const& connectGraph )
{

	const int rootID = connectGraph.getRootID( ( unsigned int )disconnectID );

	assert( rootID > 0 );
	typedef typename boost::graph_traits<GraphType>::adjacency_iterator adj_iterator;
	typedef std::pair<adj_iterator, adj_iterator> iterType;
	const vertexDescType sv = getVertexDescript( disconnectID );

	std::vector<vertexDescType> buffer;

	for ( iterType i = boost::adjacent_vertices( sv, g_ );
			i.first != i.second; ++i.first )
	{
		if ( rootID == connectGraph.getRootID( ( unsigned int )getElementID( *( i.first ) ) ) )
			buffer.push_back( *( i.first ) );
	}



	//boost::for_each(buffer, [&](vertexDescType const v) {
	BOOST_FOREACH( vertexDescType const v, buffer )
	{
		boost::remove_edge( v, sv, g_ );
	}
	//);


}



std::vector<int>
getAdjacency( int const startId, energy::LinkType const l ) const
{
	const vertexDescType v = getVertexDescript( startId );
	std::vector<int> res;


	boost::for_each( boost::adjacent_vertices( v, g_ ), [&]( vertexDescType const & d )
			{
			if ( l == boost::get( energy::graphTag::ConnectTag(), g_, boost::edge( v, d, g_ ).first )  )
			res.push_back( this->getElementID( d ) );

			} );

	return res;

}

TractionGraphType
getTractionLinkGraph() const
{
	return TractionGraphType(
			g_,
			detail::TractionEdgeFilter<GraphType> {g_},
			detail::TractionVertexFilter<GraphType> {g_}
			);
}

private:
vertexDescType
getVertexDescript( int elementId ) const
{
	typedef std::pair<vertexIteratorType, vertexIteratorType> iterType;
	for ( iterType i = boost::vertices( g_ ); i.first != i.second;
			++i.first )
		if ( boost::get( energy::graphTag::ElementTag(), g_, *( i.first ) ) == elementId )
			return *( i.first );

	assert( false );
	return -1;
}
int
getElementID( vertexDescType const i ) const
{
	return boost::get( energy::graphTag::ElementTag(), g_, i );
}

};


class EnergyPathGraph
{
	//Bundle Struct
	struct Cupler
	{
		int upperSide_;
		int lowerSide_;
	};
	struct Element
	{
		bool isConnection_;
	};
	using graphType =
		boost::adjacency_list<
		boost::vecS, boost::vecS, boost::undirectedS,
		Cupler,
		Element>;

	graphType m_graph;

	struct mapItem
	{
		int id_;
		int parentId_;
		graphType::vertex_descriptor vertex_;
	};
	struct tag_id{};
	struct tag_parent{};
	using mapType =
		boost::multi_index::multi_index_container<
		mapItem,
		boost::multi_index::indexed_by<
			boost::multi_index::hashed_unique<
			boost::multi_index::tag<tag_id>,
		boost::multi_index::member<mapItem, int, &mapItem::id_>>,
		boost::multi_index::hashed_non_unique<
			boost::multi_index::tag<tag_parent>,
		boost::multi_index::member<mapItem, int, &mapItem::parentId_>>>>;

	mapType m_idMap;
	std::vector<std::vector<int>> m_idGroup;
public:
	void init(
			int const energyType,
			element_v2::ElementBuffer const&,
			ModelConnectGraph const&);
	void clear();
	void disconnect(std::vector<int> const&);
	void groupUpdate();
	std::vector<std::vector<int>> const&
		getGroup() const{
			return m_idGroup;
		}

};
}
}

#endif /* ENERGYLINKGRAPH_HPP_ */
