

#include "AkashaWorld.h"
#include "../AkashaSystem/AkashaSystem.hpp"

#include "environment/EnvironmentLand.hpp" // 環境

#include "model/loader/RCFileLoader.hpp" // Rcdファイルローダー
#include "model/ElementModel.hpp" // モデル

#include "PlayerView.hpp"       // 視点
#include "Toy/Toy.hpp"

#include "detection/ObjectCapture.hpp" //オブジェクトキャプチャー

//#include "WorldCommand.hpp"     //ワールドコマンド

#include "IrrExtention.h"
#include "RenderType.hpp"

#include "bullet/btFractureBody.hpp"
#include "bullet/btProfileDump.hpp" //ダンプキャプチャ


#include "effect/SmorkEffect.hpp" //TODO

#include "WispTasks.hpp"
//wisp
#include <wisp_v3/wisp.hpp>

//irr
#include "COpenGLMaterialRenderer.h"
#include "COpenGLSLMaterialRenderer.h"

//bullet
#include "btIrrDebugDraw.h"
#include <BulletCollision/CollisionDispatch/btInternalEdgeUtility.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>


//boost
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/make_shared.hpp>
#include <boost/format.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

#include "convert/ConvertFunction.hpp" // データ形式コンバート

#include "bullet/btio.hpp"
//#include "avatar/AvatarBase.hpp" // プレイヤーアバター

//Debug
std::string VectorDebug(btVector3 const& v)
{
	std::string s(":");
	return std::to_string((float)(v.getX())) + s + std::to_string((float)(v.getY())) + s +  std::to_string((float)(v.getZ()));
}


using namespace akasha;
using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

#include <boost/unordered_map.hpp>
#include <boost/range/algorithm/sort.hpp>
#include <boost/range/algorithm/unique.hpp>
#include <boost/range/algorithm_ext/erase.hpp>
#include <algorithm>


//////////////////////////////////////////////////////////////////
// EntityReceptor //TODO: Deproy
template<typename WorldType>
struct doSubStepReceptor
{
	boost::signals2::connection connection_;
	void
		operator()( WorldType const&, const btScalar tickStep )
		{
			// 			auto& dispatcher = *(w.dispatcher_);
			// 			int numManifolds = dispatcher.getNumManifolds();
			// 			for ( int i = 0; i < numManifolds; i++ )
			// 			{
			// 				btPersistentManifold* contactManifold =
			// 					dispatcher.getManifoldByIndexInternal( i );
			// 				btCollisionObject const* obA =
			// 					static_cast<btCollisionObject const*>( contactManifold->getBody0() );
			// 				btCollisionObject const* obB =
			// 					static_cast<btCollisionObject const*>( contactManifold->getBody1() );
			//
			//
			//
			// 			}
		}

};

template<typename BufferType>
struct doStepReceptor
{
	BufferType& buffer_;
	doStepReceptor() = delete;
	doStepReceptor(BufferType& b) : buffer_(b){}
	void
		operator()( btDispatcher* dispater, const btScalar tickStep )
		{
			int const numManifolds = dispater->getNumManifolds();
			for ( int i = 0; i < numManifolds; ++i )
			{
				btPersistentManifold* contactManifold =
					dispater->getManifoldByIndexInternal( i );
				btCollisionObject const* obA =
					static_cast<btCollisionObject const*>( contactManifold->getBody0() );
				btCollisionObject const* obB =
					static_cast<btCollisionObject const*>( contactManifold->getBody1() );

				contactEvent::BodyPair p;
				p.manifold_ = contactManifold;
				p.set(obA, obB);
				buffer_.push_back( std::move(p));
			}
		}
};

// ObjectOptionRoutine //////////////////////////////////////////
void ObjectOptionRoutine(btCollisionObject* obj)
{
	if (obj->getUserPointer())
	{
		auto* oo = static_cast<
			bullet::ObjectOption*>(obj->getUserPointer());
		btRigidBody* body = btRigidBody::upcast(obj);
		if (oo && body)
		{
			body->applyCentralImpulse(oo->m_impluse * 60.f);
			body->applyTorqueImpulse(oo->m_torqueImpluse * 60.f);
			oo->reset();
			return;
		}
		btFractureBody* fBody = btFractureBody::upcast(obj);
		if (oo && fBody)
		{
			fBody->applyCentralImpulse(oo->m_impluse*60.f);
			fBody->applyTorqueImpulse(oo->m_torqueImpluse*60.f);
			oo->reset();
			return;
		}

	}
}
// Utility Functions ##############################################################
void InitModelPosition(
		model::ElementModel& model,
		land::EnvironmentLand const& land,
		btVector3 const& sweepFrom)
{

	auto const& aabb = model.getObserver()->getAabb(0);
	auto const aabbCenter( (aabb.second+aabb.first)*btScalar(0.5));
	auto const aabbHalf(
			((aabb.second-aabb.first)*btScalar(0.5)).absolute() );
	btBoxShape shape( aabbHalf );
	btVector3 const sweep_from( sweepFrom + btVector3(0.f, aabbHalf.getY(), 0.f));
	btVector3 const sweep_to( sweepFrom.getX(), -3000.f, sweepFrom.getZ() );

	std::cout << aabbHalf << "\n" << aabbCenter << "\n";

	btCollisionWorld::ClosestConvexResultCallback
		callback(sweep_from, sweep_to);

	callback.m_collisionFilterGroup = COL_TYPE_MODEL;
	callback.m_collisionFilterMask = COL_TYPE_LAND;

	BulletManager::Instance().convexSweepTest(
			&shape,
			btTransform( btQuaternion::getIdentity(), sweep_from),
			btTransform( btQuaternion::getIdentity(), sweep_to ),
			callback);

	btVector3 contact_pos(
			(sweep_to - sweep_from)*callback.m_closestHitFraction +
			sweep_from);

	if (auto t = akasha::system::GetSetting<float>(
				"MIZUNUKI.InitModelPosition"))
			contact_pos.setY( *t + contact_pos.getY());


	model.modelWrap(
			btTransform(
				btQuaternion( btVector3( 0. , 1. , 0. ), SIMD_PI ),
				contact_pos) );
}

//################################################################
//AkashaWorld
#include "bullet/CustumCallbackHolder.hpp"
AkashaWorld::AkashaWorld( irrExt::IrrExtention& irrEx, wisp::v3::WispHandle& client ) :
	m_irrEx(irrEx),
	m_wisp( client ),
	m_visibleExternForce( false ),
	m_visibleDebug( false ),
	m_isSimulationRunnig( true )
{

	bullet::CustomCallbackHolder::setAddedCallback(
			[](
				btManifoldPoint& cp,
				btCollisionObjectWrapper const* o0,
				int partId0,
				int index0,
				btCollisionObjectWrapper const* o1,
				int partId1,
				int index1) -> bool
			{
			constexpr int f = 0;
			// 			BT_TRIANGLE_CONCAVE_DOUBLE_SIDED
			// 			| BT_TRIANGLE_CONVEX_DOUBLE_SIDED;

			// Edge
			btAdjustInternalEdgeContacts(cp, o0, o1, partId0, index0,f);
			btAdjustInternalEdgeContacts(cp, o1, o0, partId1, index1,f);

			return true;
			}
			);
	bullet::CustomCallbackHolder::setProcessCallback([](btManifoldPoint& cp,void* o0,void* o1)->bool
			{
			// 			std::cout << "process.";
			//
			auto* obj0 = static_cast<btCollisionObject*>( o0 );
			auto* obj1 = static_cast<btCollisionObject*>( o1 );

			if ( ((obj0->getBroadphaseHandle()->m_collisionFilterGroup & COL_TYPE_LAND) != 0)
					|| ((obj1->getBroadphaseHandle()->m_collisionFilterGroup & COL_TYPE_LAND) !=0))
			{
			if (cp.getLifeTime()==0 && cp.getDistance()<0.000001f)
			std::cout << "call: " << cp.getLifeTime();
			}
			return false;
			});
	bullet::CustomCallbackHolder::setDestroyedCallback([](void*)->bool
			{
			//Destroyが観測されない?
			std::cout << "destroyed.";
			return false;
			});

	// SolverInfoSetting
	{
		auto& info = BulletManager::Instance().getSolverInfo();
		auto tree = akasha::system::GetSettingTree(
				"MIZUNUKI.Physics.Global");
		assert(tree);

		info.m_numIterations = tree->get<int>("numIterations", 10);
		info.m_splitImpulsePenetrationThreshold =
			tree->get<btScalar>("splitImpulsePenetrationThreshold", btScalar(-.04));
		info.m_erp = tree->get<btScalar>("erp", btScalar(0.2));
		info.m_erp2 = tree->get<btScalar>("erp2", btScalar(0.8));
		info.m_globalCfm = tree->get<btScalar>("globalCfm", btScalar(0.));
		info.m_linearSlop = tree->get<btScalar>("linearSlop",btScalar(0.));
		info.m_damping = tree->get<btScalar>("damping", btScalar(1.));
		// 		info.m_splitImpulse = false;
	}
	//DebugDrawセット
	{
		m_debugDrawer.reset( new btIrrDebugDraw(m_irrEx) ); //TODO : IrrDebugは未実装
		m_debugDrawer->setDebugMode(
				btIDebugDraw::DBG_DrawAabb |
				btIDebugDraw::DBG_DrawWireframe |
				btIDebugDraw::DBG_DrawContactPoints |
				btIDebugDraw::DBG_DrawConstraints |
				btIDebugDraw::DBG_DrawConstraintLimits);
	}


	//レンダラーのセットアップ
	m_wisp.synchronaizedOperate(
			wisp::SynchronizeFuncType( []( irr::IrrlichtDevice * irr )
				{
				//OpenGL以外未対応
				assert(
						irr->getVideoDriver()->getDriverType() == irr::video::EDT_OPENGL );
				//レンダラーセットアップ
				{
				irr::video::IMaterialRenderer* lender =
				new irr::scene::COpenGLMaterialRenderer_STICKER(
						static_cast<COpenGLDriver*>( irr->getVideoDriver() ) );
				RenderType<test>::Type_ =
				( irr::video::E_MATERIAL_TYPE ) irr->getVideoDriver()->addMaterialRenderer(
						lender );
				lender->drop();
				}
				{
				//シャドーカラー
				irr->getSceneManager()->setShadowColor( irr::video::SColor( 100, 0, 0, 10 ) );
				}
				} ) );


	// wisp task
	m_irrNodeUpdate = boost::make_shared< akasha::NodeUpdateTask>();
	m_irrNodeUpdate->init(m_irrEx);

	//環境
	m_land.reset( new land::EnvironmentLand(
				m_irrEx,  m_wisp  ) );

	//視点
	m_view.reset( new PlayerView( m_irrEx, m_wisp  ) );

	//Toyマネージャー
	m_toys.reset( new toy::ToyManager() );

	//ObjectCapture
	//m_capture.reset(new detection::ObjectCapture(m_entityService.getDynamicsWorld(), m_view));
	{
		auto& physics_mgr = BulletManager::Instance();
		//サブステップコールバック
		physics_mgr.getTickCallbackSignal( true ).connect(
				boost::bind( &AkashaWorld::preTick, this, _2 ) );
		physics_mgr.getTickCallbackSignal( false ).connect(
				boost::bind( &AkashaWorld::tick, this, _2));
		physics_mgr.getTickCallbackSignal( true ).connect(
				doSubStepReceptor<bullet::PhysicsWorld::WorldInfo>() );
	}

	////////////////////////////////////////////
	//ContactEventManager
	{
		auto smork = boost::make_shared<effect::SmorkEffectManager>();
		smork->setup(m_irrEx);
		m_smorkMsgr = smork;

		auto smorkFunc =effect::SmorkEffectManager::CreateContactFunc(smork);

		m_colEvent.registExcuter(
				contactEvent::EventGroup::BasicElement,
				contactEvent::EventGroup::BasicLand,
				[this,smorkFunc](btPersistentManifold* pt,btCollisionObject const* element, btCollisionObject const* land){
				smorkFunc(pt, element, land);
				if (m_model)
				{
				for (std::size_t i=0; i< (std::size_t)pt->getNumContacts(); ++i)
				m_model->contactToLandForm(pt->getContactPoint(i), element );
				}
				});

	}

	//キー割り当て

	auto& input = system::AkashaSystem::Instance().getInput();
	typedef system::ii::DegitalID d_id;

	//モデルリロード : U
	input.downAtach(
			d_id::EDegitalInput::VButton_Reload,
			[ this ]( const system::ii::ModifyKey & m )
			{

			if ( m.isCtrl() )
			{
			loadModel( m_modelFilePath );
			}
			}
			);
	//モデルフォームリセット
	input.downAtach(
			d_id::EDegitalInput::VButton_Reset,
			[this]( system::ii::ModifyKey const & m )
			{
			if ( !this->m_model ) return;
			auto const& modelObj = this->m_model->getObserver();

			auto pos =
			modelObj->getElementStatus(0).getWorldTransform().getOrigin();
			pos += btVector3(0.f, SIMD_EPSILON, 0.f);

			InitModelPosition(
					*this->m_model,
					*this->m_land,
					pos);

			} );

	//視点操作

	input.downAtach(
			d_id::EDegitalInput::VButton_ViewDown,
			[this]( system::ii::ModifyKey const& m)
			{
			auto v = m_view->getCameraOffset();
			v.second -= 0.1;
			m_view->setCameraOffset(v);
			});
	input.downAtach(
			d_id::EDegitalInput::VButton_ViewUp,
			[this]( system::ii::ModifyKey const& m)
			{
			auto v= m_view->getCameraOffset();
			v.second += 0.1;
			m_view->setCameraOffset(v);
			}
			);
	input.downAtach(
			d_id::EDegitalInput::VButton_ViewLeft,
			[this](system::ii::ModifyKey const& m)
			{
			auto v = m_view->getCameraOffset();
			v.first -= 0.1;
			m_view->setCameraOffset(v);

			});
	input.downAtach(
			d_id::EDegitalInput::VButton_ViewRight,
			[this](system::ii::ModifyKey const& m)
			{
			auto v = m_view->getCameraOffset();
			v.first += 0.1f;
			m_view->setCameraOffset(v);
			});
	input.downAtach(
			d_id::EDegitalInput::VButton_ViewZoomIn,
			[this](system::ii::ModifyKey const& m){
			m_view->setFOV( m_view->getFOV()-10.f );
			});
	input.downAtach(
			d_id::EDegitalInput::VButton_ViewZoomOut,
			[this](system::ii::ModifyKey const& m){
			m_view->setFOV( m_view->getFOV()+10.f );
			});
	input.downAtach(
			d_id::EDegitalInput::VButton_ViewReset,
			[this](system::ii::ModifyKey const& m)
			{
			m_view->setCameraOffset(std::pair<float,float>(0.f, 0.f));
			m_view->setFOV( 50.f );
			});

	// view change
	input.downAtach(
			d_id::EDegitalInput::VButton_ViewA,
			[this](system::ii::ModifyKey const& m)
			{
			m_view->setViewMode( EViewMode::Celestial);
			});
	input.downAtach(
			d_id::EDegitalInput::VButton_ViewB,
			[this](system::ii::ModifyKey const& m)
			{
			m_view->setViewMode( EViewMode::Target);
			});


	////////////////
	//初期読み込み
	namespace fs = boost::filesystem;
	fs::path currentDirPath = system::AkashaSystem::Instance().getConstant().getCurrentDirectoryPath();

	//loadLand("Resources/block/block.x");
	loadLand( ( currentDirPath.string() + "./Resources/Land.x" ).c_str() );

	//loadLand("Resources/Land1.x");
	loadModel( ( currentDirPath.string() + "./Resources/Basic.txt" ).c_str() );


	// 	m_subModel.reset( new model::ElementModel(*m_wisp, m_land) );


	//////////////////
	//以下テストコード



}

//デスクトラクタ################################################################
AkashaWorld::~AkashaWorld()
{


}

//ウィンドウサイズ変更################################################################
	void
AkashaWorld::onWindowResize( const int w, const int h )
{
	m_view->setCameraAspect( w, h );
}

	void
AkashaWorld::setDebugFlg( int flg )
{
	m_debugDrawer->setDebugMode( flg );
}

int
AkashaWorld::getDebugFlg() const
{
	return m_debugDrawer->getDebugMode();
}



//モデルを初期位置へ################################################################
	void
AkashaWorld::initModelPosition()
{
	if (m_model && m_land)
		InitModelPosition(
				*m_model, *m_land, btVector3(0.f,3000.f, 0.f) );
}

	bool
AkashaWorld::reloadModel()
{
	return loadModel( m_modelFilePath );
}
void AkashaWorld::initSubModelPosition()
{
	if (m_subModel && m_land)
		InitModelPosition(*m_subModel, *m_land, btVector3(0.f,3000.f, 0.f) );
}

//内部ステップ################################################################
	void
AkashaWorld::preTick( const btScalar step )
{
	if ( m_model )
		m_model->subStep( step );
	if ( m_subModel)
		m_subModel->subStep(step);
}

void AkashaWorld::tick( const btScalar step )
{
	if ( m_model )
		m_model->subStepEnd( step );

	if ( m_subModel )
		m_subModel->subStepEnd( step );
}

//更新################################################################
	bool
AkashaWorld::update()
{


	auto& bulletWorld = BulletManager::Instance();

	//bulletステップシミュレーション
	const int innerStep = 8;
	const btScalar fps = 1.f / 60.f;
	// 		const btScalar fps = 1.f / 180.f;
	const btScalar subFps = fps / btScalar( innerStep );

	//モデルアップデート
	m_model->update( fps, subFps, m_isSimulationRunnig );
	if (m_subModel)
		m_subModel->update(fps, subFps, m_isSimulationRunnig);

	if (m_isSimulationRunnig)
	{
		//bullet step
		bulletWorld.stepSimulation( fps, innerStep, subFps );

		// ObjectOption Routine
		auto& objArray = bulletWorld.getCollisionObjectArray();
		for(int i=0; i<objArray.size(); ++i)
			ObjectOptionRoutine(objArray[i]);


		{
			using collisionBufferType = contactEvent::ContactEventManager::inputBufferType;
			collisionBufferType buf;
			buf.reserve(1024);

			doStepReceptor<collisionBufferType> res(boost::ref(buf));
			res( bulletWorld.getDispatcher(), fps );

			boost::sort(buf);
			boost::erase(
					buf,
					boost::unique<boost::return_found_end>(buf)
					);

			m_colEvent.excute(buf);
		}
	}


	// Model step end
	m_model->stepEndUpdate( fps, subFps, m_isSimulationRunnig );
	if (m_subModel)
		m_subModel->stepEndUpdate(fps, subFps, m_isSimulationRunnig);

	m_smorkMsgr->update(fps); //TODO

	//Luaエラーメッセージの表示
	if ( m_model->getLuaErrorMesage().size() > 0 )
	{
		//TODO : wisp実装待ち
	}


	//Environment更新
	m_land->update( fps );

	//デバックDraw
	if ( isVisibleDebug() )
		bulletWorld.debugDrawWorld();
	if ( isVisibleExternForce() )
		m_model->drawExternForce();


	//カメラ更新
	m_view->update(*m_model->getObserver() );

	//task update
	m_irrNodeUpdate->apply();

	return true;
}


