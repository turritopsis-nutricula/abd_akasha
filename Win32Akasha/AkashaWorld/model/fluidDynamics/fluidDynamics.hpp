
#include "../element/ElementWrapper.hpp" //TODO : deproy
#include "../element_v2/ElementBuffer.hpp"

#include "LinearMath/btTransform.h"

#include <boost/range/algorithm.hpp>
#include <boost/foreach.hpp>
#include <boost/array.hpp>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

#include <vector>
#include <iostream>


namespace akasha
{
namespace model
{
namespace fluid
{



struct ElementForm
{
	unsigned int space_; //空間番号
	int id_;            //エレメントID
	btVector3 position_; //ワールド座標
	btVector3 normal_;   //面方向
	btVector3 firstEdge_; //先行辺座標(原点positionワールド系)
	btVector3 secondEdge_; //第二辺座標
	//btTransform form_;
	//boost::array<btVector3, 4> vertexInWorld_;


};

namespace detail
{
//流体空間
struct FluedSpace
{
	btTransform worldTrans_;
};

struct Arrangement
{
	bool isForwardEdge_;
};
}

struct ModelFiguare
{
	typedef boost::adjacency_list<
		boost::vecS,
		boost::vecS,
		//boost::bidirectionalS,
		//boost::undirectedS,
		boost::directedS,
		ElementForm,
		detail::Arrangement> GraphType;

	GraphType graph_;
	std::vector< typename GraphType::vertex_descriptor > m_virtualElement;
	unsigned int m_elementCount;


	void init(std::vector<model::element::ElementWrapper> const& list)
	{
		graph_.clear();

		{//頂点追加
			int count = 0;

			BOOST_FOREACH(auto const& e, list)
			{
				GraphType::vertex_descriptor v = boost::add_vertex(graph_);
				//graph_[v].id_ = e.getId();
				graph_[v].id_ = count++;
			}

			assert(list.size() == count);
			m_elementCount = list.size();
		}

		update(list);
	}


	void update(std::vector<model::element::ElementWrapper> const& list)
	{

		GraphType::vertex_iterator i,end;
		for(boost::tie(i,end)=boost::vertices(graph_); i!=end; ++i){
			boost::clear_vertex(*i, graph_);

			auto& v = graph_[ *i ];
			auto const& e = list[ v.id_ ];
			v.position_ = e.getTransform().getOrigin();
			//TODO : この辺適当
			v.normal_   = e.getTransform().getBasis()[ 0 ];
			v.firstEdge_ = e.getTransform().getBasis()[ 1 ];
			v.firstEdge_ = e.getTransform().getBasis()[ 2 ];
		}

	}

	void clear()
	{
		GraphType::vertex_iterator i,e;
		for(boost::tie(i,e)=boost::vertices(graph_); i!=e; ++i)
		{
			boost::clear_vertex(*i, graph_);
		}

		boost::for_each(m_virtualElement,[&](GraphType::vertex_descriptor v){
				boost::remove_vertex( v, graph_ );
				});
		m_virtualElement.clear();
	}


	void addFaseJoint(int fowardId, int backId)
	{
		auto e=boost::add_edge(getVertex(fowardId),getVertex(backId), graph_);
		graph_[e.first].isForwardEdge_ = false;
	}

	void margiEdges(std::vertex<int> const& idList)
	{

	}
	void addFowardEge(int fowardId0, int fowardId1)
	{
		GraphType::vertex_descriptor
			u=getVertex(fowardId0),
			v = getVertex(fowardId1);

		auto e = boost::add_edge(u,v, graph_);
		graph_[e.first].isForwardEdge_ = true;
		e = boost::add_edge(v,u, graph_);
		graph_[e.first].isForwardEdge_ = true;
	}


	GraphType::vertex_descriptor getVertex(const int id) const
	{
		GraphType::vertex_iterator i,e;
		for(boost::tie(i,e)=boost::vertices(graph_); i!=e; ++i)
		{
			if (id==graph_[ *i ].id_)
				return *i;
		}
		assert(false);
		return 0;
	}


	void disp() const
	{
		GraphType::vertex_iterator i,e;
		for(boost::tie(i,e)=boost::vertices(graph_); i!=e; ++i){
			auto const& p = graph_[*i];
			std::cout << p.id_ << ":" << std::endl;
		}
		for (
				std::pair<GraphType::edge_iterator,GraphType::edge_iterator> ei = boost::edges(graph_);
				ei.first != ei.second; ++ei.first
			 )
		{
			std::cout <<
				(int)boost::source(*(ei.first), graph_)
				<< "->" <<
				(int)boost::target(*(ei.first), graph_)
				<< std::endl;
		}
	}
};

class ElementAirStream
{
	std::vector<std::vector<int>> m_nearList;
public:
	void update(element_v2::ElementBuffer const& elements)
	{
		m_nearList.clear();
		std::vector<int> const l =[&elements]()
		{
			std::vector<int> r;
			r.reserve(elements.getList().size());
			for (auto const& e : elements.getList())
				if (!e.isAdapter())
					r.push_back(e.getId());
			return std::move(r);
		}

		for (std::size_t head=0; head<l.size(); ++head)
		{
			btTransform const& b =
				elements.getList()[head].getParameterPtr()->getWorldTransform();
			m_nearList.push_back( std::vector<int>() );
			auto& buffer = m_nearList.back();
			for (std::size_t id=head; id<l.size(); ++id)
			{
				btTransform const& i =
					elements.getList()[id].getParameterPtr()->getWorldTransform();
				if (b.getOrigin().distance(i.getOrigin()) < 6.f)
					buffer.push_bach(elements.getList()[id].getId());

			} // for id
		} // for head

		//TODO :
	}
};
}
}
}

