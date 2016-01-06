#ifndef AKASHA_MODEL_ELEMENT_V2_ENERGYEXCHANGEBEHAVIOR_HPP_
#define AKASHA_MODEL_ELEMENT_V2_ENERGYEXCHANGEBEHAVIOR_HPP_

#include "detail/EnergyExcahgeImpl.hpp"
#include "../event/event.hpp"

namespace akasha
{
namespace model
{
namespace element_v2
{
namespace behavior
{
namespace detail
{


template<typename EntityType>
struct EnergyExchangeBehaviorBase : EntityType
{

	template<typename PhaseNotifyType>
		void apply(PhaseNotifyType const&){}

	void apply( phase::Initialize const& e)
	{
		EntityType::init();
	}

	void apply( phase::EnergyPortRequest const& r)
	{
		EntityType::request( r.register_ );
	}

	void apply( phase::EnergyPortAccept const& r)
	{
		EntityType::accept( r.accepter_ );
	}

	void apply( phase::EnergyPortComplete const& r)
	{
		EntityType::connect( r.builder_, r.elementID_);
	}
};

} // namespace detail

using A_TellExchangeBehavior =
detail::EnergyExchangeBehaviorBase< detail::A_tellEnergyImpl >;

using AirTankExchangeBehavior =
detail::EnergyExchangeBehaviorBase< detail::AirTankEnergyImpl >;

using HousingExchangeBehavior =
detail::EnergyExchangeBehaviorBase< detail::HousingEnergyImpl >;

using OutputShaftExchangeBehavior =
detail::EnergyExchangeBehaviorBase< detail::OutputShaftEnergyImpl >;

} // namespace behavior
} // namespace element_v2
} // namespace model
} // namespace aksha
#endif /* end of include guard */
