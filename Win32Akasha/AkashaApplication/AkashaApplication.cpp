#include "../AkashaWorld/AkashaWorld.h"


#include <winsock2.h>
#include "AkashaApplication.h"

#include <wisp_v3/wisp.hpp>

#include "SoundManager.hpp"

using namespace akasha;
using namespace input;

using namespace irr;
using namespace core;
using namespace video;

AkashaApplication::AkashaApplication( HWND windowHandle ):
	m_trueFrameRate( 0.0 ),
	m_networkFrontEnd(boost::make_shared< net::NetworkFrontEnd>())
	//m_pControler(new CAkashaControllerInput()),
	//m_irrManager(new irrlicht::CIrrlichtManager()),
{
	//TODO : ��O�������l����

	//Network
	m_network.setFrontEnd( m_networkFrontEnd );

	//Irrlicht�Z�b�g�A�b�v

	assert( ::IsWindow( windowHandle ) );


	auto& wisp = wisp::v3::CreateWispHandle();

	//TODO : �e�n���h���̓o�^�ƃT�[�o�[�X�^�[�g

	using func_type = boost::function<void (irr::IrrlichtDevice*)>;
	auto onInit =
			[this,windowHandle]() ->
			irr::IrrlichtDevice*
			{
				irr::video::SExposedVideoData videoData( windowHandle );
				irr::SIrrlichtCreationParameters param;

				param.DriverType = irr::video::EDT_OPENGL;
				//param.DriverType = irr::video::EDT_DIRECT3D9;
				param.WindowId = videoData.OpenGLWin32.HWnd;
				param.ZBufferBits = 24;
				param.Stencilbuffer = true;
				param.WindowSize = irr::core::dimension2du( 1280, 720 );

				auto* irr = irr::createDeviceEx(param);

				m_irrExt.init(irr);
				irr->getVideoDriver()->setAmbientLight(
						irr::video::SColorf( 1.0, 1.0, 1.0 ) );

				return irr;
			};


	bool isSucess = wisp.start(
			std::move( onInit ),
			boost::bind( &irrExt::IrrExtention::run,
				boost::ref(m_irrExt), _1),
			 [this]( irr::IrrlichtDevice * irr) {
			 m_irrExt.release( irr );
			 }  );

	assert(isSucess);

	//�I�[�f�B�I�Z�b�g�A�b�v
	//     cricket::CricketManager::Instance().setup();

	//�t�@�C�����[�_�[�ǉ�
	/*scene::CXMeshFileLoaderEx *xLoaderEx = new scene::CXMeshFileLoaderEx(
	  m_upIrrDevice->getSceneManager(), m_upIrrDevice->getFileSystem()
	  );
	  m_upIrrDevice->getSceneManager()->addExternalMeshLoader(xLoaderEx);*/


	m_pAkashaWorld.reset( new AkashaWorld(m_irrExt, wisp));


}

//�f�X�g���N�^################
AkashaApplication::~AkashaApplication()
{
	wisp::v3::CreateWispHandle().hostCloseJoin();
}

//�v���O�������[�v################
bool AkashaApplication::run()
{

	auto& wisp = wisp::v3::CreateWispHandle();
	auto status = wisp.getHostStatus();


	//World�X�V
	m_pAkashaWorld->update();

	// �l�b�g���[�N�A�b�v�f�[�g
	m_network.update();

	m_trueFrameRate = m_frameRateTimer.elapsed().wall; // 1���[�v���Ԃ��L��
	//1���[�v���Ԃɂ��Wait Sleep�̐��x������ł�
	while ( m_frameRateTimer.elapsed().wall < ( 1000000000/60 ) )
	{
		boost::this_thread::yield();
	}


	// Extention update
	m_irrExt.extentionTaskPost();

	// �`�施��Post
	wisp.sendCommand();

	//1���[�v���Ԃ��v���J�n
	m_frameRateTimer.start();

	//fps�̕ۑ�
	m_irrFps = status.fps_;


	return status.isRun_;
}

//################################################################
//�E�B���h�E���T�C�Y�ʒm
void AkashaApplication::onResize( const int w, const int h )
{

	wisp::v3::CreateWispHandle().synchronaizedOperate(
			wisp::SynchronizeFuncType(
				[w, h]( irr::IrrlichtDevice * irr )
				{
				auto* video = irr->getVideoDriver();
				video->OnResize( irr::core::dimension2d<unsigned int>( w, h ) );
				}
				)
			);


	m_pAkashaWorld->onWindowResize( w, h );

}

//##############################
//Close

void AkashaApplication::close()
{

// 	wisp::v2::WispHandle().serverCloseRequest();
}

void AkashaApplication::postGuiEvent( irr::SEvent const& e)
{
	m_irrExt.postEvent( e );
}

//�l�b�g���[�N�֘A################
	boost::shared_ptr< net::NetworkFrontEnd >
AkashaApplication::getNetworkFrontEnd()
{
	return m_networkFrontEnd;
}

void AkashaApplication::runNetworkServer(
		net::ConnectionInfo const& info, std::string const& port)
{
	m_network.runServer(info, port);
}

void AkashaApplication::runNetworkClient(
		net::ConnectionInfo const& info,
		std::string const& address, std::string const& port)
{
	m_network.runClient(info, address, port);
}
void AkashaApplication::closeNetwork()
{
	m_network.close();
}
//�t�@�C���֘A################################################################
bool AkashaApplication::openModelFile( LPCTSTR modelFilePath )
{
	//OutDebug(_T(modelFilePath));
	m_pAkashaWorld->loadModel( io::path( modelFilePath ) );
	return true;
};

bool AkashaApplication::openSubModelFile( LPCTSTR filePath )
{
	m_pAkashaWorld->loadSubModel( io::path( filePath) );
	return true;
}
bool AkashaApplication::openLandFile( LPCTSTR landFilePath )
{
	//OutDebug(_T(landFilePath));
	m_pAkashaWorld->loadLand( io::path( landFilePath ) );
	return false;
}

//�\���֘A################################################################
bool AkashaApplication::toggleShowExternForce()
{
	if ( m_pAkashaWorld->isVisibleExternForce() )
		m_pAkashaWorld->hiddenExternForce();
	else
		m_pAkashaWorld->visibleExternForce();
	return m_pAkashaWorld->isVisibleExternForce();
}

bool AkashaApplication::toggleShowDebug()
{
	if ( m_pAkashaWorld->isVisibleDebug() )
		m_pAkashaWorld->hiddenDebug();
	else
		m_pAkashaWorld->visibleDebyg();

	return m_pAkashaWorld->isVisibleDebug();
}

void AkashaApplication::setBulletDebugDrawFlag( int flg )
{
	m_pAkashaWorld->setDebugFlg( flg );
}

int AkashaApplication::getBulletDebugDrawFlag() const
{
	return m_pAkashaWorld->getDebugFlg();
}





