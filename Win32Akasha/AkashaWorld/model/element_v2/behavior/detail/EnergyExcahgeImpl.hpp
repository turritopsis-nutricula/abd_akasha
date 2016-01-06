#ifndef AKASHA_MODEL_ELEMENT_V2_BEHAVIOR_DETAIL_ENERGYEXCHANGE_IMPL_HPP
#define AKASHA_MODEL_ELEMENT_V2_BEHAVIOR_DETAIL_ENERGYEXCHANGE_IMPL_HPP

#include "../../../energy/EnergyManager.hpp"
#include "../../direct/DefineParameter.hpp"

#include <boost/range/numeric.hpp>

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

using PortRegisterType =
	energy::ConductorBuilder::RegistDispatcher;
using PortAcceptType =
	energy::ConductorBuilder::AcceptDispatcher;

//AirTank
struct AirTankEnergyImpl
{
	static constexpr energy::EnergyType gas_type =
		energy::EnergyType::IdealGas;
	using port_type = energy::NodeID< gas_type >;

	boost::optional<port_type> base_;
	std::vector<energy::RawNodeID> ports_;

	void init(){}

	void request( PortRegisterType& r )
	{
		base_ =
			r.regist<gas_type>(energy::ConductDirect::Parent);
	}
	void accept( PortAcceptType& a)
	{
		auto const& request = a.getRequests();
		for (auto const& r : request)
		{
			if (r.nodeID_.type_ == energy::EnergyType::IdealGas)
			{
				ports_.push_back( r.nodeID_);
				a.accept(r);
			}
		}

	}
	void connect(
			energy::ConductorBuilder const& b,int elementID)
	{
		if(!b.isAccept(*base_))
			base_ = boost::none;
	}

	void setPressure(
			energy::EnergyManager& m, units::pressure_type const& p)
	{
		if (auto id = base_)
			m.refNode(*id).pressure_;

	}
};

/// HousingEnergyImpl
struct HousingEnergyImpl
{
	bool isActiveAxis_;

	std::vector<
		energy::NodeID<energy::EnergyType::AxisForce>> axisIDs_;
	std::vector<
		energy::NodeID<energy::EnergyType::IdealGas>> gasIds_;

	void init()
	{
		namespace si = units::si;
		isActiveAxis_ = false;
	}

	void request( PortRegisterType& r ){}

	void accept( PortAcceptType& a)
	{
		energy::NodeID<energy::EnergyType::AxisForce> buf;
		energy::NodeID<energy::EnergyType::IdealGas> bufGas;

		for (auto const& i : a.getRequests())
		{
			// AxisForce
			if (
					i.nodeID_.type_ == energy::EnergyType::AxisForce &&
					i.fromConnectType_ == ConnectType::Output
				)
				{
					energy::NodeID_cast(buf, i.nodeID_);
					axisIDs_.push_back(buf);
					a.accept(i);
				}

			// IdealGas
			if (i.nodeID_.type_ == energy::EnergyType::IdealGas &&
					i.fromConnectType_ != ConnectType::Output)
			{
				energy::NodeID_cast(bufGas, i.nodeID_);
				gasIds_.push_back(bufGas);
				a.accept(i);
			}
		}
	}

	void connect( energy::ConductorBuilder const& b,int id)
	{
	}

	void synchronize()
	{
	}

	void traction(
			energy::EnergyManager& em,
			units::torque_type const& accel
			)
	{
		for (auto const id : axisIDs_)
		{
			em.portAccess(id,
					[accel](energy::AxisForceParam& s){
					s.torque_ = accel;
					});
		}
	}
};

/// MultiFinImpl
struct MultiFinImpl
{
	boost::optional<
	energy::NodeID<energy::EnergyType::AxisForce>> axis_;

	void init(){}
	void request( PortRegisterType& r){}
	void accept( PortAcceptType& a){}
	void connect( energy::ConductorBuilder const& b, int id){}

	units::torque_type
		getTorque(energy::EnergyManager const& m)
		{
			units::torque_type t;

			if (auto const id = axis_)
				m.portAccess(*id,
						[&t](energy::AxisForceParam const& f)
						{ t = f.torque_;});

			return t;
		}

	units::force_type
		getAxisForce(energy::EnergyManager const& m)
		{
			units::force_type f;

			if (auto const id = axis_)
				m.portAccess(*id,
						[&f](energy::AxisForceParam const& p)
						{ f = p.axialForce_;});

			return f;
		}

};

/// OutputShaftEnergyImpl
//
struct OutputShaftEnergyImpl
{
	boost::optional<
	energy::NodeID<energy::EnergyType::AxisForce>> axisID_;

	void init(){}
	void request( PortRegisterType& r )
	{
		axisID_ = r.regist<energy::EnergyType::AxisForce>(
				energy::ConductDirect::Parent);
	}
	void accept( PortAcceptType& a)
	{
	}
	void connect( energy::ConductorBuilder const& b, int)
	{

		if (!b.isAccept(axisID_->raw_value()))
			axisID_ = boost::none;
	}

	units::torque_type getTorque(
			energy::EnergyManager const& em)
	{
		units::torque_type torque;
		if (auto id = axisID_)
			torque = em.refNode(*id).torque_;

		return torque;
	}
};

/// A_tell
struct A_tellEnergyImpl
{
	boost::optional<
		energy::NodeID<energy::EnergyType::A_tell>> toParentId_;
	std::vector<
		energy::NodeID<energy::EnergyType::A_tell>> a_tell_ports_; //RowPortID


	float volume_;
	float capacity_;
	float buf_left_;

	void init(){
		volume_ = 0.f;
		capacity_ = 1.f;
		buf_left_ = capacity_;
	}

	void request( PortRegisterType& r)
	{
		toParentId_ = r.regist<energy::EnergyType::A_tell>(
				energy::ConductDirect::Parent);
	}
	void accept( PortAcceptType& a)
	{
		energy::NodeID<energy::EnergyType::A_tell> buf;
		for (auto const& request : a.getRequests())
		{
			if (toParentId_->isEqaul(request.fromPortID_))
				continue;

			if (request.nodeID_.type_ == energy::EnergyType::A_tell)
			{
				energy::NodeID_cast( buf, request.nodeID_);
				a_tell_ports_.push_back( buf );
				a.accept( request );
			}
		}
	}

	void connect( energy::ConductorBuilder const& b, int num)
	{
		if(!b.isAccept(*toParentId_))
			toParentId_ = boost::none;
		else
			a_tell_ports_.push_back( *toParentId_);
	}

	void stanby(energy::EnergyManager& m)
	{
		supply(m, 0.f);
	}
	void stanby(energy::EnergyManager& m, float const p)
	{
		supply(m, p);
	}

	void discharge(energy::EnergyManager& m, float const p)
	{
		supply(m,p);
	}
	void discharge_all(energy::EnergyManager& m)
	{
		supply(m, comsume_all() );
	}
	float result(energy::EnergyManager const& m)
	{
		receipt(m);
		return volume_;
	}

	void supply(energy::EnergyManager& m, float const& e)
	{
		if (a_tell_ports_.empty()) return;

		std::size_t const& size( a_tell_ports_.size() );

		float const ne(e/ size);

		assert(buf_left_ > -FLT_EPSILON );
		float const mine_left(  buf_left_/ size );

		for(auto const& id : a_tell_ports_)
		{
			auto& n = m.refNode(id);
			n.supply_ = ne;
			n.capacity_left_ = mine_left;
		}
	}

	void receipt(energy::EnergyManager const& m)
	{
		for(auto const& id : a_tell_ports_)
			volume_ += m.refNode(id).receipt_;

		volume_ = std::min(capacity_, volume_);
		// save left
		buf_left_ = capacity_ - volume_;
	}

	// comsume energy
	float comsume(float const& f)
	{
		float sub = std::min(volume_, std::fabs(f));
		volume_ -= sub;
		return sub;
	}
	// comsume energy
	float comsume_all()
	{
		return comsume(volume_);
	}
	float volume() const{
		return volume_;
	}
	void volume(float const f)
	{
		volume_ = f;
	}

};

} // namespace detail
} // namespace behavior
} // namespace element_v2
} // namespace model
} // namespace aksha
#endif /* end of include guard */
