#ifndef AKASHA_MODEL_ELEMENT_V2_LEGACYELEMENT_HPP_
#define AKASHA_MODEL_ELEMENT_V2_LEGACYELEMENT_HPP_

#include "behavior/physicsBehavior.hpp"
#include "behavior/FlexibleJointBehavior.hpp"
#include "behavior/TractionJointBehavior.hpp"
#include "behavior/EnergyExchangeBehavior.hpp"
#include "behavior/JetThrustBehavior.hpp"

#include "detail/ElementBase.hpp"

#include "type_fwd.hpp"

#include <boost/fusion/include/push_back.hpp>

namespace elem_v2 = akasha::model::element_v2;
namespace behav = akasha::model::element_v2::behavior;
namespace event = akasha::model::element_v2::event;

namespace bf = boost::fusion;

namespace detail
{

////////////////////////////////////////////////////////////////////
// Core Behavior
using CoreBehavior =
 bf::vector<
 elem_v2::behavior::PhysicsBehavior,
 elem_v2::behavior::A_TellExchangeBehavior
 >;


////////////////////////////////////////////////////////////////////
// Basic Bavior
using BasicXJointBehaviorTuple = bf::vector<
	behav::FlexibleXJointBehavior,
	behav::PhysicsBehavior,
	behav::A_TellExchangeBehavior>;

using BasicYJointBehaviorTuple = bf::vector<
	behav::FlexibleYJointBehavior,
	behav::PhysicsBehavior,
	behav::A_TellExchangeBehavior>;

using BasicZJointBehaviorTuple = bf::vector<
	behav::FlexibleZJointBehavior,
	behav::PhysicsBehavior,
	behav::A_TellExchangeBehavior>;

////////////////////////////////////////////////////////////////////
// BasicJointBehaviorHelper
template<typename EventType, typename BasicBehaviorType>
struct BasicJointBehaviorInvoke
{
	static void invoke(
			EventType const& e,
			BasicBehaviorType& b,
			elem_v2::ElementParameter& p)
	{
		using behav::helper::PhysicsBehaviorDispather;
		using behav::helper::JointBehaviorDispather;

		PhysicsBehaviorDispather::physicsInvoke(
				e,bf::at_c<1>(b),p);
		JointBehaviorDispather::jointInvoke(
				e, bf::at_c<0>(b),p);
	}
};

template<typename BasicBehaviorType>
struct BasicJointBehaviorInvoke<event::PreTick, BasicBehaviorType>
{
	static void invoke(
			event::PreTick const& e,
			BasicBehaviorType& b,
			elem_v2::ElementParameter& p)
	{
		using behav::helper::PhysicsBehaviorDispather;
		using behav::helper::JointBehaviorDispather;

		PhysicsBehaviorDispather::physicsInvoke(
				e,bf::at_c<1>(b),p);
		JointBehaviorDispather::jointInvoke(
				e, bf::at_c<0>(b),p);

		behav::A_TellExchangeBehavior& exchange =
			bf::at_c<2>( b );
		p.energyLevel(exchange.volume());

		exchange.discharge_all( e.energyManager_);
	}
};

template<typename BasicBehaviorType>
struct BasicJointBehaviorInvoke<event::Tick, BasicBehaviorType>
{
	static void invoke(
			event::Tick const& e,
			BasicBehaviorType& b,
			elem_v2::ElementParameter& p)
	{
		using behav::helper::PhysicsBehaviorDispather;
		using behav::helper::JointBehaviorDispather;

		PhysicsBehaviorDispather::physicsInvoke(
				e,bf::at_c<1>(b),p);
		JointBehaviorDispather::jointInvoke(
				e, bf::at_c<0>(b),p);

		behav::A_TellExchangeBehavior& exchange =
			bf::at_c<2>( b );
		exchange.result( e.energyManager_);
	}
};

template<typename BasicBehaviorType>
struct BasicJointBehaviorInvoke<event::StepEnd, BasicBehaviorType>
{
	static void invoke(
			event::StepEnd const& e,
			BasicBehaviorType& b,
			elem_v2::ElementParameter& p)
	{
		using behav::helper::PhysicsBehaviorDispather;
		using behav::helper::JointBehaviorDispather;

		PhysicsBehaviorDispather::physicsInvoke(
				e,bf::at_c<1>(b),p);
		JointBehaviorDispather::jointInvoke(
				e, bf::at_c<0>(b),p);

	}
};

////////////////////////////////////////////////////////////////////
// Wheel Behavior
using WheelBehavior =
bf::vector<
elem_v2::behavior::TractionJointBehavior
, elem_v2::behavior::PhysicsBehavior
, elem_v2::behavior::A_TellExchangeBehavior
>;

////////////////////////////////////////////////////////////////////
// Jet Behavior
using JetBehavior =
bf::result_of::as_vector< bf::result_of::push_back<
BasicXJointBehaviorTuple
, elem_v2::behavior::JetThrustBehavior
>::type>::type;

}

namespace akasha
{
namespace model
{
namespace element_v2
{
namespace handler
{

//////////////////////////////////////////////////////////////////////
// Core

// Handle
template<typename EventType>
	void CoreHandle(
			EventType const&,
			::detail::CoreBehavior&,
			ElementParameter&){}

// Core Event
template<typename EventType, typename BehaviorType>
struct EventInvoke<
TypeToId< elem_v2::CoreElement >::value,
	EventType, BehaviorType>
{
	static void invoke(
			EventType const& e,
			BehaviorType& b,
			ElementParameter& p)
	{
		using behav::helper::PhysicsBehaviorDispather;

		CoreHandle(e, b, p);
		PhysicsBehaviorDispather::physicsInvoke( e, bf::at_c<0>(b), p);
	}
};

template<>
void CoreHandle(
		event::PreTick const& e,
		::detail::CoreBehavior& b,
		ElementParameter& p)
{
	behavior::A_TellExchangeBehavior&
		exchange = bf::at_c<1>( b );

	p.energyLevel( exchange.volume());
	exchange.comsume_all();
	exchange.discharge(e.energyManager_, 1.0f );
}

template<>
void CoreHandle(
		event::Tick const& e,
		::detail::CoreBehavior& b,
		ElementParameter& p)
{
	behavior::A_TellExchangeBehavior&
		exchange = bf::at_c<1>( b );

	exchange.result(e.energyManager_);
}

template<>
void CoreHandle(
		event::StepEnd const& e,
		::detail::CoreBehavior& b,
		ElementParameter& p)
{

}

////////////////////////////////////////////////////////////////////
// Chip

template<typename EventType, typename BehaviorTupleType>
struct EventInvoke<
TypeToId< elem_v2::ChipElement >::value,
	EventType, BehaviorTupleType>
{
	static void invoke(
			EventType const& e,
			BehaviorTupleType& b,
			ElementParameter& p)
	{
		::detail::BasicJointBehaviorInvoke<
			EventType, BehaviorTupleType>::invoke(
				e,b,p);
	}
};

////////////////////////////////////////////////////////////////////
// Rudder
template<typename EventType, typename BehaviorTupleType>
struct EventInvoke<
TypeToId< elem_v2::RudderElement >::value,
	EventType, BehaviorTupleType>
{
	static void invoke(
			EventType const& e,
			BehaviorTupleType& b,
			ElementParameter& p)
	{
		::detail::BasicJointBehaviorInvoke<
			EventType, BehaviorTupleType>::invoke(
				e,b,p);
	}
};

////////////////////////////////////////////////////////////////////
// Trim
template<typename EventType, typename BehaviorTupleType>
struct EventInvoke<
TypeToId< elem_v2::TrimElement >::value,
	EventType, BehaviorTupleType>
{
	static void invoke(
			EventType const& e,
			BehaviorTupleType& b,
			ElementParameter& p)
	{
		::detail::BasicJointBehaviorInvoke<
			EventType, BehaviorTupleType>::invoke(
				e,b,p);
	}
};

////////////////////////////////////////////////////////////////////
// Frame
template<typename EventType, typename BehaviorTupleType>
struct EventInvoke<
TypeToId< elem_v2::FrameElement >::value,
	EventType, BehaviorTupleType>
{
	static void invoke(
			EventType const& e,
			BehaviorTupleType& b,
			ElementParameter& p
			)
	{
		::detail::BasicJointBehaviorInvoke<
			EventType, BehaviorTupleType>::invoke(
				e,b,p);
	}
};

////////////////////////////////////////////////////////////////////
//FrameRudder
template<typename EventType, typename BehaviorTupleType>
struct EventInvoke<
TypeToId< elem_v2::RudderFrameElement >::value,
	EventType, BehaviorTupleType>
{
	static void invoke(
			EventType const& e,
			BehaviorTupleType& b,
			ElementParameter& p
			)
	{
		::detail::BasicJointBehaviorInvoke<
			EventType, BehaviorTupleType>::invoke(
				e,b,p);
	}
};

////////////////////////////////////////////////////////////////////
// FrameTrim
template<typename EventType, typename BehaviorTupleType>
struct EventInvoke<
TypeToId< elem_v2::TrimFrameElement >::value,
	EventType, BehaviorTupleType>
{
	static void invoke(
			EventType const& e,
			BehaviorTupleType& b,
			ElementParameter& p
			)
	{
		::detail::BasicJointBehaviorInvoke<
			EventType, BehaviorTupleType>::invoke(
				e,b,p);
	}
};

////////////////////////////////////////////////////////////////////
// Rim
template<typename EventType, typename BehaviorTupleType>
struct EventInvoke<
TypeToId< elem_v2::RimElement >::value,
	EventType, BehaviorTupleType>
{
	static void invoke(
			EventType const& e,
			BehaviorTupleType& b,
			ElementParameter& p)
	{
		::detail::BasicJointBehaviorInvoke<
			EventType, BehaviorTupleType>::invoke(
				e,b,p);
	}
};

//////////////////////////////////////////////////////////////////////
// Wheel

// Handle
template<typename EventType>
	void WheelHandle(
			EventType const&,
			::detail::WheelBehavior&,
			ElementParameter&){}

// Event
template<typename EventType, typename BehaviorTupleType>
struct EventInvoke<
TypeToId< WheelElement >::value,
	EventType, BehaviorTupleType>
{
	static void invoke(
			EventType const& e,
			BehaviorTupleType& b,
			ElementParameter& p)
	{
		using behav::helper::PhysicsBehaviorDispather;

		PhysicsBehaviorDispather::physicsInvoke( e, bf::at_c<1>(b), p);
		WheelHandle(e, b, p);
		bf::at_c<2>(b).apply(e);
	}
};

template<>
void WheelHandle(
		event::PreTick const& e,
		::detail::WheelBehavior& b,
		ElementParameter& p)
{
	using power = akasha::model::element::parameter::power;
	using brake = akasha::model::element::parameter::brake;

	behavior::A_TellExchangeBehavior& exchange =
		bf::at_c<2>( b );
	constexpr float ratio( 10000.f );
	float const& powerValue(
			p.getAttribute().getValue(boost::type<power>()));
	float const energy(
			exchange.comsume( std::fabs(powerValue)/ratio) *
			(powerValue>0.f ? 1.f : -1.f));

	bf::at_c<0>(b).traction(
			energy*ratio,
			p.getAttribute().getValue(boost::type<brake>()),
			e.subStep_
			);

	// energy level save
	p.energyLevel( exchange.volume() );

	exchange.discharge_all(e.energyManager_);
}

template<>
void WheelHandle(
		event::Tick const& e,
		::detail::WheelBehavior& b,
		ElementParameter& p)
{
	behavior::A_TellExchangeBehavior& exchange =
		bf::at_c<2>( b );
	exchange.result( e.energyManager_ );
}

template<>
void WheelHandle(
		event::StepEnd const& e,
		::detail::WheelBehavior& b,
		ElementParameter& p)
{
}

////////////////////////////////////////////////////////////////////
// Jet Element
template<typename EventType>
void JetHandle(
		EventType const&,
		::detail::JetBehavior& b,
		ElementParameter& p){}

template<typename EventType, typename BehaviorTupleType>
struct EventInvoke<
TypeToId< JetElement >::value,
	EventType, BehaviorTupleType>
{
	static void invoke(
			EventType const& e,
			BehaviorTupleType& b,
			ElementParameter& p)
	{

		JetHandle(e, b, p);

		::detail::BasicJointBehaviorInvoke<
			EventType, BehaviorTupleType>::invoke(
				e,b,p);
	}
};

template<>
void JetHandle(
		event::PreTick const& e,
		::detail::JetBehavior& b,
		ElementParameter& p)
{
	behavior::A_TellExchangeBehavior& exchange =
		bf::at_c<2>( b );
	behavior::JetThrustBehavior& jet =
		bf::at_c<3>( b );

	using power = akasha::model::element::parameter::power;
	float const& powerValue =
		p.getAttribute().getValue(boost::type<power>());

	constexpr float ratio(10000.f);

	float const energy(
			exchange.comsume(std::fabs(powerValue)/ratio)
			* (powerValue>0.f ? 1.f : -1.f));

	jet.thrust( energy*ratio, e.subStep_ );
}

} // namespace handler
} // namespace element_v2
} // namespace model
} // namespace aksha

////////////////////////////////////////////////////////////////////
// DefineElement
#define DEFINE_ELEMENT(ELEMENT_TYPE, BEHAVIVER) \
class ELEMENT_TYPE :                            \
	public elem_v2::detail::ElementBase<         \
	ELEMENT_TYPE, BEHAVIVER>{}

DEFINE_ELEMENT( elem_v2::CoreElement,   ::detail::CoreBehavior             );
DEFINE_ELEMENT( elem_v2::ChipElement,   ::detail::BasicXJointBehaviorTuple );
DEFINE_ELEMENT( elem_v2::RudderElement, ::detail::BasicYJointBehaviorTuple );
DEFINE_ELEMENT( elem_v2::TrimElement,   ::detail::BasicZJointBehaviorTuple );

DEFINE_ELEMENT( elem_v2::RimElement,    ::detail::BasicXJointBehaviorTuple );
DEFINE_ELEMENT( elem_v2::WheelElement,  ::detail::WheelBehavior            );

DEFINE_ELEMENT( elem_v2::JetElement, ::detail::JetBehavior);

DEFINE_ELEMENT( elem_v2::FrameElement,       ::detail::BasicXJointBehaviorTuple );
DEFINE_ELEMENT( elem_v2::RudderFrameElement, ::detail::BasicYJointBehaviorTuple );
DEFINE_ELEMENT( elem_v2::TrimFrameElement,   ::detail::BasicZJointBehaviorTuple );
#undef DEFINE_ELEMENT


////////////////////////////////////////////////////////////////////
// DefineM Makefunction
#define DEFINE_MAKE_FUNC(TYPE)                                  \
	template<> TYPE* elem_v2::MakeElement( CreateInfo const& d,  \
			boost::shared_ptr<akasha::NodeUpdateTask> const& task, \
			boost::type< TYPE > const&)                            \
{ auto* e = new TYPE(); e->init(d, task); return e; }

DEFINE_MAKE_FUNC( elem_v2::CoreElement   );
DEFINE_MAKE_FUNC( elem_v2::ChipElement   );
DEFINE_MAKE_FUNC( elem_v2::RudderElement );
DEFINE_MAKE_FUNC( elem_v2::TrimElement   );

DEFINE_MAKE_FUNC( elem_v2::RimElement    );
DEFINE_MAKE_FUNC( elem_v2::WheelElement  );

DEFINE_MAKE_FUNC( elem_v2::JetElement );

DEFINE_MAKE_FUNC( elem_v2::FrameElement       );
DEFINE_MAKE_FUNC( elem_v2::RudderFrameElement );
DEFINE_MAKE_FUNC( elem_v2::TrimFrameElement   );
#undef DEFINE_MAKE_FUNC

#endif /* end of include guard */
