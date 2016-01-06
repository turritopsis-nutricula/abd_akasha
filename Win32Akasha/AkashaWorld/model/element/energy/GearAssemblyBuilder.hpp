/*
 * GearAssemblyBuilder.hpp
 *
 *  Created on: 2012/08/25
 *      Author: ely
 */

#ifndef GEARASSEMBLYBUILDER_HPP_
#define GEARASSEMBLYBUILDER_HPP_



#include "TractionLink.hpp"
#include "Harnnes.hpp"

namespace akasha{ namespace model{ namespace energy{


//! boost::graph::dfs_visiter

template<typename ElementListType>
	struct GearAssemblyBuilder
	{
		ElementListType& elements_;
		std::vector<GearAssimbly>& assimblies_;

		template<typename VertexType, typename GraphType>
			void initialize_vertex(VertexType v, GraphType const& g)
			{

			}
		template<typename VertexType, typename GraphType>
			void start_vertex(VertexType v, GraphType const& g)
			{
				//連結成分の最初
				if (boost::out_degree(v, g)>0 || boost::in_degree(v, g)>0)
					assimblies_.push_back(GearAssimbly());

				//std::cout << "start : " << boost::get(boost::vertex_name, g, v) << std::endl;
			}

		template<typename VertexType, typename GraphType>
			void discover_vertex(VertexType v, GraphType const& g)
			{
				//構成ノード
				int id = getElementID(v,g);
				energy::LinkInterfaceType ptr;
				ptr = boost::make_shared<energy::Gear>();
				elements_.at(id).getEnergyLink(boost::ref(ptr));

				if (boost::out_degree(v, g)>0 || boost::in_degree(v, g)>0)
					//if (boost::get<boost::shared_ptr<energy::Gear>>(ptr))
					assimblies_.back().geares_.push_back(boost::get<boost::shared_ptr<energy::Gear>>(ptr) );

				/*else
				  assert(false);*/

				//std::cout << "discover : " << boost::get(boost::vertex_name, g, v) << std::endl;
			}
		template<typename EdgeType, typename GraphType>
			void examine_edge(EdgeType e, GraphType const& g)
			{
				/*std::cout << "examine : " <<
				  boost::get(boost::vertex_name, g, boost::source(e,g)) <<
				  "--" <<
				  boost::get(boost::vertex_name, g, boost::target(e,g)) <<
				  "(" << boost::get(boost::edge_color, g,e) << std::endl;*/
			}
		template<typename EdgeType, typename GraphType>
			void tree_edge(EdgeType e, GraphType const& g)
			{
				/*std::cout << "tree : " <<
				  boost::get(boost::vertex_name, g, boost::source(e,g)) <<
				  "--" <<
				  boost::get(boost::vertex_name, g, boost::target(e,g)) << std::endl;*/
			}


		template<typename EdgeType, typename GraphType>
			void back_edge(EdgeType e, const GraphType& g)
			{
				//後退辺が発生した時点で閉路
				assert(false);
			}

		template<typename EdgeType, typename GraphType>
			void forward_or_cross_edge(EdgeType e, GraphType const& g)
			{
				/*std::cout << "forward,cross : " <<
				  boost::get(boost::vertex_name, g, boost::source(e,g)) << "--" <<
				  boost::get(boost::vertex_name, g, boost::target(e,g)) << std::endl;*/
			}

		template<typename VertexType, typename GraphType>
			void finish_vertex(VertexType v, GraphType const& g)
			{
				//std::cout << "finish : " << boost::get(boost::vertex_name, g, v) << std::endl;
			}

		template<typename VertexType, typename GraphType>
			int
			getElementID(VertexType v, GraphType const& g)
			{
				return boost::get(energy::graphTag::ElementTag(), g, v);
			}
	};


}
}}

#endif /* GEARASSEMBLYBUILDER_HPP_ */
