/**
  Akasha �����N���X�Q
  */

#ifndef AKASHA_APPLICATION__H
#define AKASHA_APPLICATION__H



//Akasha�w�b�_ TODO �w�b�_�����̐���
#include "net/FrontEnd.hpp"
#include "net/Network.hpp"

#include <wisp_v3/wisp.hpp>
#include <wisp_v3/irrExt.hpp>

//STL�w�b�_
#include <vector>
#include <iostream>
#include <memory>

//Boost�w�b�_
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <boost/timer/timer.hpp>

//Irrlicht�w�b�_
#include "irrlicht.h"





namespace akasha
{
	//fwd
	class AkashaWorld;
	namespace input
	{
		class IAkashaControllerInput;
		typedef boost::scoped_ptr<IAkashaControllerInput> pIAkashaControllerInput;
	}


	typedef boost::scoped_ptr<AkashaWorld> pAkashaWorld;

	//################################################################
	//! Akasha �A�v���P�[�V����
	/**
	 * Akasha�̒����N���X�B�S�Ă�API��񋟁B
	 * - Irrlicht�̊�{�ݒ���Ǘ��B
	 */
	class AkashaApplication
	{
		public:
			/**
			 * �E�B���h�E�n���h�����K�{
			 */
			AkashaApplication( HWND windowHandle );
			/**
			 * ���̒i�K�œ����̑S�ẴI�u�W�F�N�g��delete�����B
			 */
			~AkashaApplication();

			//! �v���O�������[�v
			/**
			 * ���C�����[�v�B
			 * while()�ŕ�ރ����V�B
			 * @return ���[�v�t���O�B�I����false�B
			 */
			bool run();

			//! ���T�C�Y����
			/**
			 * �E�B���h�E�T�C�Y�ύX���ɌĂԕK�v������B
			 * @param w �����s�N�Z����
			 * @param h �c���s�N�Z����
			 */
			void onResize( const int w, const int h );

			//Close
			void close();

			// GuiEvent
			void postGuiEvent( irr::SEvent const&);

		public:

			//! FPS�擾
			/**
			 * Irrlicht��FPS��񋟁B
			 */
			int getFPS() const
			{
				return m_irrFps;
			}

			//! NetworkFrontEnd�̒�
			boost::shared_ptr< net::NetworkFrontEnd >
				getNetworkFrontEnd();

			//! �l�b�g���[�N�E�T�[�o�[
			void runNetworkServer(net::ConnectionInfo const&, std::string const&);
			//! �l�b�g���[�N�N���C�A���g
			void runNetworkClient(
					net::ConnectionInfo const&, std::string const& address, std::string const& port);
			//! �l�b�g���[�N�N���[�Y
			void closeNetwork();

			//! Frame���[�g�擾
			/**
			 * ���O�̂�����^�C�}�[�ɂ��t���[�����[�g�擾
			 */
			inline double getFrameRate()
			{
				return m_trueFrameRate;
			}



			//���b�Z�[�W�n����################
			//! ���f���t�@�C�����J��
			/**
			 * ���f���t�@�C���̃p�X��������ă��f�����J��
			 */
			bool openModelFile( LPCTSTR modelFilePath );

			//! Open SubModel by File
			bool openSubModelFile( LPCTSTR filePath );

			//! �����h�t�@�C�����J��
			/**
			 * �����h�t�@�C���̃p�X��������Ĕ���̌�ɊJ��
			 */
			bool openLandFile( LPCTSTR landFilePath );

			//! ��p�͕\���g�O��
			/**
			 * �\����Ԃ�Ԃ�
			 */
			bool toggleShowExternForce();

			//! �f�o�b�N�\���g�O��
			/**
			 * �\����Ԃ�Ԃ�
			 */
			bool toggleShowDebug();

			//! �����G���W���̃f�o�b�N�`��p�����[�^��ݒ肷��
			void setBulletDebugDrawFlag( int flg );
			int getBulletDebugDrawFlag() const;

		private:
			HWND m_windowHandle; // Window �n���h��

			irrExt::IrrExtention m_irrExt;

			pAkashaWorld m_pAkashaWorld; //!<Akasha���[���h
			boost::timer::cpu_timer m_frameRateTimer;// FPS����̂��߂̃^�C�}�[
			double m_trueFrameRate; // 1Frame�̏�������

			int m_irrFps;
			net::Network m_network;
			boost::shared_ptr< net::NetworkFrontEnd > m_networkFrontEnd;

	};

}

#endif //AKASHA_APPLICATION__H
