/*
 * LogWindow.hpp
 *
 *  Created on: 2012/01/06
 *      Author: ely
 */

#ifndef LOGWINDOW_HPP_
#define LOGWINDOW_HPP_

#include "LogList.hpp"
#include <WinSock2.h>//Winsock.h��ǂ܂��Ȃ����߂�
#include <wincore.h>



#include <boost/signals2/connection.hpp>
#include <boost/bind.hpp>

namespace akasha
{
namespace win32
{

//TODO : ���O�ʒm�����ł�������Create����
/*���� : ��xOnCreate���Ă΂�Ȃ����胍�O�ʒm��Create����Ȃ��B
 * ��x�����ƈȌ�͂��Ȃ炸Create�����
 * */
class LogViewFrame : public Win32xx::CWnd
	{
		LogList m_list;
		boost::signals2::connection m_loggerConnection;
		commandReciverType& m_reciver;
		akasha::system::AkashaLogger& m_logger;
	public:
		LogViewFrame(commandReciverType& c) : m_reciver(c),m_logger(akasha::system::AkashaSystem::Instance().getLogger())
		{
			//���K�[�ɃV�O�i���o�^
			m_loggerConnection = m_logger.updateConnect(
					boost::bind(&LogViewFrame::LogUpdate, this, ::_1)
					);
		}

		void PreCreate(CREATESTRUCT& cs)
		{
			cs.cx = 400;
			cs.cy = 300;
			CWnd::PreCreate(cs);
		}
		void
			OnCreate()
			{
				m_list.Create(this);
				m_list.logUpdateNotify(m_logger.getLog().size());

			}

		//! ���O�̃A�b�v�f�[�g�ʒm
		void LogUpdate(int logCount)
		{
			if(!this->IsWindow())
				Create();
			m_list.logUpdateNotify(logCount);
		}



		LRESULT
			WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
				switch (uMsg)
				{
				case WM_SIZE:
					m_list.MoveWindow(GetClientRect(), TRUE);
					break;
				case WM_DESTROY:
					//	m_loggerConnection.disconnect(); //�f�B�X�R�l�N�g
					break;
				}

				return WndProcDefault(uMsg, wParam, lParam);
			}

		BOOL
			OnCommand(WPARAM wp, LPARAM lp)
			{
				return m_reciver(*this, wp, lp);
			}

	};
}
}

#endif /* LOGWINDOW_HPP_ */
