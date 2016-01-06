#ifndef ELEMENTBASE_HPP_INCLUDED
#define ELEMENTBASE_HPP_INCLUDED

#include "../event/event.hpp"
#include "ElementParameter.hpp"
#include "../type_fwd.hpp"

#include "../../../Receptor/Receptors.hpp"
#include "../../../contactEvent/EventGroup.hpp"
#include "../../energy/EnergyManager.hpp"

#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/include/for_each.hpp>


namespace akasha
{
namespace model
{
namespace element_v2
{

namespace handler
{

struct VoidHandler
{
	template<typename BehaiviorType, typename EventType>
		void invoke( EventType const&,BehaiviorType&, ElementParameter&)
		{
		}
	template<typename BehaiviorType, typename EventType>
		void invoke( EventType const&,BehaiviorType&, ElementParameter&) const
		{
		}
};

template<int TypeId, typename EventType,  typename BehaviorTupleType>
struct EventInvoke
{
	static void invoke(
	EventType const&,
	BehaviorTupleType& b,
	ElementParameter&){}
};



} // namespace handler

namespace detail
{
namespace parameter = element::parameter;


extern void* enabler;
template<int N, typename T, typename A>
	class has_apply
	{
	private:
		template<typename U>
			static auto check(U&& v) ->
			decltype(
					v.template apply<N>(std::declval<A>()),
					std::true_type());
		static auto check(...) ->
			decltype( std::false_type());
	public:
		typedef decltype( check( std::declval<T>())) type;
		static bool const value = type::value;
	};


template<typename Type, typename BehaviorSeqType, typename EventHandlerType = handler::VoidHandler>
	struct ElementBase : private boost::noncopyable
{

public:
	using behaviorType = BehaviorSeqType;
private:
	typedef ElementParameter statusType;
	typedef ElementBase< Type, BehaviorSeqType, EventHandlerType> this_type;
	constexpr static int TypeNum = TypeToId< Type >::value;
	BehaviorSeqType m_behaviors;
	boost::shared_ptr<statusType> m_parameter;
	EventHandlerType m_handler;

	typedef ElementReceptor<this_type, TypeNum> ReceptorType;
	ReceptorType m_receptor;
	entity::EntityReceptor m_receptorWrapper;

	//EventInvoker
	template<int N, typename EventType>
		struct EventInvoker
		{
			EventType const& e_;

			template<typename T,
				typename std::enable_if<
					has_apply<N, T, EventType>::value>::type*&
					= enabler>
					void operator()(T& t) const
					{
						t.template apply<N>(e_);
					}
			template<typename T,
				typename std::enable_if<
					!has_apply<N, T, EventType>::value>::type*&
					= enabler>
					void operator()(T& t) const
					{
						t.apply(e_);
					}
		};
public:
	template<typename EventType>
		void eventInvoke(EventType const& e)
		{
			m_handler.invoke( e, m_behaviors, *m_parameter);
			handler::EventInvoke< TypeNum, EventType,  behaviorType>::invoke(
					e, m_behaviors, *m_parameter);
		}
	template<typename EventType>
		void eventInvoke(EventType const& e) const
		{
			m_handler.invoke( e, m_behaviors, *m_parameter);
		}

	// This function behaiviors only
	template<typename PhaseNotifyType>
	void phaseInvoke(PhaseNotifyType const& e)
	{
			boost::fusion::for_each(
					m_behaviors,
					EventInvoker<
					TypeNum, PhaseNotifyType>{ boost::ref(e)}
					);
	}
public:
	void init(CreateInfo const& info,
			boost::shared_ptr<NodeUpdateTask> const& nodeTask)
	{
		//Parameter Init
		m_parameter = boost::make_shared<statusType>();
		m_parameter->init(getTypeId(),isAdapter(), info );

		this->phaseInvoke(
				phase::Initialize{boost::ref(info),  m_parameter, nodeTask});

		//Recepter
		m_receptor.this_ = this;
		m_receptorWrapper = entity::EntityReceptor( m_receptor );

		//SetTransform
		setWorldTransform(btTransform::getIdentity());
	}
	void energyInitPhase1(energy::ConductorBuilder& b)
	{
		auto r = b.createRegister(this->getId());
		this->phaseInvoke(phase::EnergyPortRequest{boost::ref(r)} );

	}
	void energyInitPhase2(energy::ConductorBuilder& b)
	{
		auto a = b.conductorReport(this->getId());
		this->phaseInvoke(phase::EnergyPortAccept{boost::ref(a)} );
	}
	void energyInitPhase3(energy::ConductorBuilder& b)
	{
		this->phaseInvoke(
				phase::EnergyPortComplete{boost::ref(b), this->getId()});
	}
	//TODO : dummy
	float shotHit(btVector3 const&, btVector3 const&, float){
		return 0.f;
	}

	constexpr int getTypeId() const
	{
		return TypeNum;
	}
	constexpr bool isAdapter() const
	{
		return IsAdapterFromId< TypeNum >();
	}
	int getId() const
	{
		return m_parameter->id();
	}
	int getParentId() const
	{
		return m_parameter->parentId();
	}
	element_v2::DirectType const&
		getDirectType() const
		{
			return m_parameter->getDirectType();
		}
	boost::shared_ptr<statusType> const& getStatusPtr() const
	{
		return m_parameter;
	}
	std::string const& getNameString() const
	{
		return m_parameter->getNameString();
	}
	// 				, HAS_C_MEMBER( getWorldAnglerVelocity, btVector3 const&())
	boost::shared_ptr<btFractureBody>
		getPhysicsBody()
		{
			boost::shared_ptr<btFractureBody> b;
			event::GetPhysicsBody e{ boost::ref( b )};
			this->eventInvoke( e );
			return b;
		}
	void setWorldTransform( btTransform const& s)
	{
		m_parameter->setWorldTransform(s);
		this->eventInvoke(event::ForceMove{s});
	}

	//Update
	void stepStart( float step, float sub, energy::EnergyManager& em)
	{
		event::StepStart e{ step, sub,  em };
		this->eventInvoke( e );
	}
	void stepEnd(energy::EnergyManager const& em)
	{
		event::StepEnd e{ em };
		this->eventInvoke( e );
	}

	//ParameterSet
	template<typename parameterType, typename ValueType>
		void setParameter(const ValueType v, boost::type<parameterType> const&)
		{
			m_parameter->getAttribute().setValue( boost::type<parameterType>(), v);
			typedef event::ParameterUpdate<parameterType> event_type;
			this->eventInvoke<event_type>( event_type() );
		}
	template<typename parameterType>
		typename parameterType::holderType
		getParameter(boost::type<parameterType> const& t) const
		{
			return m_parameter->getAttribute().getValue(t);
		}

};


} //namespace detail
} //namespace element_v2
}//namespace model
} //namespace akasha

#define PARAM_SETGET(Name, paramType) \
	void set ## Name (typename paramType::holderType const v) \
{ this_->setParameter(v, boost::type< paramType >()); } \
typename paramType::holderType get ## Name () const \
{ return this_->getParameter(boost::type< paramType >()); }


namespace akasha {
namespace model {
namespace element_v2 {

template<typename T>
	struct ElementHolder
	{
		boost::shared_ptr<T> this_;
		int getId() const
		{
			return this_->getId();
		}
		int getParentId() const
		{
			return this_->getParentId();
		}
		constexpr int getTypeId() const
		{
			return this_->getTypeId();
		}
		constexpr bool isAdapter() const
		{
			return this_->isAdapter();
		}

		boost::optional<contactEvent::EventGroup>
			getContactEventType() const
			{
				boost::optional<
					contactEvent::EventGroup> result(boost::none);

				this_->eventInvoke(
						event::GetContactEventGroup{result}
						);

				//TODO
				return result;
			}
		element_v2::DirectType const&
			getDirectType() const
			{
				return this_->getDirectType();
			}
		//TODO: ElementParameter const
		boost::shared_ptr<ElementParameter > const&
			getParameterPtr() const
			{
				return this_->getStatusPtr();
			}

		void initEnergyPhase1(energy::ConductorBuilder& b)
		{
			this_->energyInitPhase1(b);
		}
		void initEnergyPhase2(energy::ConductorBuilder& b)
		{
			this_->energyInitPhase2(b);
		}
		void initEnergyPhase3(energy::ConductorBuilder& b)
		{
			this_->energyInitPhase3(b);
		}
		void initWelding(){//TODO
		}
		void initConnect(
				boost::shared_ptr<btFractureBody> parentBody,
				std::vector<boost::shared_ptr<ElementParameter>> parentsParam)
		{
			assert( parentBody );

			//rerative trans

			btMatrix3x3 relativeRotation(
					direct::GetRelativeRotationBasis((*parentsParam.begin())->getDirectType())
					);

			this_->phaseInvoke(
					phase::ElementConnect{
					getDirectType(),
					parentBody,
					getPhysicsBody(),
					*getParameterPtr(),
					parentsParam,
					relativeRotation
					});
		}
		//Init Process
		void initAlignment(btTransform const& parentTransform)
		{
			btTransform result( getParameterPtr()->getWorldTransform() );
			phase::ElementArrangement e{
				getDirectType(),
					parentTransform,
					boost::ref(result)};
			this_->phaseInvoke( e );
			setTransform( result );
		}

		//Update
		void preTick(const float f, energy::EnergyManager& em)
		{
			this_->eventInvoke( event::PreTick{f, boost::ref(em)} );
		}
		void tick(float const f, energy::EnergyManager const& em)
		{
			this_->eventInvoke( event::Tick{f,boost::ref(em)});
		}
		void stepStart(
				const float step,const float sub, energy::EnergyManager& em)
		{
			this_->stepStart( step, sub, em);
		}
		void stepEnd(energy::EnergyManager const& em)
		{
			this_->stepEnd(em);
		}
		//Relas
		void constraintRelease()
		{
			//TODO
		}

		//Phusics parameter get

		//Physics Operat
		void setTransform(btTransform const& s)
		{
			this_->setWorldTransform(s);
		}
		void applyImpulse(btVector3 const& impluse,btVector3 const& pos)
		{ //TODO

		}
		void applyCentralImpulse(btVector3 const& implse)
		{ //TODO

		}
		void applyTorqueImpulse(btVector3 const& tourq)
		{ //TODO

		}

		void forceDisconnect()
		{
			this_->eventInvoke(event::ForceDisconnect());
			this_->getStatusPtr()->setConnectStatus(false);
		}
		boost::shared_ptr<btFractureBody>
			getPhysicsBody() const
			{
				return this_->getPhysicsBody();
			}


		//parameters
		PARAM_SETGET(Angle, element::parameter::angle)
			PARAM_SETGET(Brake, element::parameter::brake)
			PARAM_SETGET(Color, element::parameter::color)
			PARAM_SETGET(Damper, element::parameter::damper)
			PARAM_SETGET(Effect, element::parameter::effect)
			PARAM_SETGET(Spring, element::parameter::spring)
			PARAM_SETGET(Power, element::parameter::power)
			PARAM_SETGET(Link, element::parameter::link)
			PARAM_SETGET(LinkNumber, element::parameter::linkNumber)
			std::string const&
			getNameString() const
			{
				return this_->getNameString();
			}
		float getEnergy() const
		{
			return 0.f;
		}

	};

} // namespace eleme
} // namespace model
} // namespace akasha


#endif // ELEMENTBASE_HPP_INCLUDED
