#ifndef AKASHA_MODEL_BEHAVIOR_JETTHRUST_HPP_
#define AKASHA_MODEL_BEHAVIOR_JETTHRUST_HPP_

#include "../event/event.hpp"

namespace akasha
{
namespace model
{
namespace element_v2
{
namespace behavior
{

class JetThrustBehavior
{
	boost::shared_ptr<btRigidBody> m_body;
public:
	template<typename T>
		void apply(T const&){ }

	void apply( phase::ElementConnect const& c)
	{
		m_body = c.mineBody_;
	}
	void thrust(float const impluse, float step)
	{
		assert(m_body);
		btTransform const& t = m_body->getWorldTransform();
		m_body->applyCentralImpulse(
				t.getBasis().getColumn(1) * impluse * step);
	}
};

} // namespace behavior
} // namspace element_v2
} // namespace model
} // namespace akasha
#endif /* end of include guard */
