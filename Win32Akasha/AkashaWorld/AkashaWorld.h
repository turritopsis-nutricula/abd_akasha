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


#include "akashaWorldCommon.h"  // �e���b�ݒ�
#include "akashaWorldType.h"    // �^


#include "../AkashaApplication/network/WorldData.hpp" //���[���h�f�[�^

#include "other/OtherElementModel.hpp" //���������ւ���ƃG���[

#include "BulletWorldManager.h" // bullet�Ǘ��N���X
#include "detection/DetectionWorld.hpp"


#include "BulletExtention.h"

#include "../AkashaApplication/Instantiater.hpp"

//################################################################
// �C���N���[�h

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


	//! Akasha���[���h
	// ���z������Ԃ��Ǘ����܂��B
	class AkashaWorld {
		public:
			AkashaWorld() = delete;
			AkashaWorld(irrExt::IrrExtention&,wisp::v3::WispHandle& );

			~AkashaWorld();


			//! �t�@�C���p�X����Land�ǂݍ���
			bool loadLand(const irr::io::path& landFilePath);

			//!�t�@�C�����烂�f���ǂݍ���
			bool loadModel(const irr::io::path& modelFilePath);

			//! ���f���������ʒu��
			void initModelPosition();

			//! ���f���ēǂݍ���
			bool reloadModel();

			//! Sub Model Load
			bool loadSubModel(irr::io::path const& filePath);

			//! Sub Model Position Init
			void initSubModelPosition();

			//! �E�B���h�E�T�C�Y�ύX
			void onWindowResize(const int w, const int h);

			//! ��p�͕\����Ԏ擾
			inline bool isVisibleExternForce() const{return m_visibleExternForce; };

			//! ��p�͕\��
			void visibleExternForce(){ m_visibleExternForce = true;};

			//! ��p�͔�\��
			void hiddenExternForce(){m_visibleExternForce = false; };

			//! �f�o�b�N�\�����
			inline bool isVisibleDebug() const{ return m_visibleDebug; };

			//! �f�o�b�N�\��
			void visibleDebyg(){
				BulletManager::Instance().setDebugDrawer(m_debugDrawer.get());
				m_visibleDebug = true;
			};

			//! �f�o�b�N��\��
			void hiddenDebug(){
				BulletManager::Instance().setDebugDrawer(nullptr);
				m_visibleDebug = false;
			};

			//! �f�o�b�N�\���t���O���Đݒ�
			void setDebugFlg(int flg);
			int getDebugFlg() const;

			//! �����X�e�b�v�X�V
			void preTick(const btScalar);
			void tick(const btScalar );

			//! �X�V
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


			//! ���ڃm�[�h�̐ݒ�
// ���C���J������ύX���܂��B(�C���^�[�t�F�C�X�ύX�\��)
// 			  @param node ���ڑΏۂ�Akasha�I�u�W�F�N�g
			inline bool setCameraTarget(/*spPhysicsEntity ent*/){

				return true;
			}

			//! �J�����^�[�Q�b�g�̒ǉ�
			void addCameraTarget(/*wpPhysicsEntity ent*/){

			}



			///////////////////
			//�l�b�g���[�N
			//! ���f���X�V�f�[�^�̎擾
			void
				getNetworkModelUpdatar(network::OtherModelUpdataer& d) const
				{
					//TODO:�v����
				}
			void
				getNetworkModelBase(/* */) const
				{
					//TODO:�v����
				}

			void
				addNetworkOtherModel(/* */)
				{
					//TODO:�v����
				}
			void
				applyNetworkOtherModelUpdata(boost::uuids::uuid hostID, network::OtherModelUpdataer const& d)
				{
					//TODO:�v����
				}


		private:

			irrExt::IrrExtention& m_irrEx;
			wisp::v3::WispHandle& m_wisp;
			boost::shared_ptr<akasha::NodeUpdateTask> m_irrNodeUpdate;

			//TODO: BulletWorld����ɉ������K�v�L��
			boost::scoped_ptr<btGhostPairCallback> m_ghostPairCallback;
			boost::scoped_ptr<btIDebugDraw> m_debugDrawer;

			boost::shared_ptr<land::EnvironmentLand> m_land; // �����h

			boost::scoped_ptr<model::ElementModel> m_model; //���f��
			irr::io::path m_modelFilePath; //<! ���݂̃��f���t�@�C���p�X

			boost::scoped_ptr<model::ElementModel> m_subModel; //Sub model

			std::vector<model::other::OtherElementModel> m_otherModel; //�l�b�g���[�N�z���̃��f��
			boost::shared_ptr<PlayerView> m_view; // �r���[

			boost::scoped_ptr<toy::ToyManager> m_toys; //toy

			boost::scoped_ptr<detection::ObjectCapture> m_capture;

			contactEvent::ContactEventManager m_colEvent;

			boost::shared_ptr<effect::SmorkEffectManager> m_smorkMsgr; // TODO

			bool m_visibleExternForce;       //!< ��p�͕\��bool
			bool m_visibleDebug;             //!< �f�o�b�N�\��bool

			bool m_isSimulationRunnig;



	};
}// namespace akasha


#endif //AKASHA_WORLD__H
