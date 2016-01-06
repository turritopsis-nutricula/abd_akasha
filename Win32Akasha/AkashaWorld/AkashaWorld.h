#ifdef MSVC
#pragma unmanaged
#endif

#ifndef AKASHA_WORLD__H
#define AKASHA_WORLD__H


#include "contactEvent/ContactEventManager.hpp"

//Irrlicht################################################################
// #define _IRR_STATIC_LIB_
#include <irrlicht.h>


//wisp #################################################################
#include <wisp_v3/wisp.hpp>
#include <wisp_v3/irrExt.hpp>

//bullet################################################################
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"


//Akasha################################################################


#include "akashaWorldCommon.h"  // 各種基礎設定
#include "akashaWorldType.h"    // 型


#include "../AkashaApplication/network/WorldData.hpp" //ワールドデータ

#include "other/OtherElementModel.hpp" //ここを入れ替えるとエラー

#include "BulletWorldManager.h" // bullet管理クラス
#include "detection/DetectionWorld.hpp"


#include "BulletExtention.h"

#include "../AkashaApplication/Instantiater.hpp"

//################################################################
// インクルード

#ifdef _DEBUG

#include "assert.h"
#include "tchar.h"


#endif //_DEBUG

//boost################################################################
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/unordered_map.hpp>



#ifdef WIN32
//Irr
/*#define WIN32_LEAN_AND_MEAN
#include <Winsock2.h>
#include <windows.h>
*/
#ifdef MSVC
#pragma comment(lib, "Irrlicht.lib")

//bullet
#pragma comment(lib,"LinearMath.lib")
#pragma comment(lib,"BulletCollision.lib")
#pragma comment(lib,"BulletDynamics.lib")
#endif //MSVC

#define USE_WIN32_THREADING 1

//Akasha
#endif // WIN32





//################################################################
//AkashaWorld

namespace akasha {
	//fwd
	class NodeUpdateTask;
	class PlayerView;
	namespace model
	{
		class ElementModel;
	}
	namespace land
	{
		class EnvironmentLand;
	}
	namespace avatar
	{
		class AvatarBase;
	}
	namespace toy
	{
		class ToyManager;
	}
	namespace detection
	{
		class ObjectCapture;
	}
	namespace effect
	{
		class SmorkEffectManager;
	}


	//! Akashaワールド
	// 仮想物理空間を管理します。
	class AkashaWorld {
		public:
			AkashaWorld() = delete;
			AkashaWorld(irrExt::IrrExtention&,wisp::v3::WispHandle& );

			~AkashaWorld();


			//! ファイルパスからLand読み込み
			bool loadLand(const irr::io::path& landFilePath);

			//!ファイルからモデル読み込み
			bool loadModel(const irr::io::path& modelFilePath);

			//! モデルを初期位置へ
			void initModelPosition();

			//! モデル再読み込み
			bool reloadModel();

			//! Sub Model Load
			bool loadSubModel(irr::io::path const& filePath);

			//! Sub Model Position Init
			void initSubModelPosition();

			//! ウィンドウサイズ変更
			void onWindowResize(const int w, const int h);

			//! 作用力表示状態取得
			inline bool isVisibleExternForce() const{return m_visibleExternForce; };

			//! 作用力表示
			void visibleExternForce(){ m_visibleExternForce = true;};

			//! 作用力非表示
			void hiddenExternForce(){m_visibleExternForce = false; };

			//! デバック表示状態
			inline bool isVisibleDebug() const{ return m_visibleDebug; };

			//! デバック表示
			void visibleDebyg(){
				BulletManager::Instance().setDebugDrawer(m_debugDrawer.get());
				m_visibleDebug = true;
			};

			//! デバック非表示
			void hiddenDebug(){
				BulletManager::Instance().setDebugDrawer(nullptr);
				m_visibleDebug = false;
			};

			//! デバック表示フラグを再設定
			void setDebugFlg(int flg);
			int getDebugFlg() const;

			//! 内部ステップ更新
			void preTick(const btScalar);
			void tick(const btScalar );

			//! 更新
			bool update();

			//! sumiration stop
			bool isSimulationRunning() const
			{
				return m_isSimulationRunnig;
			}

			void setSimulationRunning(bool const b)
			{
				m_isSimulationRunnig = b;
			}


			//! 注目ノードの設定
// メインカメラを変更します。(インターフェイス変更予定)
// 			  @param node 注目対象のAkashaオブジェクト
			inline bool setCameraTarget(/*spPhysicsEntity ent*/){

				return true;
			}

			//! カメラターゲットの追加
			void addCameraTarget(/*wpPhysicsEntity ent*/){

			}



			///////////////////
			//ネットワーク
			//! モデル更新データの取得
			void
				getNetworkModelUpdatar(network::OtherModelUpdataer& d) const
				{
					//TODO:要実装
				}
			void
				getNetworkModelBase(/* */) const
				{
					//TODO:要実装
				}

			void
				addNetworkOtherModel(/* */)
				{
					//TODO:要実装
				}
			void
				applyNetworkOtherModelUpdata(boost::uuids::uuid hostID, network::OtherModelUpdataer const& d)
				{
					//TODO:要実装
				}


		private:

			irrExt::IrrExtention& m_irrEx;
			wisp::v3::WispHandle& m_wisp;
			boost::shared_ptr<akasha::NodeUpdateTask> m_irrNodeUpdate;

			//TODO: BulletWorldより後に解放する必要有り
			boost::scoped_ptr<btGhostPairCallback> m_ghostPairCallback;
			boost::scoped_ptr<btIDebugDraw> m_debugDrawer;

			boost::shared_ptr<land::EnvironmentLand> m_land; // ランド

			boost::scoped_ptr<model::ElementModel> m_model; //モデル
			irr::io::path m_modelFilePath; //<! 現在のモデルファイルパス

			boost::scoped_ptr<model::ElementModel> m_subModel; //Sub model

			std::vector<model::other::OtherElementModel> m_otherModel; //ネットワーク越しのモデル
			boost::shared_ptr<PlayerView> m_view; // ビュー

			boost::scoped_ptr<toy::ToyManager> m_toys; //toy

			boost::scoped_ptr<detection::ObjectCapture> m_capture;

			contactEvent::ContactEventManager m_colEvent;

			boost::shared_ptr<effect::SmorkEffectManager> m_smorkMsgr; // TODO

			bool m_visibleExternForce;       //!< 作用力表示bool
			bool m_visibleDebug;             //!< デバック表示bool

			bool m_isSimulationRunnig;



	};
}// namespace akasha


#endif //AKASHA_WORLD__H
