/*
 * DebugWindow.hpp
 *
 *  Created on: 2012/01/12
 *      Author: ely
 */

#ifndef DEBUGWINDOW_HPP_
#define DEBUGWINDOW_HPP_

#include "../../AkashaApplication/AkashaApplication.h"
#include "../res/resource.h"
#include <WinSock2.h>//winsock.hを読ませないために
#include <wincore.h>
#include <stdcontrols.h>


//boost
#include <boost/function.hpp>

//Bullet
#include <LinearMath/btIDebugDraw.h>


namespace akasha
{
namespace win32
{
class FunctorButton : public Win32xx::CButton
	{
		LPTSTR text_;
		Win32xx::CRect rect_;
	public:
		boost::function<void (bool)> handler_;

	public:
		FunctorButton(LPTSTR t, const Win32xx::CRect& r) : text_(t), rect_(r){ }

	protected:
		void PreCreate(CREATESTRUCT& cs)
		{
			cs.style = WS_CHILD | WS_VISIBLE | BS_LEFTTEXT | BS_AUTOCHECKBOX;

			cs.cx = rect_.Width(); cs.cy= rect_.Height();
			cs.x = rect_.left; cs.y = rect_.top;

			CButton::PreCreate(cs);
		}

		void OnCreate()
		{
			SetWindowText(text_);
		}

	public:
		void OnAction()
		{
			if( !handler_.empty())
				handler_(BST_CHECKED==GetCheck());
		}
	};

class DebugWindow : public Win32xx::CWnd
	{

		boost::shared_ptr<akasha::AkashaApplication> m_appPtr;
		FunctorButton m_button1;
		FunctorButton m_button2;
		FunctorButton m_button3;

	public:
		DebugWindow() :
			m_button1("DebugDraw", CRect(10,10, 100,30)),
			m_button2("AABBDraw", CRect(10, 40, 100, 70)),
			m_button3("ExternForce", CRect(10, 70, 100, 100))
		{

		}

		//アプリケーションポインタセット
		void setApplication(boost::shared_ptr<akasha::AkashaApplication> p)
		{
			m_appPtr = p;
		}

		//クリエイト :　アプリケーションが登録されてなければ作られない
		HWND Create(CWnd* pParent = NULL)
		{
			if (m_appPtr)
				return CWnd::Create(pParent);
			else
			{
				return NULL;
			}
		}

		void PreCreate(CREATESTRUCT& cs)
		{
			cs.cx = 300;
			cs.cy = 300;
			CWnd::PreCreate(cs);
		}

		void OnCreate()
		{

			m_button1.Create(this);
			m_button1.handler_ = boost::bind(&akasha::AkashaApplication::toggleShowDebug, m_appPtr );

			m_button2.Create(this);
			m_button2.handler_ = boost::bind(&DebugWindow::toggleDebugFlgHelper, this, btIDebugDraw::DebugDrawModes::DBG_DrawAabb);

			m_button3.Create(this);
			m_button3.handler_ = boost::bind(&akasha::AkashaApplication::toggleShowExternForce, m_appPtr);

		}
		LRESULT
			WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
				switch (uMsg)
				{
				case WM_DESTROY:
					break;
				}

				return WndProcDefault(uMsg, wParam, lParam);
			}

		BOOL OnCommand(WPARAM wParam, LPARAM lParam)
		{
			if ((LPARAM)(m_button1.GetHwnd()) == lParam)
			{
				m_button1.OnAction();
				return TRUE;
			}
			if ((LPARAM)(m_button2.GetHwnd())== lParam)
			{
				m_button2.OnAction();
				return TRUE;
			}
			if ((LPARAM)(m_button3.GetHwnd())==lParam)
			{
				m_button3.OnAction();
				return TRUE;
			}
			return FALSE;
		}

		void toggleDebugFlgHelper(int flg)
		{
			const int nowFlg = m_appPtr->getBulletDebugDrawFlag();
			if (nowFlg & flg)
				m_appPtr->setBulletDebugDrawFlag(nowFlg & ~flg);
			else
				m_appPtr->setBulletDebugDrawFlag(nowFlg | flg);
		}
	};
}
}


#endif /* DEBUGWINDOW_HPP_ */
