#ifndef AKASHA_MODEL_ELEMENTV2_ELEMENTCONCEPT_HPP_INCLUDED
#define AKASHA_MODEL_ELEMENTV2_ELEMENTCONCEPT_HPP_INCLUDED

#include "detail/ElementParameter.hpp"
#include "../../bullet/btFractureBody.hpp"

#include "../../contactEvent/EventGroup.hpp"
#include "../energy/EnergyManager.hpp"

#include <boost/any.hpp>
#include <boost/type_erasure/any.hpp>
#include <boost/type_erasure/member.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/punctuation/paren.hpp>

#define MEMBER_DEF( name, n) BOOST_TYPE_ERASURE_MEMBER( BOOST_PP_LPAREN() BOOST_PP_CAT(has_, name) BOOST_PP_RPAREN() , name, num)
MEMBER_DEF( update,0)
MEMBER_DEF( getId, 0)
MEMBER_DEF( getDirectType, 0)

MEMBER_DEF( getParentId, 0)
MEMBER_DEF( getTypeId, 0)
MEMBER_DEF( getContactEventType, 0)
MEMBER_DEF( isAdapter, 0)

//Init Process
MEMBER_DEF(initEnergyPhase1, 1)
MEMBER_DEF(initEnergyPhase2, 1)
MEMBER_DEF(initEnergyPhase3, 1)
MEMBER_DEF( initAlignment, 0)
MEMBER_DEF( initWelding, 0)
MEMBER_DEF( initConnect, 2)

	//Update
MEMBER_DEF( energyUpdate, 1)
MEMBER_DEF( preTick, 2)
MEMBER_DEF( tick,    2)
MEMBER_DEF( stepStart, 3)
MEMBER_DEF( stepEnd, 1)
	//Relase
MEMBER_DEF( constraintRelease, 0)

	//Phusics parameter get
MEMBER_DEF( getTransform, 0)
MEMBER_DEF( getWorldVelocity, 0)
MEMBER_DEF( getWorldAnglerVelocity, 0)
MEMBER_DEF( getMass, 0)

	//Physics Operat
MEMBER_DEF( setTransform, 1)
MEMBER_DEF( applyImpulse, 2)
MEMBER_DEF( applyCentralImpulse, 1)
MEMBER_DEF( applyTorqueImpulse, 1)
MEMBER_DEF( getPhysicsBody, 0)

MEMBER_DEF( getParameterPtr, 0)
MEMBER_DEF( forceDisconnect, 0)
#define MEMBER_SET( name ) MEMBER_DEF( BOOST_PP_CAT( set, name), 2)
#define MEMBER_GET( name ) MEMBER_DEF( BOOST_PP_CAT( get, name), 1)
#define MEMBER_PARAM(name) MEMBER_GET(name) MEMBER_SET(name)
	//parameters
MEMBER_PARAM(Angle)
MEMBER_PARAM(Brake)
MEMBER_PARAM(Color)
MEMBER_PARAM(Damper)
MEMBER_PARAM(Effect)
MEMBER_PARAM(Spring)
MEMBER_PARAM(Power)
MEMBER_PARAM(Link)
MEMBER_PARAM(LinkNumber)
#undef MEMBER_PARAM
#undef MEMBER_GET
#undef MEMBER_SET

MEMBER_DEF( getNameString, 0)
MEMBER_DEF(getEnergy, 0)


#undef MEMBER_DEF

#define HAS_MEMBER( name, sig ) BOOST_PP_CAT(has_,name) < sig >
#define HAS_C_MEMBER( name, sig ) \
		BOOST_PP_CAT(has_,name) < sig , const boost::type_erasure::_self>
#define HAS_SETGET( name, paramType ) \
		BOOST_PP_CAT( ::has_set, name) < void ( \
				typename paramType::holderType const, \
				boost::type< paramType> const&)>, \
	BOOST_PP_CAT( ::has_get, name) < \
	typename paramType::holderType (boost::type< paramType > const&)>

	namespace akasha {
	namespace model {
	namespace element_v2 {

	using ElementConcept =
		boost::type_erasure::any<
		boost::mpl::vector<
		boost::type_erasure::copy_constructible<>
		, boost::type_erasure::assignable<>

		, HAS_C_MEMBER( getId, int())
		, HAS_C_MEMBER( getParentId, int())
		, HAS_C_MEMBER( getTypeId, int())
		, HAS_C_MEMBER( isAdapter, bool())
		//Update
		, HAS_MEMBER( preTick,   void(const float, energy::EnergyManager&))
		, HAS_MEMBER( tick,      void(const float, energy::EnergyManager const&))
		, HAS_MEMBER( stepStart, void(float const, float const,energy::EnergyManager&))
		, HAS_MEMBER( stepEnd,   void(energy::EnergyManager const&))
		//Relase
		, HAS_MEMBER( constraintRelease, void())

		//Phusics parameter get
		, HAS_C_MEMBER( getPhysicsBody, boost::shared_ptr<btFractureBody>())

		//Status parameter
		, HAS_C_MEMBER( getParameterPtr, boost::shared_ptr<ElementParameter> const&())
		, HAS_MEMBER( forceDisconnect, void() )
		, HAS_C_MEMBER( getDirectType, element_v2::DirectType const&())
		, HAS_C_MEMBER( getContactEventType, boost::optional<contactEvent::EventGroup>())
		>,
		boost::type_erasure::_self>;

	using ElementInitConcept =
		boost::type_erasure::any<
		boost::mpl::vector<
		boost::type_erasure::copy_constructible<>
		, boost::type_erasure::assignable<>
		, HAS_C_MEMBER( getId, int())
		, HAS_C_MEMBER( getParentId, int())
		, HAS_C_MEMBER( isAdapter, bool())
		//Init Process
		, HAS_MEMBER( initEnergyPhase1, void(energy::ConductorBuilder&))
		, HAS_MEMBER( initEnergyPhase2, void(energy::ConductorBuilder&))
		, HAS_MEMBER( initEnergyPhase3, void(energy::ConductorBuilder&))
		, HAS_MEMBER( initAlignment, void(btTransform const&))
		, HAS_MEMBER( initWelding, void())
		, HAS_MEMBER( initConnect, void(boost::shared_ptr<btFractureBody>,std::vector<boost::shared_ptr<ElementParameter>>))
		//Getter
		, HAS_C_MEMBER( getPhysicsBody, boost::shared_ptr<btFractureBody>())

		>,
		boost::type_erasure::_self>;

	using ElementManipulateConcept =
		boost::type_erasure::any<
		boost::mpl::vector<
		boost::type_erasure::copy_constructible<>
		, boost::type_erasure::assignable<>
		, HAS_C_MEMBER( getId, int())
		, HAS_MEMBER( forceDisconnect, void() )
		//Physics Operat
		, HAS_MEMBER( setTransform, void(btTransform const&))
		, HAS_MEMBER( applyImpulse, void(btVector3 const&,btVector3 const&))
		, HAS_MEMBER( applyCentralImpulse, void(btVector3 const&))
		, HAS_MEMBER( applyTorqueImpulse, void(btVector3 const&))
		, HAS_C_MEMBER( getPhysicsBody, boost::shared_ptr<btFractureBody>())
		>,
		boost::type_erasure::_self>;
	} // namespace element_v2
	} // namespace model
	} // namespace akasha

#undef HAS_MEMBER
#undef HAS_SETGET

#endif // AKASHA_MODEL_ELEMENTV2_ELEMENTCONCEPT_HPP_INCLUDED
