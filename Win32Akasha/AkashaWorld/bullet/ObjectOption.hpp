#ifndef AKASHA_WORLD_BULLET_OBJECTOPTION_HPP_
#define AKASHA_WORLD_BULLET_OBJECTOPTION_HPP_

#include <LinearMath/btVector3.h>
namespace akasha
{
namespace bullet
{

struct ObjectOption
{
	btVector3 m_impluse;
	btVector3 m_torqueImpluse;

	void reset()
	{
		m_impluse.setZero();
		m_torqueImpluse.setZero();
	}

};

	template<typename BodyType>
ObjectOption& GetOption(BodyType const& b)
{
	return
		*(static_cast<ObjectOption*>(b.getUserPointer()));
}
}
} // namespace akasha

#endif /* end of include guard */
