#include "ModelConnectGraph.hpp"

using namespace akasha;
using namespace akasha::model;

void ModelConnectGraph::constract(
		std::vector<std::pair<int,int>> const& elements)
{
	m_graph.clear();
	for ( auto const& e : elements)
		boost::put( vertex_element_id_t(), m_graph,
				boost::add_vertex(m_graph), (std::size_t)e.first);

	bool is_add;
	edgeDescType edge;
	for ( auto const& e : elements)
	{
		vertexDescType const v =
			*(getVertexDescByElementID( e.first ));
		vertexDescType const vp =
			*(getVertexDescByElementID( e.second ));

		boost::tie( edge, is_add) =
			boost::add_edge( v, vp, m_graph );
		assert( is_add && "ModelConnectGraph constract");
		boost::put( edge_joint_connect_t(), m_graph, edge, true);

	}


}

void
ModelConnectGraph::disconnect( int const id )
{
	vertexDescType vertex = *( getVertexDescByElementID( id ) );
	boost::graph_traits<Graph>::out_edge_iterator i, end;
	assert( boost::out_degree( vertex, m_graph ) <= 1 );
	boost::tie( i, end ) = boost::out_edges( vertex, m_graph );

	if ( i != end )
		boost::put( edge_joint_connect_t(), m_graph, *i, false );

}


std::vector<int>
ModelConnectGraph::update(
		std::vector<std::pair<int, bool>> const& connectList)
{
	std::vector<int> res;

	bool buf;
	for (auto const& connectParam : connectList)
	{
		vertexDescType const v =
			*( getVertexDescByElementID( connectParam.first ));
		bool const& isConnect = connectParam.second;
		auto edge_param =  boost::out_edges(v, m_graph);
		if ( edge_param.first != edge_param.second)
		{
			buf = boost::get( edge_joint_connect_t(), m_graph, *(edge_param.first));
			if (buf && !isConnect) res.push_back( connectParam.first );
			boost::put( edge_joint_connect_t(), m_graph,
					*(edge_param.first), isConnect);
		}
	}

	return std::move(res);
}



//対象エレメントのルートIDを返す
int
ModelConnectGraph::getRootID( int id ) const
{
	vertexDescType vertex = *( getVertexDescByElementID( id ) );
	boost::graph_traits<ConnectGraphType>::out_edge_iterator i, end;

	assert( boost::out_degree( vertex, m_connectGraph ) <= 1 );
	boost::tie( i, end ) = boost::out_edges( vertex, m_connectGraph );
	for ( ;; )
	{
		if ( i != end )
		{
			vertex = boost::target( *i, m_connectGraph );
			assert( boost::out_degree( vertex, m_connectGraph ) <= 1 );
			boost::tie( i, end ) = boost::out_edges( vertex,
					m_connectGraph );
		}
		else
		{
			break;
		}
	}

	return boost::get( vertex_element_id_t(), m_connectGraph, vertex );
}

//対象エレメントが持つ直接の子のリスト
void
ModelConnectGraph::getChildrenIds( int id, std::vector<int>& res ) const
{
	auto vertex = *( getVertexDescByElementID( id ) );

	//				boost::graph_traits<Graph>::in_edge_iterator i, end;
	boost::graph_traits<ConnectGraphType>::in_edge_iterator i, end;
	boost::for_each(
			//boost::in_edges(vertex, m_graph),
			boost::in_edges( vertex, m_connectGraph ),

			[this, &res]( edgeDescType const & e )
			{
			res.push_back(
					boost::get( vertex_element_id_t(), m_connectGraph, boost::source( e, m_connectGraph ) ) );
			} );
}

//対象エレメント以下の子全て
void
ModelConnectGraph::getAllChildrenIds( int start, std::vector<int>& res ) const
{
	res.push_back( start );
	vertexDescType vertex = *( getVertexDescByElementID( start ) );

	//boost::graph_traits<ConnectGraphType>::in_edge_iterator i, end;

	boost::for_each( boost::in_edges( vertex, m_connectGraph ),
			[this, &res]( const edgeDescType & e )
			{

			int id = boost::get( vertex_element_id_t(), m_connectGraph, boost::source( e, m_connectGraph ) );


			getAllChildrenIds( id, res );
			} );
}

//対象エレメントから最上段のエレメントまでのリスト
void
ModelConnectGraph::getPrentsIds( unsigned int const id, std::vector<int>& res ) const
{

	vertexDescType vertex = boost::vertex(id, m_graph);
	boost::graph_traits<ConnectGraphType>::out_edge_iterator i, end;

	for ( ;; )
	{
		res.push_back(
				boost::get( vertex_element_id_t(), m_connectGraph,
					vertex ) );

		boost::tie( i, end ) = boost::out_edges( vertex,
				m_connectGraph );
		if ( i != end )
		{
			vertex = boost::target( *i, m_connectGraph );
		}
		else
			break;
	}
}
unsigned int ModelConnectGraph::getParentsIds( unsigned int const id, std::vector<int>& result) const
{

	unsigned int count(0);
	vertexDescType vertex = boost::vertex(id, m_graph);
	boost::graph_traits<ConnectGraphType>::out_edge_iterator i, end;

	for ( ;; )
	{
		boost::tie( i, end ) =
			boost::out_edges( vertex, m_connectGraph );
		if ( i != end )
		{
			vertex = boost::target( *i, m_connectGraph );
			result.push_back( boost::get( vertex_element_id_t(), m_connectGraph, vertex ) );
			++count;
		}
		else
			break;
	}
	return count;
}

int ModelConnectGraph::getParent( unsigned int const id) const
{
	auto v( boost::vertex(id, m_graph));
	boost::graph_traits<ConnectGraphType>::out_edge_iterator iter, end_iter;

	boost::tie(iter, end_iter) = boost::out_edges(v, m_connectGraph );
	if (iter != end_iter)
	{
		auto parent_vertex = boost::target( *iter, m_connectGraph);
		return (int)(boost::get(vertex_element_id_t(), m_connectGraph, parent_vertex));
	}

	return -1;

}

//IDからVertexDescを取得
ModelConnectGraph::vertexIteratorType
ModelConnectGraph::getVertexDescByElementID( int id ) const
{

	vertexIteratorType i, end;
	for ( boost::tie( i, end ) = boost::vertices( m_graph ); i != end;
			i++ )
	{
		if ( boost::get( vertex_element_id_t(), m_graph, *i ) == id )
			return i;
	}

	return end;
}
bool ModelConnectGraph::isRange(int elementID ) const
{
	return (unsigned int)elementID < boost::num_vertices(m_graph);
}
