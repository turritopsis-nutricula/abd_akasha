/*
 * BasicElementFiguer.cpp
 *
 *  Created on: 2013/03/11
 *      Author: ely
 */


#include "BasicElementFiguer.hpp"
#include "../../../WispTasks.hpp"

#include "../../../RenderType.hpp"

#include "../../../../AkashaSystem/AkashaSystem.hpp"
#include <wisp_v3/irrExt.hpp>
#include <wisp_v3/wisp.hpp>

#include <irrlicht/ISceneManager.h>
#include <irrlicht/IAnimatedMeshSceneNode.h>
#include <irrlicht/IShadowVolumeSceneNode.h>

using namespace akasha;
using namespace akasha::model::element::figuer;


float degees(btScalar x)
{
	return x * (360.f / (2.f*3.14159));
}

class detail::FiguerImpl
{
	btTransform m_transform;
	wisp::ObjectID m_nodeID;
	irrExt::IrrExtention& m_client;
	boost::shared_ptr<NodeUpdateTask> m_task;

public:

	FiguerImpl(
			std::string const& meshName,
			irrExt::IrrExtention& c,
			boost::shared_ptr<NodeUpdateTask> const& t):
		m_client( c ), m_task(t)
	{

		//TODO : 仮実装
		m_nodeID = m_client.createObject(
				wisp::CreateFuncType( [meshName](
						boost::uuids::uuid id,
						irr::IrrlichtDevice * irr,
						wisp::IrrObjectStore & store )
					{
					wisp::WorkingDir workDir(irr);
					akasha::system::SetCurrentDirectory( irr );

					irr::scene::ISceneManager* mgr = irr->getSceneManager();
					irr::scene::IAnimatedMeshSceneNode* node =
					mgr->addAnimatedMeshSceneNode(
							mgr->getMesh( irr::io::path( ( std::string( "Resources/element/" ) + meshName ).c_str() ) )
							);

					if ( node )
					{
					store.store( id, static_cast<irr::scene::ISceneNode*>( node ), 0 );
					irr::video::SMaterial& m = node->getMaterial( 0 );
					node->setName(meshName.c_str());

					//マテリアル設定
					m.MaterialType = RenderType<test>::Type_;
					m.Lighting = true;


					m.SpecularColor = irr::video::SColor( 255, 255, 255, 255 );
					m.ColorMaterial = irr::video::ECM_NONE;
					m.Shininess = 100.f;


					irr::scene::IShadowVolumeSceneNode* s =
						node->addShadowVolumeSceneNode();
					s->setRotation(
							irr::core::vector3df( 0.f, 50.f, 0.f )
							);
					}
					} ) );

	}

	void setTransform( btTransform const& trans )
	{

		m_transform = trans;

		m_task->add(*m_nodeID, trans);


		// 		btTransform const& t = trans;
		// 		assert( m_nodeID );
		//
		// 		btVector3 const& vec = t.getOrigin();
		// 		btScalar x, y, z;
		// 		t.getBasis().getEulerYPR( y, x, z );
		//
		//
		// 		m_client.moveNode(
		// 				m_nodeID,
		// 				vec.getX(),
		// 				vec.getY(),
		// 				vec.getZ()
		// 				);
		// // 		m_client.rotateNode(
		// // 				m_nodeID,
		// // 				btDegrees( z ),
		// // 				btDegrees( x ),
		// // 				btDegrees( y )
		// // 				);
		// 		m_client.rotateNode(
		// 				m_nodeID,
		// 				degees( z ),
		// 				degees( x ),
		// 				degees( y )
		// 				);

	}
};




//コンストラクタ
BasicElementFiguer::BasicElementFiguer()
{

}

//デストラクタ
BasicElementFiguer::~BasicElementFiguer()
{

}

void BasicElementFiguer::create(
		irrExt::IrrExtention& c,
		boost::shared_ptr<NodeUpdateTask> const& t)
{
	create("chip.x", c, t);
}
void BasicElementFiguer::create(
		std::string const& meshName,
		irrExt::IrrExtention& c,
		boost::shared_ptr<NodeUpdateTask> const& t)
{
	impl_.reset( new detail::FiguerImpl( meshName, c, t ));

}

void BasicElementFiguer::setTransform( btTransform const& t )
{
	impl_->setTransform( t );
}
