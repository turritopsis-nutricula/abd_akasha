#ifndef AKASHA_MODEL_ELEMENT_V2_BEHAVIOR_BATTERYBEHAVIOR_HPP_
#define AKASHA_MODEL_ELEMENT_V2_BEHAVIOR_BATTERYBEHAVIOR_HPP_

#include "../../energy/EretnicSocket.hpp"
namespace akasha
{
namespace model
{
namespace element_v2
{
namespace behavior
{
class BatteryBehavior
{
	boost::shared_ptr<energy::ElectricSocket> m_socket;
public:
	template<typename EventType>
		void apply(EventType const&){}

	void apply(phase::Initialize const& init)
	{
		auto& param = init.paramPtr_;
		m_socket = param->addElectricSocket();
		m_socket->volt_ = 0.f;
	}
};
} // namespace behavior
} // namespace element_v2
} // namespace model
} // namespace akasha
#endif /* end of include guard */
