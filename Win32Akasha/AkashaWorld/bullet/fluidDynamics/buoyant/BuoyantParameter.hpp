#ifndef AKASHA_WORLD_BULLET_FLUIDDYNAMICS_BUYANT_BUOYANTPARAMETER_HPP_
#define AKASHA_WORLD_BULLET_FLUIDDYNAMICS_BUYANT_BUOYANTPARAMETER_HPP_

#include <LinearMath/btVector3.h>
namespace akasha
{
namespace bullet
{
namespace buoyant
{
struct BuoyantParameter
{
	btScalar floatyness_;
	btScalar radius_;
	btScalar totalVolume_;
	btScalar volumePerVoxel_;
	int numVoxels_;
	btVector3* voxelPositions_;
	// 				btConvexShape* convexShape_;
};
} // namespace buoyant
} // namespace bullet
} // namespace akasha
#endif /* end of include guard */
