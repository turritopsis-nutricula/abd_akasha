#ifndef LAND_WATER__HPP
#define LAND_WATER__HPP

#include "../../AkashaSystem/AkashaSystem.hpp"
#include "../bullet/fluidDynamics/fluid/btHfFluid.h"
// #include "../BulletHfFluid/btHfFluid.h"

#include "../BulletWorldManager.h"

#include <wisp_v3/wisp.hpp>
#include <wisp_v3/irrExt.hpp>
#include "../entity/EntityReceptor.hpp"
//irr
#include "CSceneManager.h"
#include "CMeshBuffer.h"
#include "SMesh.h"
#include "CMeshSceneNode.h"

//boost
//#include <boost/interprocess/smart_ptr/unique_ptr.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/optional.hpp>
#include <boost/utility/in_place_factory.hpp>

//akasha
//#include "../entity/EntityService.hpp"

namespace akasha
{
namespace land
{
class LandWater : boost::noncopyable
	{

		struct btStaticPlaneShapeArg
		{
			btStaticPlaneShapeArg(
					const btVector3& normal,
					const btScalar constant ) :
				planeNormal_( normal ),
				planeConstant_( constant ) { }

			btVector3 planeNormal_;
			btScalar planeConstant_;
		};

		struct FluidDeleter
		{
			typename BulletManager::hold_type& w_;
			FluidDeleter(typename BulletManager::hold_type& w) : w_(w){ }

			void operator()( btHfFluid* f) const
			{
				w_.removeHfFluid( f );
				delete f;
			}
		};

		btScalar m_alt;       //! 水面高度
		wisp::ObjectID m_nodeID;
		boost::shared_ptr< btHfFluid > m_fluid; //TODO : 削除子用意していないためリークする!!

		entity::EntityReceptor m_receptor;

	public:
		LandWater() :
			m_alt( btScalar( -0.45 ) ),
			//m_alt(btScalar(20.0)),
			m_receptor( entity::gDefaultReceptor )
		{ }

		~LandWater()
		{
			//TODO: ちゃんと削除子を用意したい
		}

		void setup( irrExt::IrrExtention& irrEx, wisp::v3::WispHandle& client )
		{
			m_fluid.reset();




			btVector3 min, max;
			BulletManager::Instance().getBroadphase()->getBroadphaseAabb(
					min, max
					);
			//min = btVector3(-20.f, -20.f, -20.f);
			//max = btVector3(20.f, 20.f, 20.f);

			const btScalar surfaceHeight = m_alt;
			const btScalar bottom = min.getY();
			const btScalar unitSize = ( max.getX() - min.getX() );

			btHfFluid* fluid = NULL;

			fluid = new btHfFluid ( unitSize , 4, 4 );
			btTransform xform;
			xform.setIdentity ();
			xform.getOrigin() = btVector3(
					-unitSize * 1.5f ,
					bottom,
					-unitSize * 1.5f
					);
			fluid->setWorldTransform ( xform );

			fluid->setHorizontalVelocityScale ( btScalar( 0.2f ) );
			fluid->setVolumeDisplacementScale ( btScalar( 0.2f ) );
			if (auto m = akasha::system::GetSetting<btScalar>("MIZUNUKI.Physics.Water.mu"))
				fluid->setMu(*m);
			if (auto d = akasha::system::GetSetting<btScalar>("MIZUNUKI.Physics.Water.density"))
				fluid->setDensity( *d );

			BulletManager::Instance().addHfFluid( fluid );

			for ( int i = 0; i < fluid->getNumNodesLength()*fluid->getNumNodesWidth(); i++ )
			{
				fluid->setFluidHeight( i, surfaceHeight - bottom );
			}

			fluid->setGravity( btScalar( -9.81 ) );
			//auto* resp = new entity::EntityReceptor(entity::gDefaultReceptor);
			//         fluid->setUserPointer( &( m_receptor ) );
			fluid->prep();
			m_fluid.reset( fluid, FluidDeleter( BulletManager::Instance() ) );

			//irr


			irr::io::path currentDirPath = akasha::system::AkashaSystem::Instance().getConstant().getCurrentDirectoryPath().c_str();
			m_nodeID = irrEx.createObject(
					wisp::CreateFuncType(
						[currentDirPath](
							boost::uuids::uuid id,
							irr::IrrlichtDevice * irr,
							wisp::IrrObjectStore & store
							)
						{


						typedef irr::video::S3DVertex VertexType;
						typedef irr::scene::CMeshBuffer<VertexType> MeshBufferType;

						float size = 30000.f;
						float alt = 0.f;
						float tCoord = size * 0.1f;
						MeshBufferType* meshBuf = new MeshBufferType();
						meshBuf->Vertices.push_back( VertexType( size, alt, size , 0.f, 1.f, 0.f,  0xccFFFFFF, tCoord, tCoord ) );
						meshBuf->Vertices.push_back( VertexType( -size, alt, size, 0.f, 1.f, 0.f,  0xccFFFFFF, 0, tCoord ) );
						meshBuf->Vertices.push_back( VertexType( -size, alt, -size, 0.f, 1.f, 0.f,  0xccFFFFFF, 0, 0 ) );
						meshBuf->Vertices.push_back( VertexType( size, alt, -size, 0.f, 1.f, 0.f,  0xccFFFFFF, tCoord, 0 ) );

						meshBuf->Indices.push_back( 0 );
						meshBuf->Indices.push_back( 3 );
						meshBuf->Indices.push_back( 1 );

						meshBuf->Indices.push_back( 1 );
						meshBuf->Indices.push_back( 3 );
						meshBuf->Indices.push_back( 2 );

						//meshBuf->getMaterial().ZBuffer = false;
						meshBuf->getMaterial().MaterialType = irr::video::EMT_TRANSPARENT_VERTEX_ALPHA;

						meshBuf->recalculateBoundingBox();

						irr::scene::SMesh* mesh = new irr::scene::SMesh( );
						mesh->addMeshBuffer( meshBuf );
						mesh->recalculateBoundingBox();


						irr::scene::ISceneManager* mgr = irr->getSceneManager();

						auto* n = mgr->addMeshSceneNode( mesh );
						store.store( id, n, 0 );
						n->setName("LandWaterPlane");

						auto* t = mgr->getVideoDriver()->getTexture( currentDirPath + "./Resources/water.jpg" );

						irr::video::SMaterial& m = n->getMaterial( 0 );
						m.setTexture( 0, t );

						auto& l = m.TextureLayer[0];
						l.TextureWrapU =  irr::video::E_TEXTURE_CLAMP::ETC_REPEAT;
						l.TextureWrapV =  irr::video::E_TEXTURE_CLAMP::ETC_REPEAT;

						} ) );


		}

		void setWaterSerfaceAlt( btScalar a )
		{
			m_alt = a;
			btVector3 min, max;
			BulletManager::Instance().getBroadphase()->getBroadphaseAabb(
					min, max
					);
			for ( int i = 0; i < m_fluid->getNumNodesLength()*m_fluid->getNumNodesWidth(); i++ )
			{
				m_fluid->setFluidHeight( i, m_alt - min.getY() );
			}
			m_fluid->adjustAabb();

			//TODO : メッシュ側の操作
		}

	};
}
}


BOOST_FUSION_ADAPT_STRUCT(
		akasha::land::LandWater::btStaticPlaneShapeArg,
		( btVector3, planeNormal_ )
		( btScalar, planeConstant_ )
		)


#endif //LAND_WATER__HPP
