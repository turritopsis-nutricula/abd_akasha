#include "BuoyantAuthoizer.hpp"

#include <BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.h>
#include <BulletCollision/CollisionShapes/btConvexShape.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>


using akasha::bullet::buoyant::BuoyantAuhoizer;
using akasha::bullet::buoyant::BuoyantParameter;

#define MAX_VOXEL_DIMENSION 32

//fwd
bool intersect(
		btVoronoiSimplexSolver* simplexSolver,
		btTransform const& transfromA,
		btTransform const& transformB,
		btConvexShape const* a,
		btConvexShape* b
		);

void BuoyantAuhoizer::calcParameter(
		BuoyantParameter& result,
		btConvexShape const* shape, btScalar const radius, btScalar const gap)
{

	btTransform T;
	T.setIdentity ();
	btVector3 aabbMin, aabbMax;
	shape->getAabb(T,aabbMin, aabbMax);

	result.radius_ = radius;
	result.numVoxels_ = 0;

	btVoronoiSimplexSolver simplexSolver;
	btSphereShape sphereShape(radius);
	btVector3* voxelPositions =
		(btVector3*)btAlignedAlloc (
				sizeof(btVector3)*MAX_VOXEL_DIMENSION*MAX_VOXEL_DIMENSION*MAX_VOXEL_DIMENSION,16);

	for (int i = 0; i < MAX_VOXEL_DIMENSION; i++)
	{
		for (int j = 0; j < MAX_VOXEL_DIMENSION; j++)
		{
			for (int k = 0; k < MAX_VOXEL_DIMENSION; k++)
			{
				btVector3 point;
				btTransform sT;
				sT.setIdentity ();

				point.setX(aabbMin.getX() + (i * btScalar(2.0f) * radius) + (i * gap));
				point.setY(aabbMin.getY() + (j * btScalar(2.0f) * radius) + (j * gap));
				point.setZ(aabbMin.getZ() + (k * btScalar(2.0f) * radius) + (k * gap));

				if (TestPointAgainstAabb2(aabbMin, aabbMax, point))
				{
					btTransform sT;
					sT.setIdentity ();
					sT.setOrigin (point);

					if (intersect (&simplexSolver, T, sT, shape, &sphereShape))
					{
						voxelPositions[result.numVoxels_] = point;
						result.numVoxels_++;
					}
				}
			}
		}
	}
	result.voxelPositions_ =
		(btVector3*)btAlignedAlloc (sizeof(btVector3)*result.numVoxels_, 16);
	for (int i = 0; i < result.numVoxels_;i++)
	{
		result.voxelPositions_[i] = voxelPositions[i];
	}
	btAlignedFree (voxelPositions);
	result.volumePerVoxel_ = btScalar(4.0f)/btScalar(3.0f)*SIMD_PI*radius*radius*radius;
	result.totalVolume_ = result.numVoxels_ * result.volumePerVoxel_;
	result.radius_ = radius;

	result.floatyness_ = btScalar(0.00);


}

#define REL_ERROR2 btScalar(1.0e-6)

bool intersect(
		btVoronoiSimplexSolver* simplexSolver,
		btTransform const& transformA,
		btTransform const& transformB,
		btConvexShape const* a,
		btConvexShape* b
		)
{

	btScalar squaredDistance = SIMD_INFINITY;
	btTransform localTransA = transformA;
	btTransform localTransB = transformB;
	btVector3 positionOffset = (localTransA.getOrigin() + localTransB.getOrigin()) * btScalar(0.5);
	localTransA.getOrigin() -= positionOffset;
	localTransB.getOrigin() -= positionOffset;
	btScalar delta = btScalar(0.);
	btVector3 v = btVector3(1.0f, 0.0f, 0.0f);
	simplexSolver->reset ();
	do
	{
		btVector3 seperatingAxisInA = (-v)* transformA.getBasis();
		btVector3 seperatingAxisInB = v* transformB.getBasis();

		btVector3 pInA = a->localGetSupportVertexNonVirtual(seperatingAxisInA);
		btVector3 qInB = b->localGetSupportVertexNonVirtual(seperatingAxisInB);

		btVector3  pWorld = localTransA(pInA);
		btVector3  qWorld = localTransB(qInB);

		btVector3 w	= pWorld - qWorld;
		delta = v.dot(w);

		// potential exit, they don't overlap
		if ((delta > btScalar(0.0)))
		{
			return false;
		}

		if (simplexSolver->inSimplex (w))
		{
			return false;
		}

		simplexSolver->addVertex (w, pWorld, qWorld);

		if (!simplexSolver->closest(v))
		{
			return false;
		}

		btScalar previousSquaredDistance = squaredDistance;
		squaredDistance = v.length2();

		if (previousSquaredDistance - squaredDistance <= SIMD_EPSILON * previousSquaredDistance)
		{
			return false;
		}
	} while (!simplexSolver->fullSimplex() && squaredDistance > REL_ERROR2 * simplexSolver->maxVertex());

	return true;
}
