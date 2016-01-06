#include "AkashaFrames.h"

#include "nainai/keycode.hpp"
#include "nainai/BuildConfig.hpp"

using namespace akasha;

	BOOL
Win32AkashaApplication::InitInstance()
{
	// ウィンドウ生成
	m_view.Create();
	//m_logWindow.Create();

	//AkashaApplicationを構成
	if ( m_view.IsWindow() )
		m_applicationPtr = boost::make_shared<akasha::AkashaApplication>(
				m_view.GetHwnd() );


	//リソースの読み込み
// 	HINSTANCE hInst = Win32xx::GetApp()->GetResourceHandle();
//
// 	HRSRC resource = ::FindResource(hInst,MAKEINTRESOURCE(IDR_LUA1),"LUA");
// 	HGLOBAL globalHandle = ::LoadResource( hInst, resource);
// 	if (globalHandle)
// 		RE_elementModelConstractLuaCode = (char*)(::LockResource(globalHandle));

	//Joypadセットアップ
	{

		//TODO : Exception
		m_inputBridge.init(GetInstanceHandle(), m_view.GetHwnd() );

		m_view.SetActiveWindow();
	}

	if ( !m_applicationPtr )
		::PostQuitMessage( 0 );


	{
		using akasha::system::ii::DegitalID;
		using nai::keycode;
		auto& i = m_inputBridge;

		i.setKeyMap( keycode::K_UP, DegitalID::EDegitalInput::VButton_0 );
		i.setKeyMap( keycode::K_DOWN, DegitalID::EDegitalInput::VButton_1 );
		i.setKeyMap( keycode::K_LEFT, DegitalID::EDegitalInput::VButton_2 );
		i.setKeyMap( keycode::K_RIGHT, DegitalID::EDegitalInput::VButton_3 );

		i.setKeyMap( keycode::K_Z, DegitalID::EDegitalInput::VButton_4 );
		i.setKeyMap( keycode::K_X, DegitalID::EDegitalInput::VButton_5 );
		i.setKeyMap( keycode::K_C, DegitalID::EDegitalInput::VButton_6 );
		i.setKeyMap( keycode::K_A, DegitalID::EDegitalInput::VButton_7 );
		i.setKeyMap( keycode::K_S, DegitalID::EDegitalInput::VButton_8 );
		i.setKeyMap( keycode::K_D, DegitalID::EDegitalInput::VButton_9 );
		i.setKeyMap( keycode::K_V, DegitalID::EDegitalInput::VButton_10 );
		i.setKeyMap( keycode::K_B, DegitalID::EDegitalInput::VButton_11 );
		i.setKeyMap( keycode::K_F, DegitalID::EDegitalInput::VButton_12 );

		i.setKeyMap( keycode::K_H, DegitalID::EDegitalInput::VButton_S0 );
		i.setKeyMap( keycode::K_J, DegitalID::EDegitalInput::VButton_S1 );
		i.setKeyMap( keycode::K_N, DegitalID::EDegitalInput::VButton_S2 );
		i.setKeyMap( keycode::K_M, DegitalID::EDegitalInput::VButton_S3 );

		//TODO ; NextVersion Wait...
#if(NAINAI_VERSION_MAJOR==1 && NAINAI_VERSION_MINOR==1 && NAINAI_VERSION_HOTFIX==1)
		i.setKeyMap( keycode::K_LCONTOL, DegitalID::EDegitalInput::VButton_Ctrl_Modify );
#else
		i.setKeyMap( keycode::K_LCONTROL, DegitalID::EDegitalInput::VButton_Ctrl_Modify );
#endif
		i.setKeyMap( keycode::K_Y, DegitalID::EDegitalInput::VButton_YForce );
		i.setKeyMap( keycode::K_U, DegitalID::EDegitalInput::VButton_Reload );
		i.setKeyMap( keycode::K_R, DegitalID::EDegitalInput::VButton_Reset );

		i.setKeyMap( keycode::K_L, DegitalID::EDegitalInput::VButton_ViewUp );
		i.setKeyMap( keycode::K_K, DegitalID::EDegitalInput::VButton_ViewDown );
		i.setKeyMap( keycode::K_COMMA, DegitalID::EDegitalInput::VButton_ViewLeft );
		i.setKeyMap( keycode::K_PERIOD, DegitalID::EDegitalInput::VButton_ViewRight );
		i.setKeyMap( keycode::K_O, DegitalID::EDegitalInput::VButton_ViewZoomOut );
		i.setKeyMap( keycode::K_I, DegitalID::EDegitalInput::VButton_ViewZoomIn );
		i.setKeyMap( keycode::K_P, DegitalID::EDegitalInput::VButton_ViewReset );

		i.setKeyMap( keycode::K_F8, DegitalID::EDegitalInput::VButton_ViewA );
		i.setKeyMap( keycode::K_F9, DegitalID::EDegitalInput::VButton_ViewB );
	}



	akasha::system::AkashaSystem::Instance().getLogger()(
			akasha::system::log::ApplicationInfo
			{ _T( "Yururu Init Compl." ) } );


	//デバックウィンドウにアプリケーションをセット
	m_debugWindow.setApplication( m_applicationPtr );

	//ウィンドウ配置の初期化
	OnMainWindowResize( m_view.GetClientRect() );

	return TRUE;
}
