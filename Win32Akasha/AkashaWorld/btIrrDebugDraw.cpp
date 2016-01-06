


#include "btIrrDebugDraw.h"


#include <irrlicht.h>

#include <stdio.h>
#include <tchar.h>
#include <iostream>

//#include "irrlicht/IrrExtention.h"

using namespace irr;
using namespace akasha;

btIrrDebugDraw::btIrrDebugDraw( irrExt::IrrExtention& c) :
	m_client( c )
{ }

void btIrrDebugDraw::setDebugMode( int debugMode )
{
	m_debugMode = static_cast<btIDebugDraw::DebugDrawModes>( debugMode );
}

void btIrrDebugDraw::drawLine( const btVector3& from, const btVector3& to, const btVector3& )
{



	//     m_client.setLineColor( static_cast<u32>( color.getX() ),
	//                            static_cast<u32>( color.getY() ),
	//                            static_cast<u32>( color.getZ() ) );

	m_client.move3DLine (
			static_cast<float>( from.getX() ),
			static_cast<float>( from.getY() ),
			static_cast<float>( from.getZ() )
			);
	m_client.draw3DLine(
			static_cast<float>( to.getX() ),
			static_cast<float>( to.getY() ),
			static_cast<float>( to.getZ() )
			);



}

void btIrrDebugDraw::drawContactPoint(
		const btVector3& PointOnB,
		const btVector3& normalOnB,
		btScalar distance,
		int lifeTime,
		const btVector3& color )
{


	// TODO :çƒé¿ëï


}

int btIrrDebugDraw::getDebugMode() const
{
	return static_cast<int>( m_debugMode );
}
