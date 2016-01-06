#ifndef AKASHA_MODEL_ENERGY_ENERGYPORTMAP_HPP_
#define AKASHA_MODEL_ENERGY_ENERGYPORTMAP_HPP_

#include "EnergyType.hpp"
#include "../../units/units.hpp"

#include <Riceball/mpl/EasyTuple.hpp>

#include <boost/unordered_map.hpp>
#include <boost/range/algorithm/remove_if.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <boost/range/algorithm_ext/erase.hpp>
#include <boost/noncopyable.hpp>

#include <cmath>
#include <float.h>

namespace akasha
{
namespace model
{
namespace energy
{

template<int>
struct VoidPort{};
struct VoidLink{};

// IdealGas
struct AirPlug
{
	// input fileds
	units::pressure_type pressure_;
	units::length_type   head_;

	units::volume_type        flow_;
	units::flow_velocity_type vel_;
};

// Traction
struct TractionShaft
{
	units::torque_type in_torque_;
	units::torque_type out_torque_;
};

// Turbulence
struct TurbulenceFiled
{
	units::angular_velocity_type in_velocity_;
	units::angular_velocity_type out_velociity_;
};

// AxisDynamics
struct AxisForceParam
{
	units::force_type  axialForce_;
	units::torque_type torque_;
};

// A_tell
struct ATELL_Param
{
	float supply_ = 0.f;
	float receipt_ = 0.f;

	float capacity_left_ = FLT_MAX; //TODO: not init
	float out_capacity_left_ = FLT_MAX;
};
struct ATELL_Stream
{
};

template<EnergyType N> struct Pair{ using type = void;};
template<typename T> struct RPair;
template<EnergyType N> struct Link{ using type = VoidLink; };

// update
template<EnergyType N>
	inline void UpdatePortPair(
		typename Pair<N>::type& port0,
		typename Pair<N>::type& port1,
		typename Link<N>::type& output
		){ }

#define Mapping( N, TYPE)                                                  \
	template<> struct Pair< N >{ using type = TYPE ; };                     \
template<> struct RPair< TYPE > { constexpr static EnergyType value = N; }

#define LinkMapping( N, TYPE) \
	template<> struct Link< N >{ using type = TYPE; }

Mapping( EnergyType::Undefine,   VoidPort<0>     );
Mapping( EnergyType::IdealGas,   AirPlug         );
Mapping( EnergyType::Traction,   TractionShaft   );
Mapping( EnergyType::Turbulence, TurbulenceFiled );
Mapping( EnergyType::AxisForce,  AxisForceParam  );
Mapping( EnergyType::A_tell,     ATELL_Param     );
LinkMapping( EnergyType::A_tell, ATELL_Stream    );
Mapping( EnergyType::DarkMater,  VoidPort<1>     );

#undef LinkMapping
#undef Mapping

//
// template<>
// inline void UpdatePortPair<EnergyType::Traction> (
// 		TractionShaft const& _0, TractionShaft const& _1, TractionShaft&)
// {
// 	assert(false && "not impliment");
// }
//
// template<>
// inline void UpdatePortPair<EnergyType::Turbulence> (
// 		TurbulenceFiled const& _0, TurbulenceFiled const& _1, TurbulenceFiled&)
// {
// // 	auto t = (_0.in_velocity_ + _1.in_velocity_) * 0.5f;
// // 	_0.out_velociity_ = t;
// // 	_1.out_velociity_ = t;
// }
//
// template<>
// inline void UpdatePortPair<EnergyType::AxisForce> (
// 		AxisForceParam const& _0, AxisForceParam const& _1,AxisForceParam& out)
// {
// 	out.axialForce_ = (_0.axialForce_ + _1.axialForce_ );
// 	out.torque_ = (_0.torque_ + _1.torque_);
// }

template<>
inline void UpdatePortPair<EnergyType::A_tell> (
		ATELL_Param& _0, ATELL_Param& _1,
		ATELL_Stream& out)
{
	_0.receipt_ = _1.supply_;
	_1.receipt_ = _0.supply_;

	if (_1.receipt_ > _1.capacity_left_)
		_0.receipt_ += _1.receipt_ - _1.capacity_left_;
	if (_0.receipt_ > _0.capacity_left_)
		_1.receipt_ += _0.receipt_ - _0.capacity_left_;

	_0.out_capacity_left_ = _1.capacity_left_;
	_1.out_capacity_left_ = _0.capacity_left_;
}

} // namespace energy
} // namespace model
} // namespace akasha


namespace akasha
{
namespace model
{
namespace energy
{
namespace detail
{

constexpr EnergyType add(EnergyType i){ return static_cast<EnergyType>( static_cast<int>( i ) + 1); }

template< template<EnergyType> class MapType, EnergyType N, EnergyType END_N>
struct EmunRec
{
	using base_type =  EmunRec<MapType, add( N ), END_N>;
	using type =
		typename rice::mpl::PushBack< typename MapType<N>::type, typename base_type::type>::type;
};
template<template<EnergyType> class MapType, EnergyType END_N>
struct EmunRec<MapType, END_N, END_N>
{
	using type = rice::mpl::TypeTuple< typename MapType<END_N>::type >;
};


template<typename T>
struct Unit
{
	using value_type = T;

	constexpr static EnergyType value =
		akasha::model::energy::RPair<T>::value;
	using link_value =
		typename akasha::model::energy::Link<value>::type;

	using container_type = std::vector< value_type >;
	using link_values_type = std::vector< link_value >;
	container_type container_;
	link_values_type linkValues_;
	using pair_map_type =
		std::vector<std::tuple<int, int, std::size_t >>;
	pair_map_type pairMap_; // <index, index, outputValueIndex>


	int regist()
	{
		container_.emplace_back();
		return container_.size()-1;
	}
	void addPair(
			int const localIndex0,
			int const localIndex1)
	{
		assert( localIndex0 < container_.size() &&
				localIndex1 < container_.size() );

		linkValues_.emplace_back();
		pairMap_.emplace_back(
				localIndex0, localIndex1, linkValues_.size()-1);
	}

	void removePair(
			int const localIndex0,
			int const localIndex1
			)
	{
		assert( localIndex0 < container_.size() &&
				localIndex1 < container_.size() );

		auto new_last_iter = boost::remove_if(
				pairMap_,
				[&](typename pair_map_type::value_type const& v){
				return
				std::get<0>(v) == localIndex0 &&
				std::get<1>(v) == localIndex1;
				});

		std::size_t outputIndex( std::get<2>(*new_last_iter));
		linkValues_.erase(linkValues_.begin()+outputIndex);

		pairMap_.erase( new_last_iter, pairMap_.end());
	}

	void update()
	{
		for( auto& pair : pairMap_)
		{
			assert(
					std::get<0>(pair) < container_.size()   &&
					std::get<1>(pair) < container_.size()   &&
					std::get<2>(pair) < linkValues_.size() &&
					"Unit::Update" );

			akasha::model::energy::UpdatePortPair< value >(
					container_[ std::get<0>(pair)],
					container_[ std::get<1>(pair)],
					linkValues_[ std::get<2>(pair)]
					);
		}
	}

	value_type&
		refNodeValue(int const index)
		{
			assert(
					index < container_.size() &&
					"Unit::refNodeValue");
			return container_[ index ];
		}

	value_type const&
		refNodeValue(int const index) const
		{
			assert(
					index < container_.size() &&
					"Unit::refNodeValue");
			return container_[ index ];
		}
	link_value const&
		refLinkValue(int const index) const
		{
			auto iter = boost::find_if(pairMap_,
					[&index](typename pair_map_type::value_type const& v){
					return std::get<0>(v) == index || std::get<1>(v);
					});
			assert( iter != pairMap_.cend());

			return linkValues_[ std::get<2>(*iter) ];
		}

};
template<typename T>
struct mapper{ using type = Unit<T>; };

struct Holder : boost::noncopyable
{
	// return_type mpl struct
	template<EnergyType N>
		struct return_type
		{
			using unit_type =
				typename mapper< typename akasha::model::energy::Pair<N>::type >::type;
			using type = typename unit_type::container_type;
		};

	// regist visiter
	struct registVisiter
	{
		EnergyType target_;
		std::size_t& index_;

		template<typename T>
			void operator()( T& v) const
			{
				if (target_ == T::value)
					index_ = v.regist();
			}
	};

	// SetPair visiter
	struct setPairVisiter
	{
		int localIndex0_;
		int localIndex1_;
		EnergyType type_;
		bool& result_;

		template<typename T>
			void operator()( T& v) const
			{
				if (type_ == T::value)
				{
					v.addPair(localIndex0_, localIndex1_);
					result_ = true;
				}
			}
	};

	// RemovePair visiter
	struct removePairVisiter
	{
		int localIndex0_;
		int localIndex1_;
		EnergyType type_;
		bool& result_;

		template<typename T>
			void operator()( T& v) const
			{
				if (type_ == T::value)
				{
					v.removePair(localIndex0_, localIndex1_);
					result_ = true;
				}
			}
	};

	//update visiter
	struct updateVisiter
	{
		template<typename T>
			void operator()( T& v) const
			{
				v.update();
			}
	};


	using HolderBase =
		typename rice::mpl::TypeTupleFold<
		typename rice::mpl::ReMap<
		mapper, typename EmunRec<
		akasha::model::energy::Pair, EnergyType::Undefine, EnergyType::DarkMater>::type >::type
		>;

	HolderBase m_base;
	boost::unordered_map<int, std::pair<int, EnergyType> > m_indexMap;

	template<EnergyType N>
		typename return_type<N>::type&
		get()
		{
			using type = typename mapper<
				typename akasha::model::energy::Pair<N>::type >::type;

			return m_base.getValue(boost::type<type>()).container_;
		}
	template<EnergyType N>
		typename return_type<N>::type const&
		get() const
		{
			using type = typename mapper<
				typename akasha::model::energy::Pair<N>::type >::type;

			return m_base.getValue(boost::type<type>()).container_;
		}


	void regist(EnergyType n, int id)
	{
		std::size_t insert_index(-1);
		registVisiter r{ n, insert_index };
		m_base.accept( r );

		if (insert_index >= 0)
		{
			m_indexMap[id] =
				std::make_pair(insert_index, n);
		}
	}

	template<EnergyType N>
		typename return_type<N>::type::value_type&
		ref( int id )
		{
			return get<N>().at( checkAndGetIterator<N>( id ).first );
		}

	template<EnergyType N>
		typename return_type<N>::type::value_type const&
		const_ref( int id) const
		{
			return get<N>().at( checkAndGetIterator<N>( id ).first );
		}

template<EnergyType N>
	typename return_type<N>::type::value_type&
	ref_node( int id )
	{
			using unit_type = typename mapper<
				typename akasha::model::energy::Pair<N>::type >::type;
			return m_base.getValue(
					boost::type<unit_type>()).refNodeValue(
						checkAndGetIterator<N>(id).first);
	}

template<EnergyType N>
	typename return_type<N>::type::value_type const&
	ref_node( int id ) const
	{
			using unit_type = typename mapper<
				typename akasha::model::energy::Pair<N>::type >::type;
			return m_base.getValue(
					boost::type<unit_type>()).refNodeValue(
						checkAndGetIterator<N>(id).first);
	}

	template<EnergyType N>
		typename return_type<N>::type::link_value const&
		ref_link( int id ) const
		{
			using unit_type = typename mapper<
				typename akasha::model::energy::Pair<N>::type >::type;

			return m_base.getValue(
					boost::type<unit_type>()).refLinkValue(
					checkAndGetIterator<N>( id ).first);
		}

		template<EnergyType N>
		std::pair< int, EnergyType> const&
		checkAndGetIterator( int id ) const
		{
			auto index_iter =  m_indexMap.find(id);
			assert( index_iter!= m_indexMap.end() && " Not regist.");

			auto const& index = index_iter->second;
			assert(index.second == N && "Index error");

			return index;
		}

	bool setPair(int const index0, int const index1)
	{
		auto iter0 = m_indexMap.find(index0);
		auto iter1 = m_indexMap.find(index1);
		if ( iter0 == m_indexMap.end() || iter1 == m_indexMap.end())
			return false;

		if (iter0->second.second != iter1->second.second)
			return false;

		bool result(false);
		m_base.accept(
				setPairVisiter{
				iter0->second.first, iter1->second.first,
				iter0->second.second, result});
		return result;
	}

	bool removePair(int const index0, int const index1)
	{
		auto iter0 = m_indexMap.find(index0);
		auto iter1 = m_indexMap.find(index1);
		if ( iter0 == m_indexMap.end() || iter1 == m_indexMap.end())
			return false;

		if (iter0->second.second != iter1->second.second)
			return false;

		bool result(false);
		m_base.accept(
				removePairVisiter{iter0->second.first, iter1->second.first,
				iter0->second.second, result});
		return result;
	}

	bool find(int id) const
	{
		return m_indexMap.find(id) != m_indexMap.end();
	}

	bool getType(int id, EnergyType& result) const
	{
		auto i = m_indexMap.find(id);
		if (m_indexMap.find(id) != m_indexMap.end())
		{
			result = i->second.second;
			return true;
		}
		return false;
	}

	void update()
	{
		m_base.accept(
				updateVisiter());
	}

};

} // namespace detail


class EnergyPortMap : public detail::Holder{};

} // namespace energy
} // namespace model
} // namespace akasha
#endif /* end of include guard */
