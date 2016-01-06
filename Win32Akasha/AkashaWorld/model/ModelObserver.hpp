#ifndef AKASHA_MODEL_MODELPROXY_HPP_
#define AKASHA_MODEL_MODELPROXY_HPP_

#include "element_v2/detail/ElementParameter.hpp"
#include "ModelValue.hpp"

#include <boost/range/algorithm/sort.hpp>
#include <boost/range/algorithm/unique.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <boost/unordered/unordered_map.hpp>


namespace akasha {
namespace model {

// Element集合へのプロクシ
class ModelObserver {
public:
	using statusListType =
		std::vector<
		boost::shared_ptr<element_v2::ElementParameter>>;
private:
	statusListType m_list;
	std::vector< std::size_t > m_topList;

	std::vector< std::pair<btVector3, btVector3> > m_aabbList;
	std::vector< btVector3> m_gravityCenterList;

	std::size_t m_modelLifeTicks;
	boost::unordered_map<std::string, float> m_modelValueList;

public:
	void addElement(boost::shared_ptr<element_v2::ElementParameter> const& e)
	{
		m_list.push_back( e );
	}
	void addValue(std::string const& name)
	{
		m_modelValueList[name] = 0.f;
	}

	void update(
			std::size_t const modelLifeTicks,
			value::ModelValueBuffer const& values)
	{
		// Value update
		for (auto const& v : values)
			m_modelValueList[v.getName()] = v.getVolum();

		// pair list
		using pair_type = std::pair<std::size_t, std::size_t>;
		std::vector<pair_type> parent_child_list;

		parent_child_list.reserve( m_list.size() );

		for (auto const& s : m_list)
			parent_child_list.emplace_back( getElementTop(s->id()), s->id());

		boost::sort(parent_child_list,
				[](pair_type const& lh,pair_type const& rh){ return lh.first<rh.first;});

		{
			m_topList.clear();

			m_aabbList.clear();
			m_gravityCenterList.clear();

			auto top_iter = parent_child_list.cbegin();
			auto end_iter = top_iter;

			std::size_t top_index(0);

			while (end_iter != parent_child_list.cend())
			{
				// find first other top iter
				end_iter = std::find_if(top_iter, parent_child_list.cend(),
						[&top_iter](pair_type const& p){ return p.first != top_iter->first;});

				for (int i=0; i<std::distance(top_iter,end_iter); ++i)
					m_topList.push_back(top_index);
				++top_index;

				m_aabbList.push_back(calcAabb( top_iter, end_iter));
				m_gravityCenterList.push_back( calcGravityCenter( top_iter, end_iter) );

				top_iter = end_iter;
			}
		}


		m_modelLifeTicks = modelLifeTicks;
	}

	template<typename F>
		void status_for_each(F f) const
		{
			for (auto const& i : m_list)
				f( *i );
		}
	boost::unordered_map<std::string, float> const&
		getModelValueMap() const
		{
			return m_modelValueList;
		}

	element_v2::ElementParameter const&
		getElementStatus(std::size_t id) const
		{
			assert(id < m_list.size());
			return *(m_list[id]);
		}
	std::size_t getElementCount() const
	{
		return m_list.size();
	}

	std::size_t getElementTop(std::size_t id) const
	{
		if ( getElementStatus(id).isConnect() )
			return getElementTop( --id );
		else
			return id;
	}

	btVector3 const& getGravityCenter(std::size_t id) const
	{
		assert( id < getElementCount() && "ModelObserver::getGravityCenter()");
		return m_gravityCenterList.at( m_topList.at(id) );
	}

	std::pair<btVector3,btVector3> const&
		getAabb(std::size_t id) const
		{
			assert(
					id < getElementCount() &&
					id < m_topList.size() &&
					"ModelObserver::getAabb()" );
			return m_aabbList.at( m_topList.at(id) );
		}


	std::size_t getLifeTicks() const
	{
		return m_modelLifeTicks;
	}

private:

	template<typename IterType>
		btVector3 calcGravityCenter(
				IterType const top,
				IterType const end)
		{
			btVector3 ret_vec;
			ret_vec.setZero();

			btScalar massSum(0.);
			for (auto const& pair : boost::make_iterator_range(top, end))
			{
				auto const& s = getElementStatus( pair.second );
				auto const mass = s.getInvMass() > SIMD_EPSILON ? btScalar(1.)/s.getInvMass() : 0.f;
				ret_vec += s.getWorldTransform().getOrigin() * mass;
				massSum += mass;
			}

			if (massSum > SIMD_EPSILON)
				return ret_vec / massSum;
			else
				return btVector3(0.,0.,0.);
		}

	template<typename IterType>
		std::pair<btVector3, btVector3>
		calcAabb(
				IterType const top,
				IterType const end
			)
	{
		auto const top_id = top->first;
		auto const& top_status = getElementStatus( top_id );
		auto const& top_trans = top_status.getWorldTransform();

		std::pair<btVector3, btVector3> return_aabb;
		return_aabb.first.setValue(
				SIMD_INFINITY,SIMD_INFINITY, SIMD_INFINITY);
		return_aabb.second.setValue(
				-SIMD_INFINITY,-SIMD_INFINITY, -SIMD_INFINITY);

		for (auto const& id : boost::make_iterator_range(top, end))
		{
			auto const& status = getElementStatus( id.second );
			auto const relative_origin =
				top_trans.invXform(status.getWorldTransform().getOrigin());
			return_aabb.first.setMin(relative_origin);
			return_aabb.second.setMax(relative_origin);
		}

		return_aabb.first  -= btVector3(0.3,0.3,0.3);
		return_aabb.second += btVector3(0.3,0.3,0.3);

		return_aabb.first  = top_trans(return_aabb.first);
		return_aabb.second = top_trans(return_aabb.second);

		return std::move( return_aabb );
	}


};
} // namespace model
} // namespace akasha
#endif /* end of include guard */
