#ifndef AKASHA_MODEL_ELEMENT_V2_BASICCHIPELEMENT_IPP_
#define AKASHA_MODEL_ELEMENT_V2_BASICCHIPELEMENT_IPP_

#include "behavior/PhysicsBehavior.hpp"
#include "behavior/FlexibleJointBehavior.hpp"
#include "behavior/TractionJointBehavior.hpp"

#include "behavior/EnergyExchangeBehavior.hpp"

#include "detail/ElementBase.hpp"

#include "type_fwd.hpp"

#include <boost/fusion/include/push_back.hpp>
#include <boost/fusion/include/pop_back.hpp>

namespace bf = boost::fusion;

namespace elem = akasha::model::element;
namespace elem_v2 = akasha::model::element_v2;
namespace behav = elem_v2::behavior;
namespace handler = elem_v2::handler;
namespace event = elem_v2::event;




////////////////////////////////////////////////////////////////////
// BasicChipsHanlder
template<typename BehaviorTupleType, typename JointConceptType>
	struct BasicElementHandler
{

	template<typename EventType>
		void invoke(
				EventType const& e,
				BehaviorTupleType& b,
				elem_v2::ElementParameter& p)
		{
				behav::helper::PhysicsBehaviorDispather::physicsInvoke(
						e,bf::at_c<1>(b),p);
				behav::helper::JointBehaviorDispather::jointInvoke(
						e, bf::at_c<0>(b),p);
		}

};



////////////////////////////////////////////////////////////////////
// Basic Bavior
using BasicXJointBehaviorTuple = bf::vector<
behav::FlexibleXJointBehavior,
	behav::PhysicsBehavior>;

using BasicYJointBehaviorTuple = bf::vector<
behav::FlexibleYJointBehavior,
	behav::PhysicsBehavior>;

using BasicZJointBehaviorTuple = bf::vector<
behav::FlexibleZJointBehavior,
	behav::PhysicsBehavior>;


////////////////////////////////////////////////////////////////////////
// AirTank
using AirTankBehavior =
bf::result_of::as_vector<
bf::result_of::push_back< BasicXJointBehaviorTuple, behav::AirTankExchangeBehavior >::type>::type;

////////////////////////////////////////////////////////////////////
// Housing
using HousingBehavior =
bf::result_of::as_vector<
bf::result_of::push_back< BasicXJointBehaviorTuple, behav::HousingExchangeBehavior>::type>::type;

struct HousingHandler
{

	static behav::HousingExchangeBehavior&
		getExchange( HousingBehavior& b)
		{
			return bf::at_c<2>( b );
		}

	template<typename EventType>
		void invoke(EventType const& e,
				HousingBehavior& b,
				elem_v2::ElementParameter& p)
		{
			behav::helper::JointBehaviorDispather::jointInvoke(
					e, bf::at_c<0>(b), p);
			behav::helper::PhysicsBehaviorDispather::physicsInvoke(
					e, bf::at_c<1>(b),p);
		}

	void invoke(
			event::PreTick const& e,
			HousingBehavior& b,
			elem_v2::ElementParameter& p
			)
	{
		behav::helper::JointBehaviorDispather::jointInvoke(
				e, bf::at_c<0>( b ), p);

		using power = akasha::model::element::parameter::power;
		namespace u = akasha::units;
		namespace si = u::si;
		float power_value =p.getAttribute().getValue( boost::type<power>());
		getExchange(b).traction(
				e.energyManager_,
				u::torque_type(power_value * si::newton * si::meter / si::radian ));
	}
};

class elem_v2::HousingElement :
	public elem_v2::detail::ElementBase<
	elem_v2::HousingElement,
	HousingBehavior,
	HousingHandler>
{};

//////////////////////////////////////////////////////////////////
// OutputShaft
template<typename OutputBehaviorType>
struct OutputHandler :
	handler::TractionJointHandler
// 	behav::helper::PhysicsBehaviorDispather<OutputBehaviorType, 1>
{

	behav::TractionJointBehavior&
		getJoint(OutputBehaviorType& b) const
		{
			return boost::fusion::at_c<0>( b );
		}

	behav::OutputShaftExchangeBehavior&
		getExchange(OutputBehaviorType& b) const
		{
			return boost::fusion::at_c<2>( b);
		}

	template<typename EventType>
		void invoke(
				EventType const& e,OutputBehaviorType& b, elem_v2::ElementParameter& p)
		{
			behav::helper::PhysicsBehaviorDispather::physicsInvoke (
					e, bf::at_c<1>(b), p);
		}

	// PreTick
	void invoke(
			event::PreTick const& e,OutputBehaviorType& b, elem_v2::ElementParameter& p)
	{

		getJoint(b).traction(
				getExchange(b).getTorque(e.energyManager_).value(),
				0.f,
				e.subStep_
				);
	}

};

using OutputShaftBehavior =
bf::vector<
elem_v2::behavior::TractionJointBehavior ,
	elem_v2::behavior::PhysicsBehavior,
	behav::OutputShaftExchangeBehavior>;

class elem_v2::OutputShaftElement :
	public elem_v2::detail::ElementBase<
	elem_v2::OutputShaftElement,
	OutputShaftBehavior,
	OutputHandler<OutputShaftBehavior>>
{};


#define DEFINE_MAKE_FUNC(TYPE)                                  \
	template<> TYPE* elem_v2::MakeElement( CreateInfo const& d,  \
			boost::shared_ptr<akasha::NodeUpdateTask> const& task, \
			boost::type< TYPE > const&)                            \
{ auto* e = new TYPE(); e->init(d, task); return e; }

#define MAKE_BASIC_ELEMENTS(TYPE, BEHAIVE, JointIndex)    \
	class TYPE :                                           \
		public elem_v2::detail::ElementBase<                \
		TYPE, BEHAIVE,                                      \
		BasicElementHandler<                                \
		BEHAIVE,                                            \
		bf::result_of::at_c<BEHAIVE,JointIndex>::type >>{}; \
DEFINE_MAKE_FUNC( TYPE )

//Core
// DEFINE_MAKE_FUNC( elem_v2::CoreElement );

//Chip, Rudder, Trim
// MAKE_BASIC_ELEMENTS( elem_v2::ChipElement,   BasicXJointBehaviorTuple, 0)
// MAKE_BASIC_ELEMENTS( elem_v2::RudderElement, BasicYJointBehaviorTuple, 0);
// MAKE_BASIC_ELEMENTS( elem_v2::TrimElement,   BasicZJointBehaviorTuple, 0);

//RIM
// MAKE_BASIC_ELEMENTS( elem_v2::RimElement, BasicXJointBehaviorTuple, 0 );

//AirTank
MAKE_BASIC_ELEMENTS( elem_v2::AirTankElement, AirTankBehavior, 0);

// Housing
DEFINE_MAKE_FUNC( elem_v2::HousingElement );

//OutputShaft
DEFINE_MAKE_FUNC( elem_v2::OutputShaftElement );


#undef MAKE_BASIC_ELEMENTS
#undef DEFINE_MAKE_FUNC

#endif /* end of include guard */
