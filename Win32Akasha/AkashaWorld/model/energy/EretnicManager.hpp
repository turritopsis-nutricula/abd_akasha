#ifndef AKASHA_MODEL_ENERGY_ERETONICMANAGER_HPP_
#define AKASHA_MODEL_ENERGY_ERETONICMANAGER_HPP_

#include <boost/range/algorithm/copy.hpp>
#include <boost/range/algorithm/set_algorithm.hpp>

#include <iterator>
#include <iostream>
namespace akasha
{
namespace model
{
namespace energy
{
class EretnicManager
{
	std::vector<
		std::vector<int>> group_;
public:
	void clear()
	{
		group_.clear();
	}
	void add(std::vector<int> const& aggregation )
	{
		{
			std::vector<int> buf;
			for (auto& g : group_)
			{
				buf.clear();
				boost::copy(g, std::inserter(buf, buf.end()));
				g.clear();

				boost::set_difference(buf, aggregation, std::inserter(g, g.end()));
			}
		}
		group_.push_back( aggregation );

		//TODO
		for (auto const& g : group_)
		{
			for (auto const& id : g)
				std::cout << id << ",";
			std::cout << "\n";
		}
	}

};
} // namespace energy
} // namespace model
} // namespace akasha
#endif /* end of include guard */
