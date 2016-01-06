/*
	Bullet Continuous Collision Detection and Physics Library
	Copyright (c) 2003-2009 Erwin Coumans  http://bulletphysics.com

	This software is provided 'as-is', without any express or implied warranty.
	In no event will the authors be held liable for any damages arising from the use of this software.
	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it freely,
	subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.

	Experimental Buoyancy fluid demo written by John McCutchan
	*/

#include "btHfFluidRigidCollisionConfiguration.h"
#include "algorithm/btFluidBodyCollisionAlgorithm.hpp"
// #include "btHfFluidRigidCollisionAlgorithm.h"
// #include "btHfFluidBuoyantShapeCollisionAlgorithm.h"
#include "LinearMath/btPoolAllocator.h"

// #include "btHfFluidBuoyantConcaveCollisionAlgorithm.h"
// #include "btHfFluidBuoyantConvexCollisionAlgorithmBase.hpp"

namespace
{
template<typename AlgorithmType>
	btCollisionAlgorithmCreateFunc*
	allocate_algorithm(void** mem_p)
{
	using allocType = typename AlgorithmType::CreateFunc;
	*mem_p = btAlignedAlloc(sizeof(allocType), 16);
	return new(*mem_p) allocType;
}

void release_algoritm(btCollisionAlgorithmCreateFunc* alog)
{
	alog->~btCollisionAlgorithmCreateFunc();
	btAlignedFree(alog);
}

}
btHfFluidRigidCollisionConfiguration::btHfFluidRigidCollisionConfiguration(
		const btDefaultCollisionConstructionInfo& constructionInfo) :
	btDefaultCollisionConfiguration(constructionInfo)
{
	void* mem;
	m_fluidCreateFunc =
		allocate_algorithm<btFluidBodyCollisionAlgorithm>(&mem);
	m_fluidCreateFunc->m_swapped = false;

	m_swappedFluidCreateFunc =
		allocate_algorithm< btFluidBodyCollisionAlgorithm>(&mem);
	m_swappedFluidCreateFunc->m_swapped = true;


	if (m_ownsCollisionAlgorithmPool && m_collisionAlgorithmPool)
	{
		int curElemSize = m_collisionAlgorithmPool->getElementSize();
		///calculate maximum element size, big enough to fit any collision algorithm in the memory pool

		int collisionAlgorithmMaxElementSize = sizeof(btFluidBodyCollisionAlgorithm);

		// 		int maxSize0 = sizeof(btHfFluidRigidCollisionAlgorithm);
		// 		int maxSize1 = sizeof(btHfFluidBuoyantConcaveCollisionAlgorithm);
		// 		int maxSize2 = sizeof(btHfFluidBuoyantConvexCollisionAlgorithmBase<>);
		//
		// 		int	collisionAlgorithmMaxElementSize = btMax(maxSize0,maxSize1);
		// 		collisionAlgorithmMaxElementSize = btMax(collisionAlgorithmMaxElementSize,maxSize2);

		if (collisionAlgorithmMaxElementSize > curElemSize)
		{
			m_collisionAlgorithmPool->~btPoolAllocator();
			btAlignedFree(m_collisionAlgorithmPool);
			void* mem = btAlignedAlloc(sizeof(btPoolAllocator),16);
			m_collisionAlgorithmPool =
				new(mem) btPoolAllocator(
						collisionAlgorithmMaxElementSize,
						constructionInfo.m_defaultMaxCollisionAlgorithmPoolSize);
		}
	}

	/* {{{*/
	// 	void* mem;
	//
	// 	mem = btAlignedAlloc(sizeof(btHfFluidRigidCollisionAlgorithm::CreateFunc),16);
	// 	m_hfFluidRigidConvexCreateFunc = new(mem) btHfFluidRigidCollisionAlgorithm::CreateFunc;
	//
	// 	mem = btAlignedAlloc(sizeof(btHfFluidRigidCollisionAlgorithm::CreateFunc),16);
	// 	m_swappedHfFluidRigidConvexCreateFunc = new(mem) btHfFluidRigidCollisionAlgorithm::CreateFunc;
	// 	m_swappedHfFluidRigidConvexCreateFunc->m_swapped = true;
	//
	// 	mem = btAlignedAlloc(sizeof(btHfFluidBuoyantShapeCollisionAlgorithm::CreateFunc),16);
	// 	m_hfFluidBuoyantShapeCollisionCreateFunc = new(mem) btHfFluidBuoyantShapeCollisionAlgorithm::CreateFunc(m_simplexSolver, m_pdSolver);
	//
	// #ifdef USE_BUOYANT_CONCAVE_ALGORITHM
	//
	// 	mem =  btAlignedAlloc(sizeof(btHfFluidBuoyantConcaveCollisionAlgorithm::CreateFunc),16);
	// 	m_hfFluidBuoyantConcaveCollisionCreateFunc = new(mem) btHfFluidBuoyantConcaveCollisionAlgorithm::CreateFunc;
	//
	// 	mem =  btAlignedAlloc(sizeof(btHfFluidBuoyantConcaveCollisionAlgorithm::SwappedCreateFunc),16);
	// 	m_swappedHfFluidBuoyantConcaveCollisionCreateFunc = new(mem) btHfFluidBuoyantConcaveCollisionAlgorithm::SwappedCreateFunc;
	//
	// 	mem = btAlignedAlloc(sizeof(btHfFluidBuoyantConvexCollisionAlgorithmBase<>::CreateFunc),16);
	// 	m_hfFluidBuoyantConvexCollisionCreateFunc = new(mem) btHfFluidBuoyantConvexCollisionAlgorithmBase<>::CreateFunc(m_simplexSolver, m_pdSolver, false);
	//
	// 	mem = btAlignedAlloc(sizeof(btHfFluidBuoyantConvexCollisionAlgorithmBase<>::CreateFunc),16);
	// 	m_swappedHfFluidBuoyantConvexCollisionCreateFunc = new(mem) btHfFluidBuoyantConvexCollisionAlgorithmBase<>::CreateFunc(m_simplexSolver, m_pdSolver, true);
	//
	// #endif //USE_BUOYANT_CONCAVE_ALGORITHM
	//
	// 	if (m_ownsCollisionAlgorithmPool && m_collisionAlgorithmPool)
	// 	{
	// 		int curElemSize = m_collisionAlgorithmPool->getElementSize();
	// 		///calculate maximum element size, big enough to fit any collision algorithm in the memory pool
	//
	// 		int maxSize0 = sizeof(btHfFluidRigidCollisionAlgorithm);
	// 		int maxSize1 = sizeof(btHfFluidBuoyantConcaveCollisionAlgorithm);
	// 		int maxSize2 = sizeof(btHfFluidBuoyantConvexCollisionAlgorithmBase<>);
	//
	// 		int	collisionAlgorithmMaxElementSize = btMax(maxSize0,maxSize1);
	// 		collisionAlgorithmMaxElementSize = btMax(collisionAlgorithmMaxElementSize,maxSize2);
	//
	// 		if (collisionAlgorithmMaxElementSize > curElemSize)
	// 		{
	// 			m_collisionAlgorithmPool->~btPoolAllocator();
	// 			btAlignedFree(m_collisionAlgorithmPool);
	// 			void* mem = btAlignedAlloc(sizeof(btPoolAllocator),16);
	// 			m_collisionAlgorithmPool = new(mem) btPoolAllocator(collisionAlgorithmMaxElementSize,constructionInfo.m_defaultMaxCollisionAlgorithmPoolSize);
	// 		}
	// 	}
	/* }}} */
}

btHfFluidRigidCollisionConfiguration::~btHfFluidRigidCollisionConfiguration()
{
	release_algoritm(m_fluidCreateFunc);
	release_algoritm(m_swappedFluidCreateFunc);

	/* {{{*/
	// 	void* mem;
	// 	m_hfFluidRigidConvexCreateFunc->~btCollisionAlgorithmCreateFunc();
	// 	m_swappedHfFluidRigidConvexCreateFunc->~btCollisionAlgorithmCreateFunc();
	// 	m_hfFluidBuoyantShapeCollisionCreateFunc->~btCollisionAlgorithmCreateFunc();
	// 	btAlignedFree(m_hfFluidRigidConvexCreateFunc);
	// 	btAlignedFree(m_swappedHfFluidRigidConvexCreateFunc);
	// 	btAlignedFree(m_hfFluidBuoyantShapeCollisionCreateFunc);
	//
	// #ifdef USE_BUOYANT_CONCAVE_ALGORITHM
	// 	m_hfFluidBuoyantConcaveCollisionCreateFunc->~btCollisionAlgorithmCreateFunc();
	// 	m_swappedHfFluidBuoyantConcaveCollisionCreateFunc->~btCollisionAlgorithmCreateFunc();
	// 	btAlignedFree(m_hfFluidBuoyantConcaveCollisionCreateFunc);
	// 	btAlignedFree(m_swappedHfFluidBuoyantConcaveCollisionCreateFunc);
	//
	// 	m_hfFluidBuoyantConvexCollisionCreateFunc->~btCollisionAlgorithmCreateFunc();
	// 	m_swappedHfFluidBuoyantConvexCollisionCreateFunc->~btCollisionAlgorithmCreateFunc();
	// 	btAlignedFree( m_hfFluidBuoyantConvexCollisionCreateFunc );
	// 	btAlignedFree( m_swappedHfFluidBuoyantConvexCollisionCreateFunc );
	// #endif //USE_BUOYANT_CONCAVE_ALGORITHM
	/* }}} */

}

btCollisionAlgorithmCreateFunc* btHfFluidRigidCollisionConfiguration::getCollisionAlgorithmCreateFunc(int proxyType0,int proxyType1)
{
	/* {{{*/
	// 	if ((proxyType0 == HFFLUID_SHAPE_PROXYTYPE) && (proxyType1 == HFFLUID_BUOYANT_CONVEX_SHAPE_PROXYTYPE))
	// 	{
	// 		return	m_hfFluidRigidConvexCreateFunc;
	// 	}
	//
	// 	if ((proxyType0 == HFFLUID_BUOYANT_CONVEX_SHAPE_PROXYTYPE) && (proxyType1 == HFFLUID_SHAPE_PROXYTYPE))
	// 	{
	// 		return	m_swappedHfFluidRigidConvexCreateFunc;
	// 	}
	//
	// 	if ((proxyType0 == HFFLUID_BUOYANT_CONVEX_SHAPE_PROXYTYPE) && (proxyType1 == HFFLUID_BUOYANT_CONVEX_SHAPE_PROXYTYPE))
	// 	{
	// 		return m_hfFluidBuoyantShapeCollisionCreateFunc;
	// 	}
	// #ifdef USE_BUOYANT_CONCAVE_ALGORITHM
	//
	// 	if ((proxyType0 == HFFLUID_BUOYANT_CONVEX_SHAPE_PROXYTYPE) && (proxyType1 == BOX_SHAPE_PROXYTYPE))
	// 	{
	// 		return m_hfFluidBuoyantConvexCollisionCreateFunc;
	// 	}
	// 	if ((proxyType0 == BOX_SHAPE_PROXYTYPE) && (proxyType1 == HFFLUID_BUOYANT_CONVEX_SHAPE_PROXYTYPE))
	// 	{
	// 		return m_swappedHfFluidBuoyantConvexCollisionCreateFunc;
	// 	}
	//
	// 	if ((proxyType0 == HFFLUID_BUOYANT_CONVEX_SHAPE_PROXYTYPE) && btBroadphaseProxy::isConcave(proxyType1))
	// 	{
	// 		return m_hfFluidBuoyantConcaveCollisionCreateFunc;
	// 	}
	//
	// 	if (btBroadphaseProxy::isConcave(proxyType0) && (proxyType1 == HFFLUID_BUOYANT_CONVEX_SHAPE_PROXYTYPE))
	// 	{
	// 		return m_swappedHfFluidBuoyantConcaveCollisionCreateFunc;
	// 	}
	// #endif //USE_BUOYANT_CONCAVE_ALGORITHM
	// 	///fallback to the regular rigid collision shape
	/* }}} */

	if (proxyType0 == HFFLUID_SHAPE_PROXYTYPE)
		return m_fluidCreateFunc;

	if (proxyType1 == HFFLUID_SHAPE_PROXYTYPE)
		return m_swappedFluidCreateFunc;


	return btDefaultCollisionConfiguration::getCollisionAlgorithmCreateFunc(proxyType0,proxyType1);
}

