/*
 * AkashaMainWindow.hpp
 *
 *  Created on: 2012/01/07
 *      Author: ely
 */

#ifndef AKASHAMAINWINDOW_HPP_
#define AKASHAMAINWINDOW_HPP_

#include <shellapi.h>

#include "res/resource.h"
#include <WinSock2.h> //winsock.hを読ませないために
#include <wincore.h>
#include <frame.h>

namespace akasha
{
namespace win32
{

//! メインウィンドウ
class Win32Akasha_View : public Win32xx::CWnd
	{

		akasha::win32::WndProcFilterType& m_wndReciver;
		akasha::win32::commandReciverType& m_reciver;


	public:
		Win32Akasha_View(akasha::win32::WndProcFilterType& w,
				akasha::win32::commandReciverType& c) :
			m_wndReciver(w), m_reciver(c)
		{

		}
		~Win32Akasha_View()
		{
		}

		void
			PreCreate(CREATESTRUCT& cs)
			{

				//ウィンドウ幅を明示しないとドライバで死ぬ
				cs.cx = 800;
				cs.cy = 500;
				cs.style = WS_THICKFRAME;

				if (/*!CreationParams.Fullscreen*/true)
					cs.style = cs.style | WS_SYSMENU | WS_BORDER | WS_CAPTION
						| WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

				cs.dwExStyle = cs.dwExStyle | WS_EX_ACCEPTFILES;

				CWnd::PreCreate(cs);
			}

		//! クリエイトフック
		void
			OnCreate()
			{


				//メニュー追加
				Win32xx::CMenu* menu = new Win32xx::CMenu(IDW_MAIN);
				SetMenu(menu);
				/*
					Win32xx::CMenu& menuTop = *(GetMenu());
				//ファイル項
				{
				Win32xx::CMenu fileMenu;
				fileMenu.CreateMenu();
				menuTop.InsertMenu(0,MF_BYPOSITION|MF_STRING|MF_POPUP, (UINT)fileMenu.GetHandle(), _T("&File"));

				fileMenu.AppendMenu(MF_STRING, IDM_FILE_OPEN_MODEL,     _T("&Open Model"));
				fileMenu.AppendMenu(MF_STRING, IDM_FILE_OPEN_LAND,             _T("Open &Land"));
				fileMenu.AppendMenu(MF_STRING, IDM_FILE_EXIT, _T("&Exit"));
				}
				//ウィンドウ項
				{
				Win32xx::CMenu windowMenu;
				windowMenu.CreateMenu();
				menuTop.InsertMenu(1, MF_BYPOSITION|MF_STRING|MF_STRING|MF_POPUP, (UINT)windowMenu.GetHandle(), _T("&Window"));


				//windowMenu.AppendMenu(MF_STRING, IDM_WINDOW_LOG, _T("&Log"));
				//TODO :リリース時には削除されるようにしたい
				windowMenu.AppendMenu(MF_SEPARATOR);
				//            windowMenu.AppendMenu(MF_STRING, IDM_WINDOW_DEBUG, _T("&Debug"));

				windowMenu.AppendMenu(MF_SEPARATOR);
				windowMenu.AppendMenu(MF_STRING, IDM_NETWORK_DIALOG, _T("&Network"));
				}
				*/

				ShowWindow(SW_SHOW);
				SetActiveWindow();
			}


		//!ウィンドメッセージ処理
		virtual LRESULT
			WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
				if (m_wndReciver(*this, uMsg, wParam, lParam))
					return WndProcDefault(uMsg, wParam, lParam);
				else
					return 0l;
			}

		//! コマンド処理
		BOOL OnCommand(WPARAM wParam, LPARAM lParam)
		{
			return m_reciver(*this, wParam, lParam);
		}


	public:
		BOOL
			InsertSubMenu(Win32xx::CMenu& subMenu, Win32xx::CMenu& parent,
					const std::string menuName, UINT uItem, BOOL fByPos/* = FALSE */)
			{ //TODO:UNICODE環境化を考えてない
				MENUITEMINFO info;
				info.fMask = MIIM_TYPE | MIIM_SUBMENU;
				info.fType = MFT_STRING;
				info.dwTypeData = LPSTR(menuName.c_str());

				info.hSubMenu = subMenu.GetHandle();
				return parent.InsertMenuItem(uItem, &info, fByPos);
			}

	};

}
}
#endif /* AKASHAMAINWINDOW_HPP_ */
