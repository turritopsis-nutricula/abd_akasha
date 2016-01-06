#ifndef AKASHA_MODEL_PHYSICSBEHAIVER_HPP_INCLUDED
#define AKASHA_MODEL_PHYSICSBEHAIVER_HPP_INCLUDED

#include "../event/event.hpp"

#include "../../../BulletWorldManager.h"
#include "../../../RenderType.hpp"

#include "../../../contactEvent/EventGroup.hpp"
#include "../../element/figuer/BasicElementFiguer.hpp"

#include <LinearMath/btMotionState.h>

#include <boost/fusion/mpl/at.hpp>


namespace akasha
{
namespace model
{
namespace element_v2
{
namespace behavior
{
struct PhysicsBehavior
{
public:
	struct MotionState : public btMotionState
	{
		element::figuer::BasicElementFiguer* figure_;
		element_v2::ElementParameter* parameter_;
		btTransform offset_;

		virtual void getWorldTransform(btTransform& t) const
		{
			t = btTransform::getIdentity();
		}
		virtual void setWorldTransform(btTransform const& t)
		{
			figure_->setTransform(t * offset_ );
		}
	};
private:
	boost::shared_ptr< btFractureBody> m_body;
	boost::shared_ptr<btCollisionShape> m_shape;

	element::figuer::BasicElementFiguer m_figuer;
	MotionState m_state;

	ElementParameter* m_baseParameterPtr;

public:
	template<typename EventType>
		void apply(EventType const&){ }

	//Init Arrangement
	void apply(phase::ElementArrangement const& a)
	{
		a.resultTransform_ = m_body->getWorldTransform();
	}


	////////////////////////////////////////////
	// Initialize Event Apply
	// Core Chip Rudder Trim
	CreateSwitch(N, N==0 || N==1 || N==2 || N==6 )
		void apply(phase::Initialize const& i)
		{
			m_baseParameterPtr = i.paramPtr_.get();
			createPanel(i);
			initPhysicsParameter( *m_baseParameterPtr );
		}
	// Rim
	CreateSwitch(N, N==3)
		void apply(phase::Initialize const& i)
		{
			m_baseParameterPtr = i.paramPtr_.get();
			createRim(i);
			initPhysicsParameter( *m_baseParameterPtr );
		}
	//Wheel
	CreateSwitch(N, N==4)
		void apply(phase::Initialize const& i)
		{
			m_baseParameterPtr = i.paramPtr_.get();
			createWheel( i );
			initPhysicsParameter( *m_baseParameterPtr );
		}
	// Jet
	CreateSwitch(N, N==7)
		void apply( phase::Initialize const& i)
		{
			m_baseParameterPtr = i.paramPtr_.get();
			createJet( i );
			initPhysicsParameter( *m_baseParameterPtr );
		}
	// Core Chip Rudder Trim
	CreateSwitch(N, N==33 || N==34 || N==35 )
		void apply( phase::Initialize const& i)
		{
			m_baseParameterPtr = i.paramPtr_.get();
			createPanel( i );
			initPhysicsParameter( *m_baseParameterPtr );
		}
	// Unknown
	void apply(phase::Initialize const& i)
	{
		m_baseParameterPtr = i.paramPtr_.get();
		createUnknown(i);
		initPhysicsParameter( *m_baseParameterPtr );
	}


	//////////////////////////////////////////////

	contactEvent::EventGroup
		getContactGroup(ElementParameter const&) const
	{
		return contactEvent::EventGroup::BasicElement;
	}

	void physicsUpdate( ElementParameter& );

	void forceMove(btTransform const&);

	boost::shared_ptr<btFractureBody> const&
		getPhysicsBody() const
		{
			return m_body;
		}
private:
	//build
	void createPanel(phase::Initialize const&);
	void createRim(phase::Initialize const&);
	void createWheel(phase::Initialize const&);
	void createJet(phase::Initialize const&);
	void createUnknown(phase::Initialize const&);
	void initPhysicsParameter(ElementParameter& p)
	{
		p.setInvMass( m_body->getInvMass() );
	}
};

namespace helper
{

struct PhysicsBehaviorDispather
{
	template<typename TupleType, int N>
	static PhysicsBehavior& getPhysicsBehav(TupleType& b)
	{
		return boost::fusion::at_c<N>(b);
	}
	template<typename EventType>
		static void physicsInvoke(
				EventType const&,
				PhysicsBehavior& b,
				ElementParameter& p
				)
		{}

	static void physicsInvoke(
			event::StepEnd const& e,
			PhysicsBehavior& b,
			ElementParameter& p
			)
	{
		b.physicsUpdate(p);
	}

	static void physicsInvoke(
			event::GetContactEventGroup const& e,
			PhysicsBehavior& b,
			ElementParameter& p
			)
	{
		e.group_ = b.getContactGroup(p);
	}

	static void physicsInvoke(
			event::ForceMove const& f,
			PhysicsBehavior& b,
			ElementParameter& p
			)
	{
		b.forceMove(f.to_);
	}

	static void physicsInvoke(
			event::GetPhysicsBody const& gb,
			PhysicsBehavior& b,
			ElementParameter& p
			)
	{
		gb.body_ =
			b.getPhysicsBody();
	}

};
// template<typename BehaiviorType, int N =0 >
// struct PhysicsBehaviorDispather
// 	{
// 		static PhysicsBehavior& getPhysicsBehav(BehaiviorType& b)
// 		{
// 			return boost::fusion::at_c<N>(b);
// 		}
//
// 		template<typename EventType, typename BehaviorTupleType>
// 			void physicsInvoke(
// 					EventType const&,
// 					BehaviorTupleType&,
// 					ElementParameter&){}
//
//
// 		template<typename BehaviorTupleType>
// 		void physicsInvoke(
// 				event::StepEnd const& e,
// 				BehaviorTupleType& b,
// 				ElementParameter& p
// 				)
// 		{
// 			getPhysicsBehav(b).physicsUpdate(p);
// 		}
//
// 		template<typename BehaviorTupleType>
// 		void physicsInvoke(
// 				event::GetContactEventGroup const& e,
// 				BehaviorTupleType& b,
// 				ElementParameter& p
// 				)
// 		{
// 			e.group_ = getPhysicsBehav(b).getContactGroup(p);
// 		}
//
// 		template<typename BehaviorTupleType>
// 		void physicsInvoke(
// 				event::ForceMove const& f,
// 				BehaviorTupleType& b,
// 				ElementParameter& p
// 				)
// 		{
// 			getPhysicsBehav(b).forceMove(f.to_);
// 		}
//
// 		template<typename BehaviorTupleType>
// 			void physicsInvoke(
// 					event::GetPhysicsBody const& gb,
// 					BehaviorTupleType& b,
// 					ElementParameter& p
// 					)
// 			{
// 				gb.body_ =
// 					getPhysicsBehav(b).getPhysicsBody();
// 			}
// 	};

} // namespace helper
} // namespace behavior
} // namespace element_v2
} // namespace model
} // namespace akasha


#endif // AKASHA_MODEL_PHYSICSBEHAIVER_HPP_INCLUDED
