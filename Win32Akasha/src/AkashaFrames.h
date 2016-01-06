#ifndef AKASHA_FRAMES__H
#define AKASHA_FRAMES__H

//Akasha Includes
#include "../AkashaSystem/AkashaSystem.hpp"
#include "../AkashaApplication/AkashaApplication.h"

//resurces
#include "res/resource.h"

#include <wisp/win32Mouse/win32Mouse.hpp>

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <windows.h>
#include <commdlg.h>
#include <tchar.h>
#include <commctrl.h>

#include <wincore.h>
#include <frame.h>
#include <winutils.h>

//#include <stdcontrols.h>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/optional.hpp>

namespace akasha
{
namespace win32
{
using commandReciverType =
	boost::function < BOOL ( Win32xx::CWnd&, WPARAM, LPARAM ) >;
using WndProcFilterType =
	boost::function < bool ( Win32xx::CWnd&, UINT&, WPARAM&, LPARAM& ) >;
}
}
//入力
#include "InputBridge/InputBridge.hpp"

//#include "Win32Debug.hpp"
#define DEBUG_OUT(t)

#include "AkashaMainWindow.hpp"
#include "LogWindow/LogWindow.hpp"
#include "DebugWindow/DebugWindow.hpp"
#include "Network/NetworkDialog.hpp"

#include <wisp_v3/wisp.hpp>

//char* RE_elementModelConstractLuaCode;

class VersionDialog : public Win32xx::CDialog
{
	Win32xx::CStatic m_versionText;
public:
	VersionDialog(Win32xx::CWnd* parent) :
		CDialog(IDD_VERSION, parent){}

	BOOL OnInitDialog() override
	{
		CDialog::AttachItem( IDM_STATIC_VERSION, m_versionText);
		auto const& sysConst = akasha::system::AkashaSystem::Instance().getConstant();
		auto text = boost::format(
				"Yururu : %4%\n version %1%.%2%.%3%"
				) % sysConst.getMajorVersion() %
			sysConst.getMinorVersion() %
			sysConst.getRevision() %
			sysConst.getCodeName();

		m_versionText.SetWindowText( text.str().c_str() );
		return TRUE;
	}
};

// ###############################################################
// Aplication


//! アプリケーション
class Win32AkashaApplication : public Win32xx::CWinApp
{
private:

	akasha::win32::WndProcFilterType m_WndProcReciver;
	akasha::win32::commandReciverType m_commandReciver;

	akasha::win32::Win32Akasha_View m_view;
	akasha::win32::LogViewFrame m_logWindow;
	akasha::win32::DebugWindow m_debugWindow;
	akasha::win32::NetworkConsole m_networkSelectDialog;
	VersionDialog m_versionDialog;

	akasha::win32::InputBridge m_inputBridge;

	boost::shared_ptr<wisp::v3::WispHandle> m_wispClient;
	boost::shared_ptr<akasha::AkashaApplication> m_applicationPtr;


public:

	Win32AkashaApplication() :
		m_view( m_WndProcReciver, m_commandReciver ),
		m_logWindow( m_commandReciver ),
		m_networkSelectDialog( &m_view ),
		m_versionDialog( &m_view ),
		m_wispClient( nullptr ),
		m_applicationPtr( nullptr )
	{
		DEBUG_OUT( _T( "## Akasha Start. ################################\n" ) );

		m_WndProcReciver = boost::bind( &Win32AkashaApplication::wndProcFilter,
				this, ::_1, ::_2, ::_3, ::_4 );
		m_commandReciver = boost::bind( &Win32AkashaApplication::OnWindowCommand,
				this, ::_1, ::_2, ::_3 );
	}

	~Win32AkashaApplication()
	{
		DEBUG_OUT( _T( "## Akasha Quit. ################################\n" ) );
	}

	virtual BOOL InitInstance();

	//メッセげループ
	virtual int MessageLoop()
	{

		MSG Msg { 0 };
		//ループ開始
		while ( Msg.message != WM_QUIT )
		{
			if ( PeekMessage( &Msg, NULL, 0U, 0U, PM_REMOVE ) )
			{
				::TranslateMessage( &Msg );
				::DispatchMessage( &Msg );
			}
			else
			{

				if ( m_view.IsWindow() )
				{
					m_inputBridge.update();

					if ( !m_applicationPtr->run() )
					{
						DEBUG_OUT( _T( "Application Close.\n" ) );
						::PostQuitMessage( 0 );
						break;
					}

					auto const& sysConst =
						akasha::system::AkashaSystem::Instance().getConstant();
					m_view.SetWindowText(
							(
							 boost::format( "Yururu - %1%  %2%" ) %
							 sysConst.getAbbCodeName() %
							 m_applicationPtr->getFPS()
							).str().c_str()
							);
				}
				else
					::PostQuitMessage( 0 );
			}
		} // while (Msg.message)

		assert(Msg.message==WM_QUIT);

		return LOWORD( Msg.wParam );
	}

	//ウィンドウプロシージャフィルター
	bool wndProcFilter( Win32xx::CWnd& window, UINT& uMsg, WPARAM& wParam, LPARAM& lParam )
	{
		if ( window.GetHwnd() == m_view.GetHwnd() )
		{
			if ( auto e = wisp::makeMouseEvent( window.GetHwnd(), uMsg, wParam, lParam ) )
			{
				m_applicationPtr->postGuiEvent( *e );
			}

			//メインウィンドウに関して
			switch( uMsg )
			{
			case WM_CLOSE: //Window Close Botton

				if(
						MessageBox(
							m_view.GetHwnd(),
							"Realy?", "確認",
							MB_YESNO ) == IDNO
				  )
				{
					return false; // 閉じないためにはDefWindowProc()に処理させてはならない
				}
				else
				{
					// application close
					m_applicationPtr->close();
					break;
				}

			case WM_SIZE: // メインウィンドウのリサイズ
				OnMainWindowResize( window.GetClientRect() );
				break;
			case WM_SETFOCUS: //フォーカスが戻る毎にDirectInputの有効化
				break;
			case WM_DROPFILES: //ファイルドロップ
				OnMainWindowDropFiles( wParam, lParam );
				break;
			}
		}

		return true;
	}

	//コマンドの横取り
	BOOL
		OnWindowCommand( Win32xx::CWnd& window, WPARAM wParam, LPARAM lParam )
		{
			if ( window.GetHwnd() == m_view.GetHwnd() )
			{
				switch( LOWORD( wParam ) )
				{
				case IDM_FILE_EXIT: //終了
					::PostQuitMessage( 0 );
					return TRUE;

				case IDM_FILE_OPEN_MODEL: //モデルファイル
					OnModelFileOpen();
					break;
				case IDM_FILE_OPEN_LAND:
					OnLandFileOpen();
					break;
				case IDM_FILE_OPEN_SUBMODEL:
					OnSubModelFileOpen();
					break;
				case IDM_WINDOW_LOG: //ログウィンドウ
					{
						if ( m_logWindow.IsWindow() )
							m_logWindow.Destroy();
						else
							m_logWindow.Create();
						break;
					}
				case IDM_WINDOW_DEBUG: //デバックウィンドウ
					{
						if ( m_debugWindow.IsWindow() )
							m_debugWindow.Destroy();
						else
							m_debugWindow.Create();
						break;
					}
				case IDM_NETWORK_DIALOG: //ネットワークダイアログ
					{
						m_networkSelectDialog.setApplication( m_applicationPtr );
						m_networkSelectDialog.show();
						break;
					}
				case IDM_SHOW_VERSION_DIALOG: //Version Dialog
					{
						if (m_versionDialog.IsWindow())
							m_versionDialog.Destroy();
						else
							m_versionDialog.Create();
						break;
					}
				}
			}
			return FALSE;
		}

private:
	void OnMainWindowResize( const Win32xx::CRect& r )
	{
		//TODO:テストまだ
		if( m_applicationPtr && r.Width() != 0 && r.Height() != 0 )
			m_applicationPtr->onResize( r.Width(), r.Height() );
	}

	//! ファイルどろっぷ処理
	void OnMainWindowDropFiles( WPARAM wParam, LPARAM lParam )
	{
		//総数取得
		unsigned int dropFileCount = ::DragQueryFile( ( HDROP )wParam, -1, NULL, 0 );
		std::vector<std::string> fileList; //TODO: UNICODE環境を考えていない
		TCHAR buffer[ MAX_PATH + 1 ];
		for ( unsigned int i = 0; i < dropFileCount; i++ )
		{
			::DragQueryFile( ( HDROP )wParam, i, buffer, MAX_PATH );
			fileList.push_back( buffer );
			DEBUG_OUT( fileList.back() );
		}

		//TODO: ファイル判別してモデルファイル以外も読めるようにする
		if ( fileList.size() > 0 )
			m_applicationPtr->openModelFile( fileList[0].c_str() );

	}
	void OnModelFileOpen()
	{
		TCHAR szFilePathName[_MAX_PATH] = _T( "" );
		TCHAR szFilters[] = _T( "Model Files (*.rcd)\0*.txt;*.rcd\0\0" );

		OPENFILENAME ofn = { 0 };
		if ( Win32xx::GetWinVersion() >= 2500 )
		{
			// For Win2000 and above
			ofn.lStructSize = sizeof( OPENFILENAME );
		}
		else
		{
			// For older operating systems
			ofn.lStructSize = OPENFILENAME_SIZE_VERSION_400;
		}

		ofn.hwndOwner = m_view.GetHwnd();
		ofn.lpstrFilter = szFilters;
		ofn.lpstrFile = szFilePathName;
		ofn.nMaxFile = _MAX_PATH;
		ofn.lpstrTitle = _T( "Open Model File" );
		ofn.Flags = OFN_FILEMUSTEXIST;

		// Bring up the dialog, and open the file
		if ( !::GetOpenFileName( &ofn ) )
			return;

		m_applicationPtr->openModelFile( szFilePathName );
		//m_View.getApplication().openModelFile(szFilePathName);

	}
	void OnSubModelFileOpen()
	{
		TCHAR szFilePathName[_MAX_PATH] = _T( "" );
		TCHAR szFilters[] = _T( "Model Files (*.rcd)\0*.txt;*.rcd\0\0" );

		OPENFILENAME ofn = { 0 };
		if ( Win32xx::GetWinVersion() >= 2500 )
		{
			// For Win2000 and above
			ofn.lStructSize = sizeof( OPENFILENAME );
		}
		else
		{
			// For older operating systems
			ofn.lStructSize = OPENFILENAME_SIZE_VERSION_400;
		}

		ofn.hwndOwner = m_view.GetHwnd();
		ofn.lpstrFilter = szFilters;
		ofn.lpstrFile = szFilePathName;
		ofn.nMaxFile = _MAX_PATH;
		ofn.lpstrTitle = _T( "Open SubModel File" );
		ofn.Flags = OFN_FILEMUSTEXIST;

		// Bring up the dialog, and open the file
		if ( !::GetOpenFileName( &ofn ) )
			return;

		m_applicationPtr->openSubModelFile( szFilePathName );
	}

	void OnLandFileOpen()
	{
		TCHAR szFilePathName[_MAX_PATH] = _T( "" );
		TCHAR szFilters[] = _T( "Land Files (*.x)\0*.x;\0\0" );
		OPENFILENAME ofn =
		{ 0 };
		if ( Win32xx::GetWinVersion() >= 2500 )
		{
			// For Win2000 and above
			ofn.lStructSize = sizeof( OPENFILENAME );
		}
		else
		{
			// For older operating systems
			ofn.lStructSize = OPENFILENAME_SIZE_VERSION_400;
		}

		ofn.hwndOwner = m_view.GetHwnd();
		ofn.lpstrFilter = szFilters;
		ofn.lpstrFile = szFilePathName;
		ofn.nMaxFile = _MAX_PATH;
		ofn.lpstrTitle = _T( "Open Land File" );
		ofn.Flags = OFN_FILEMUSTEXIST;

		// Bring up the dialog, and open the file
		if ( !::GetOpenFileName( &ofn ) )
			return;

		m_applicationPtr->openLandFile( szFilePathName );
		//m_View.getApplication().openLandFile(szFilePathName);

	}


};

#endif // AKASHA_FRAMES__H
