#ifndef LANDFORM__HPP
#define LANDFORM__HPP


#include "../akashaWorldCommon.h"

#include "../contactEvent/EventGroup.hpp"

// #include "../Receptor/LandReceptor.hpp"
#include "../convert/ConvertMeshBuffer.hpp"

#include "../BulletWorldManager.h"
#include "../../AkashaApplication/SoundManager.hpp"
#include "../../AkashaSystem/AkashaSystem.hpp"

//wisp
#include <wisp_v3/wisp.hpp>
#include <wisp_v3/irrExt.hpp>
//irr
#include <ISceneManager.h>
#include <IAnimatedMeshSceneNode.h>
#include <IMeshCache.h>
//bullet
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionDispatch/btInternalEdgeUtility.h>
//boost
#include <boost/filesystem/path.hpp>
#include <boost/make_shared.hpp>
#include <boost/fusion/container/generation/make_vector.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/array.hpp>

#include <algorithm>

namespace akasha
{
namespace effect
{

}
namespace land
{
namespace helper
{

struct MeshAccess
{
	typedef float vertexType;
	btTriangleIndexVertexArray* meshArray_;


	btVector3
		getTriangleNormal( const int partID, const int index ) const
		{
			const auto a = getTriangle( partID, index );
			return ( btVector3( a[3 + 0], a[3 + 1], a[3 + 2] ) - btVector3( a[0 + 0], a[0 + 1], a[0 + 2] ) ).cross(
					btVector3( a[6 + 0], a[6 + 1], a[6 + 2] ) - btVector3( a[0 + 0], a[0 + 1], a[0 + 2] )
					);
		}
	btVector3
		getTriangleNormalReverse( const int partID, const int index ) const
		{
			const auto a = getTriangle( partID, index );
			return
				( btVector3( a[6 + 0], a[6 + 1], a[6 + 2] ) - btVector3( a[0 + 0], a[0 + 1], a[0 + 2] ) ).cross(
						btVector3( a[3 + 0], a[3 + 1], a[3 + 2] ) - btVector3( a[0 + 0], a[0 + 1], a[0 + 2] )
						);

		}



	boost::array<float, 9>
		getTriangle( const int partID, const int index ) const
		{
			boost::array<float , 9> res;

			//btVector3 const& meshScaling = btVector3(1.0,1.f,1.f);
			auto const& m = meshArray_->getIndexedMeshArray().at( partID );

			unsigned int* gfxbase = ( unsigned int* )( m.m_triangleIndexBase +  index * m.m_triangleIndexStride );
			for ( int i = 2; i >= 0; i-- )
			{
				int graphicsindex = m.m_indexType == PHY_SHORT ? ( ( unsigned short* )gfxbase )[i] : m.m_indexType == PHY_INTEGER ? gfxbase[i] : ( ( unsigned char* )gfxbase )[i];
				float* graphicsbase = ( float* )( m.m_vertexBase + graphicsindex * m.m_vertexStride );

				res[i * 3] = graphicsbase[0]; //*meshScaling.getX();
				res[i * 3 + 1] = graphicsbase[1]; //*meshScaling.getY();
				res[i * 3 + 2] = graphicsbase[2]; //*meshScaling.getZ();
			}

			return res;
		}

};
}

struct TouchSound : boost::noncopyable
{
	//     boost::shared_ptr<cricket::AudioVoice> m_knock0; // wood knock

	TouchSound()
	{
		//         namespace fs = boost::filesystem;
		//         cricket::CricketManagerBase& cricketMgr = cricket::CricketManager::Instance();
		//         fs::path dir =
		//             system::AkashaSystem::Instance().getConstant().getValue<system::ct::CurrentDirectoryPath>() / "resources/sound";
		//
		//         m_knock0 = cricketMgr.createVoice( cricketMgr.createWave( dir / "wood_knock.wav" ) );

	}
	// 			void
	// 				touch( const btRigidBody* o, entity::EntityReceptor& e, btManifoldPoint const& p, btVector3 const& relativeVelocity, btVector3 const& landNormal )
	// 				{
	//assert(p.getDistance()<= 1.0e-3);

	//std::cout <<  "call: " << value << std::endl;

	//         if ( true )
	//             {
	//                 std::cout <<  boost::format( "impulse:%1% time:%2%" )  % p.getAppliedImpulse() % p.m_lifeTime << std::endl;
	//                 /*std::cout <<  boost::format("angler:%1%, local:%2% localInW:%3% angVel:%4% angVel:%5%") %
	//                 	 o->getAngularVelocity() %
	//                 	 p.m_localPointA %
	//                 	 (o->getWorldTransform().getBasis() * p.m_localPointA) %
	//                 	 o->getAngularVelocity().cross(o->getWorldTransform().getBasis() * p.m_localPointA) %
	//                 	(p.m_normalWorldOnB * ((o->getWorldTransform().getBasis() * p.m_localPointA).dot(p.m_normalWorldOnB)))
	//                 	 //o->getVelocityInLocalPoint(o->getWorldTransform().getBasis() * p.m_localPointA)
	//                 	 << std::endl;*/
	//                 cricket::CricketManager::Instance().calculate3DAudio( m_knock0,
	//                         btTransform( btQuaternion::getIdentity(), p.getPositionWorldOnA() ), btVector3( btScalar( 0 ), btScalar( 0 ), btScalar( 0 ) ) );
	//
	//                 m_knock0->setVolume( btClamped( m_knock0->getVolume() * relativeVelocity.length2() * 0.05f, 0.f, 2.f ) );
	//                 m_knock0->play();
	//
	//             }

	// 				}
};

class Landform : boost::noncopyable
{
public:
	using contactGroupMap =
		std::vector<
		std::pair<contactEvent::EventGroup, boost::shared_ptr<btRigidBody>>>;

private:

	struct BodyDeleter
	{
		typename BulletManager::hold_type& w_;
		BodyDeleter(typename BulletManager::hold_type& w): w_(w){ }

		void operator()(btRigidBody* b) const
		{
			w_.removeRigidBody( b );
			delete b;
		}
	};
	/*
		typedef UnitArray<
		audio::SimpleSpeakerUnit< audio::Speaker >, 10> SoundEffect;*/
	//typedef boost::shared_ptr<SoundEffect> SE_ptr;
	typedef convert::MeshBuffer<> MeshBuffer_t;




	wisp::ObjectID m_nodeID;
	//btTriangleMesh m_mesh;
	btTriangleIndexVertexArray m_mesh;
	boost::shared_ptr<btBvhTriangleMeshShape> m_landShape;
	boost::shared_ptr<btRigidBody> m_body;
	btTriangleInfoMap m_meshInfo;
	// 				effect::SmorkEffectManager m_smork;
	TouchSound m_touchSound;


	// 				LandformReceptor<effect::SmorkEffectManager, helper::MeshAccess, TouchSound> m_receptor;
	// 				entity::EntityReceptor m_receptorWrapper;

	std::vector<MeshBuffer_t> m_meshBuffer;
	//SE_ptr m_impactSound; //着弾音
	//SE_ptr m_contactSound; //接地音

public:
	Landform()
	{

		//着弾音セットアップ
		/*
			m_impactSound = boost::make_shared<SoundEffect>(
		//audio::BufferInfo("Resources/impact.wav",audio::BufferSetting<>())
		audio::BufferInfo("Resources/knock.wav",audio::BufferSetting<>())
		);
		if (m_impactSound->valied())
		{
		m_impactSound->allApply(
		boost::fusion::make_map<audio::tag::volume>(2.0f));
		m_receptor.impactSound_ = m_impactSound;
		}

		//接地音セットアップ
		m_contactSound = boost::make_shared<SoundEffect>(
		audio::BufferInfo("Resources/knock.wav",audio::BufferSetting<>())
		);
		if (m_impactSound->valied())
		{
		m_receptor.contactSound_ = m_contactSound;
		}*/
	}

	bool loadFromFile(
			const irr::io::path& meshPath,
			irrExt::IrrExtention& irrEx,
			wisp::v3::WispHandle& client
			)
	{
		irr::scene::IMesh* mesh = nullptr;
		auto meshLoad = [meshPath, &mesh, this]( irr::IrrlichtDevice * irr )
		{
			wisp::WorkingDir workDir(irr);
			using boost::fusion::at_c;

			typedef boost::fusion::vector<irr::video::E_TEXTURE_CREATION_FLAG, bool> mapType;
			boost::array< mapType, 2> buf;
			buf[0] = {irr::video::E_TEXTURE_CREATION_FLAG::ETCF_CREATE_MIP_MAPS, false};
			buf[1] = {irr::video::E_TEXTURE_CREATION_FLAG::ETCF_NO_ALPHA_CHANNEL, false};
			BOOST_FOREACH( auto & b, buf )
			{
				at_c<1>( b ) = irr->getVideoDriver()->getTextureCreationFlag( at_c<0>( b ) );
			}

			irr->getVideoDriver()->setTextureCreationFlag(
					irr::video::E_TEXTURE_CREATION_FLAG::ETCF_CREATE_MIP_MAPS, false );
			//irr->getVideoDriver()->setTextureCreationFlag(irr::video::E_TEXTURE_CREATION_FLAG::ETCF_NO_ALPHA_CHANNEL, false);

			auto* meshCache = irr->getSceneManager()->getMeshCache();
			if (meshCache->isMeshLoaded(meshPath))
			{
				// 							std::cout << " reload mesh." << meshPath.c_str();
				meshCache->removeMesh( meshCache->getMeshByName( meshPath ));
			}
			mesh = irr->getSceneManager()->getMesh( meshPath )->getMesh( 0 );

			for( auto const& b : buf)
				irr->getVideoDriver()->setTextureCreationFlag( at_c<0>( b ), at_c<1>( b ) );

		};


		client.synchronaizedOperate(
				wisp::SynchronizeFuncType( meshLoad) );

		assert( mesh );


		// JointTransform Matrix
		boost::unordered_multimap<
			unsigned int, irr::core::matrix4> transformMatrixes;
		{
			irr::scene::ISkinnedMesh* skinMesh =
				static_cast<irr::scene::ISkinnedMesh*>(mesh);
			assert(skinMesh);

			auto const& jointList = skinMesh->getAllJoints();
			for (unsigned int i=0; i<jointList.size(); ++i)
			{
				auto const* joint = jointList[i];

				auto const& attach = joint->AttachedMeshes;
				for (unsigned int a=0; a < attach.size(); ++a)
				{
					transformMatrixes.insert(
							std::make_pair(attach[a], joint->GlobalMatrix ));
				}

			}
		}
		{
			//Transfomr,IrrMeshをコンバート
			m_meshBuffer.reserve(mesh->getMeshBufferCount());
			for ( unsigned int i = 0; i < mesh->getMeshBufferCount(); ++i )
			{
				MeshBuffer_t m( *(mesh->getMeshBuffer(i)));
				auto range = transformMatrixes.equal_range(i);
				assert(std::distance(range.first,range.second) <= 1);
				boost::for_each(range, [&m ](
							std::pair<unsigned int,irr::core::matrix4> const& mat){
						// 									std::cout << i << " : trans\n";
						m.transform(
								(mat.second  ).pointer());
						});
				m_meshBuffer.push_back( std::move(m));
			}


			for (auto const& meshBuf : m_meshBuffer)
				meshBuf.createBtMesh( m_mesh );

			m_landShape = boost::make_shared<btBvhTriangleMeshShape>(
					static_cast<btStridingMeshInterface*>( &m_mesh ), true );

			//Generate MeshInfo
			btGenerateInternalEdgeInfo(m_landShape.get(), &m_meshInfo);

			//Body setting
			auto tree =  akasha::system::GetSettingTree("MIZUNUKI.Physics.Land");
			assert(tree);
			m_landShape->setMargin(tree->get<btScalar>("margin", 0.04));

			m_body.reset(new btRigidBody(
						0.f,
						static_cast<btMotionState*>( 0 ),
						static_cast<btCollisionShape*>( m_landShape.get() )
						), BodyDeleter(BulletManager::Instance()));


			m_body->setFriction( 0.9f );
			BulletManager::Instance().addRigidBody(
					m_body.get(),
					COL_TYPE_LAND,
					COL_TYPE_MODEL |
					COL_TYPE_SUB_MODEL_0 |
					COL_TYPE_TOY |
					COL_TYPE_BOM |
					COL_TYPE_AVATAR
					);
			bullet::AddCustomMaterialCallback(m_body.get());
		}

		{
			//irrNode生成
			m_nodeID = irrEx.createObject(
					wisp::CreateFuncType( [meshPath](
							boost::uuids::uuid id,
							irr::IrrlichtDevice* irr,
							wisp::IrrObjectStore & store
							)
						{
						wisp::WorkingDir working(irr);

						auto* mgr = irr->getSceneManager();
						auto* node = mgr->addAnimatedMeshSceneNode( mgr->getMesh( meshPath ) );
						store.store( id, node, 0 );

						node->setName("LandForm");

						for( unsigned int m = 0; m < node->getMaterialCount(); m++ )
						{
						//TODO : 場合によってはEMT_SOLIDを指定したい
						irr::video::SMaterial& mat = node->getMaterial( m );
						mat.MaterialType = irr::video::E_MATERIAL_TYPE::EMT_TRANSPARENT_ALPHA_CHANNEL_REF;
						for ( unsigned int i = 0; i < 4; i++ )
						mat.TextureLayer[i].TrilinearFilter = true;
						}

						} ) );



		}
		return true;
	}

	void update( const btScalar step )
	{
		// 					m_smork.update( step );
	}

	contactGroupMap
		getContactGroupMap() const
		{
			contactGroupMap buf;
			buf.push_back(
					std::make_pair(contactEvent::EventGroup::BasicLand, m_body)
					);
			return std::move(buf);
		}

};
}
}
#endif //LANDFORM__HPP
