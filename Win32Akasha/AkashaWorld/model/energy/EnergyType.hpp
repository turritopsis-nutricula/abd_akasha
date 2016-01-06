#ifndef AKASHA_MODEL_ENERGY_ENRGY_TYPE_HPP_
#define AKASHA_MODEL_ENERGY_ENRGY_TYPE_HPP_

#include <stdexcept>

namespace akasha
{
	namespace model
{
	namespace energy
{

enum class EnergyType
{
	Undefine,
	IdealGas,
	Traction,
	Turbulence,
	AxisForce,
	A_tell,
	DarkMater,
}; // enum class EnergyType

struct RawNodeID
{
	int raw_value_;
	EnergyType type_;

// 	RawNodeID() : raw_value_(-1), type_(EnergyType::Undefine){}
	
	explicit RawNodeID(int i, EnergyType t) :
		raw_value_(i), type_(t){}

	RawNodeID(RawNodeID const&) = default;
	RawNodeID(RawNodeID&&) = default;
// 	RawNodeID& operator=(RawNodeID&& o) = default;

	int raw_value() const
	{
		return raw_value_;
	}
};

template<EnergyType TypeEnum>
struct NodeID
{
	constexpr static EnergyType typeID = TypeEnum;
	int raw_value_;
	explicit NodeID() = default;

	int raw_value() const
	{
		return raw_value_;
	}

	bool isEqaul(RawNodeID const& r) const
	{
		return typeID==r.type_ && raw_value() == r.raw_value();
	}
};


template<EnergyType N>
inline void NodeID_cast(NodeID<N>& id, RawNodeID const& n)
{
	if (n.type_ == N)
		id.raw_value_ = n.raw_value_;
	else
		throw std::invalid_argument("energy::NodeID_cast not same type.");
}

} // namespace energy
} // namespace model
} // namespace akasha
#endif /* end of include guard */
