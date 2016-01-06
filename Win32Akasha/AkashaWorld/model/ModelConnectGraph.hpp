/*
 * ModelConnectGraph.hpp
 *
 *  Created on: 2012/01/21
 *      Author: ely
 */

#ifndef MODELCONNECTGRAPH_HPP_
#define MODELCONNECTGRAPH_HPP_

#include <boost/foreach.hpp>
#include <boost/range/algorithm/for_each.hpp>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/filtered_graph.hpp>

//�Ǝ��ǉ�
enum vertex_element_id_t
{
	element_id
};
enum edge_joint_connect_t
{
	joint_connect
};
namespace boost
{
BOOST_INSTALL_PROPERTY( vertex, element_id );
BOOST_INSTALL_PROPERTY( edge, joint_connect );
}

namespace akasha
{
namespace model
{

//TODO : unsigned int��int���͂����肷��
class ModelConnectGraph
{

private:

	typedef boost::property<vertex_element_id_t, int> vertex_Property;
	typedef boost::property<edge_joint_connect_t, bool> edge_Property;


	typedef boost::adjacency_list<
		boost::vecS, boost::vecS,
		boost::bidirectionalS, vertex_Property, edge_Property,
		boost::no_property > Graph;

	typedef boost::graph_traits<Graph>::vertex_iterator vertexIteratorType;
	typedef boost::graph_traits<Graph>::edge_iterator edgeIteratorType;
	typedef boost::graph_traits<Graph>::edge_descriptor edgeDescType;
	typedef boost::graph_traits<Graph>::vertex_descriptor vertexDescType;

	struct ConnectFillter
	{
		Graph const* g_;
		ConnectFillter() :
			g_( 0 )
		{
		}
		ConnectFillter( Graph const& g ) :
			g_( &g )
		{
		}
		ConnectFillter( ConnectFillter const& o ) :
			g_( o.g_ )
		{
		}

		template<typename EdgeType>
			bool
			operator()( EdgeType const& e ) const
			{
				return boost::get( edge_joint_connect_t(), *( g_ ), e );
			}
	};

	typedef boost::filtered_graph<Graph, ConnectFillter> ConnectGraphType;

	ConnectGraphType m_connectGraph;

private:
	Graph m_graph;

public:
	ModelConnectGraph() :
		m_connectGraph( m_graph, ConnectFillter( m_graph ) )
	{
	}

	void constract(std::vector<std::pair<int,int>> const&);

	//�Ώ�Element�̐ڑ���؂�
	void disconnect( int const id );


	//ElementList����X�V
	std::vector<int> update(
			std::vector<std::pair<int, bool>> const&);

	//�ΏۃG�������g�̃��[�gID��Ԃ�
	int getRootID( int id ) const;


	//�ΏۃG�������g�������ڂ̎q�̃��X�g
	void getChildrenIds( int id, std::vector<int>& res ) const;


	//�ΏۃG�������g�ȉ��̎q�S��
	void getAllChildrenIds( int start, std::vector<int>& res ) const;

	//�ΏۃG�������g����ŏ�i�̃G�������g�܂ł̃��X�g
	void getPrentsIds( unsigned int const id, std::vector<int>& res ) const;
	unsigned int getParentsIds( unsigned int const id, std::vector<int>& result) const;

	int getParent( unsigned int const id) const;

	//ID����VertexDesc���擾
	vertexIteratorType
		getVertexDescByElementID( int id ) const;

	bool isRange(int elementID ) const;
};

}
}
#endif /* MODELCONNECTGRAPH_HPP_ */
