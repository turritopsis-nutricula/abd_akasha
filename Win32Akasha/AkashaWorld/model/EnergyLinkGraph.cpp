#include "EnergyLinkGraph.hpp"

#include <boost/graph/connected_components.hpp>
#include <boost/range/algorithm/sort.hpp>
#include <boost/function.hpp>
#include <algorithm>
using akasha::model::EnergyPathGraph;

bool getEnergy(akasha::model::element_v2::SocketInfo const& i)
{
	return i.energy_;
}
bool getElectronic(akasha::model::element_v2::SocketInfo const& i)
{
	return i.electronic_;
}

void EnergyPathGraph::init(
		int const energyType,
		akasha::model::element_v2::ElementBuffer const& elements,
		akasha::model::ModelConnectGraph const& connectGraph
		)
{
	boost::function<bool (akasha::model::element_v2::SocketInfo const&)> getValue;
	switch(energyType)
	{
		case 0:
			getValue = &getEnergy;
			break;
		case 1:
			getValue = &getElectronic;
			break;
		default:
			assert(false);
	}

	auto const& list = elements.getList();
	for (auto const& elem : list)
	{
		int id = elem.getId();
		int parentId = elem.getParentId();
		if (parentId < 0) continue;

		auto const& parent = list.at(parentId);
		auto const& status = elem.getParameterPtr();
		auto const& parentStatus = parent.getParameterPtr();


		if (getValue(status->getUpperConnectionInfo()) &&
				getValue(parentStatus->getLowerConnectionInfo()))
		{
			auto newVertex = boost::add_vertex(m_graph);
#ifndef NDEBUG
			auto result =
#endif
				m_idMap.insert({id,  parentId,newVertex});
#ifndef NDEBUG
			assert(result.second);
#endif

			m_graph[ newVertex ].upperSide_ = parentId;
			m_graph[ newVertex ].lowerSide_ = id;

			// parent side connect
			if(getValue(parentStatus->getInnerConnectionInfo())){
				auto const& idMap =m_idMap.get<tag_id>();
				auto pVer = idMap.find( parentId );
				if (pVer != idMap.end())
				{
					auto e = boost::add_edge(
							pVer->vertex_, newVertex,m_graph);
					m_graph[ e.first ].isConnection_ =
						getValue(parentStatus->getInnerConnectionInfo());
				}
			}

			// sibling Edge
			boost::for_each(
					m_idMap.get<tag_parent>().equal_range( parentId ),
					[this, newVertex](mapItem const& i){
					auto e = boost::add_edge(
						newVertex, i.vertex_, m_graph);
					m_graph[e.first].isConnection_ = true;
					});

			// lower side
			if ( getValue(status->getInnerConnectionInfo()) )
			{
				boost::for_each(
						m_idMap.get<tag_parent>().equal_range(id),
						[this, newVertex, &status, &getValue](mapItem const& i){
						auto e = boost::add_edge(
							newVertex, i.vertex_, m_graph);
						m_graph[e.first].isConnection_ =
						getValue(status->getInnerConnectionInfo());
						});
			}
		} // if connect

	} // for element

	groupUpdate();
}
void EnergyPathGraph::groupUpdate()
{
	std::vector<int> d( boost::num_vertices(m_graph));
	int num = boost::connected_components(m_graph,&d[0]);

	m_idGroup.clear();
	m_idGroup.resize(num);
	graphType::vertex_descriptor buf(0);
	for(auto const& index : d)
	{
		auto const& prop = m_graph[buf];
		m_idGroup[index].push_back(prop.upperSide_);
		m_idGroup[index].push_back(prop.lowerSide_);
		++buf;
	}

	boost::for_each(m_idGroup, [](std::vector<int>& map){
			boost::sort( map );
			map.erase( std::unique(map.begin(), map.end()), map.end());
			});

}
void EnergyPathGraph::disconnect(std::vector<int> const& buf)
{
	for (auto const& id : buf)
	{
		auto iter = m_idMap.get<tag_id>().find(id);
		if (iter != m_idMap.get<tag_id>().end())
		{
			int parentId = iter->parentId_;
			auto vertex = iter->vertex_;
			// remove sibling edge
			boost::for_each(
					m_idMap.get<tag_parent>().equal_range( parentId ),
					[this,vertex](mapItem const& item)
					{
					boost::remove_edge(vertex, item.vertex_, m_graph);
					});
			// remove parent edge
			auto parentIter = m_idMap.get<tag_id>().find(parentId);
			if (parentIter != m_idMap.get<tag_id>().end())
				boost::remove_edge(vertex, parentIter->vertex_, m_graph);

		} // if find id
	} //for buf

	groupUpdate();
}
void EnergyPathGraph::clear()
{
	m_graph.clear();
	m_idMap.clear();
	m_idGroup.clear();
}
