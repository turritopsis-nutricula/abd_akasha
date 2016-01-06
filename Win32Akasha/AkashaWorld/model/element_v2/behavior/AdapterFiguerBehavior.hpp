#ifndef AKASHA_MODEL_ADAPTER_FIGUER_BEHAVIOR_HPP_
#define AKASHA_MODEL_ADAPTER_FIGUER_BEHAVIOR_HPP_

#include "../event/event.hpp"

#include "../../element/figuer/BasicElementFiguer.hpp"
#include <LinearMath/btMotionState.h>

#include "../detail/AdapterParameter.hpp"

namespace akasha
{
namespace model
{
namespace element_v2
{
namespace behavior
{
//fwd
void adapterTransform(float const angle, btTransform& result);

struct AdapterFiguerFiguerBehavior
{
	using figuer_type = element::figuer::BasicElementFiguer;
	struct MotionState : public btMotionState
	{
	public:
		figuer_type* figure_;
		virtual void getWorldTransform(btTransform& t) const override
		{
			t = btTransform::getIdentity();
		}
		virtual void setWorldTransform(btTransform const& t) override
		{
			figure_->setTransform(t);
		}
	};
private:
	figuer_type m_figuer;
	MotionState m_state;
	boost::shared_ptr<ElementParameter> m_adapteeElementParam;

public:
	template<typename EventType>
		void apply(EventType const& e){}

	void apply(phase::Initialize const& init)
	{
		m_figuer.create("adapter.x", init.info_.irrEx_, init.nodeUpdateTask_);
		auto& parameter = init.paramPtr_;
		assert( parameter->isAdapter() );
		auto& adapter = parameter->getAdapterParameter();
		auto& transform = adapter->getTransformData();

		transform = btTransform::getIdentity();
		float const angle = parameter->getAttribute().getValue(
				boost::type<element::parameter::angle>());
		adapterTransform(angle, transform);
	}

	void apply(phase::ElementConnect const& c)
	{
		connect( c );
	}

//////////////////////////////////////////////
	void connect(phase::ElementConnect const& connect)
	{
		auto const& params = connect.parentsParams_;
		assert(params.size()>0);
		m_adapteeElementParam = params.at(0);
	}

	void transformUpdate(ElementParameter const& p)
	{
		m_figuer.setTransform(
				m_adapteeElementParam->getWorldTransform());

		adapterTransform(
				p.getAttribute().getValue(
					boost::type<element::parameter::angle>()),
				p.getAdapterParameter()->getTransformData()
				);
	}
};

inline void adapterTransform(float const angle, btTransform& result)
{
	result.setRotation(
			btQuaternion(btVector3(1.f, 0.f, 0.f), btRadians( angle )));
}

} // namespace behavior
} // namespace element_v2
} // namespace model
} // namespace akasha
#endif /* end of include guard */
