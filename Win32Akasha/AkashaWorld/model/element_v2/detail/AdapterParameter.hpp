#ifndef AKASHA_MODEL_ADAPTER_PARAMETER_HPP_
#define AKASHA_MODEL_ADAPTER_PARAMETER_HPP_

#include <LinearMath/btTransform.h>
namespace akasha
{
namespace model
{
namespace element_v2
{
class AdapterParameter
{
	btTransform m_jointTransform;
public:
	btTransform& getTransformData()
	{
		return m_jointTransform;
	}
	void jointTransform(btTransform& t) const
	{
		t *= m_jointTransform;
	}
};
} // namespace element_v2
} // namespace model
} // namespace akasha
#endif /* end of include guard */
