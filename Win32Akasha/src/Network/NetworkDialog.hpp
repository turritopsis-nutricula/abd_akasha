#ifndef NETWORKDIALOG_HPP_INCLUDED
#define NETWORKDIALOG_HPP_INCLUDED

#include "SelectorDialog.hpp"
#include "ChatListView.hpp"

#include "../res/resource.h"
#include "../../AkashaApplication/net/FrontEnd.hpp"

#include <boost/algorithm/string/trim.hpp>

#include <WinSock2.h>
#include <wincore.h>
#include <stdcontrols.h>

#include <set>

namespace akasha
{
namespace win32
{


class NetworkDialog : public Win32xx::CDialog
	{
		boost::shared_ptr<net::NetworkFrontEnd> m_frontend;
		boost::shared_ptr<boost::function<void ()>> m_handler;

		boost::function<void ()> m_closeHandler;
		Win32xx::CListBox m_playerList;
		ChatListView m_chatList;
		Win32xx::CButton m_closeButton;
		Win32xx::CEdit m_chatInputEdit;
		Win32xx::CButton m_chatSendButton;

	public:
		NetworkDialog(CWnd* parent) : CDialog(IDD_NETWORK_CONSOLE,parent){ }

		void setFrontEnd(boost::shared_ptr<net::NetworkFrontEnd> p)
		{
			m_frontend = p;
			m_handler = boost::make_shared< boost::function<void ()> >(
					[this](){
					if ( this->IsWindow())
					this->RedrawWindow();
					});
			m_frontend->setPlayerUpdateHandler(m_handler);
			m_chatList.setFrontEnd( p );
		}
		template<typename FuncType>
			void setCloseHandle(FuncType&& h)
			{
				m_closeHandler = std::move(h);
			}

		BOOL OnInitDialog()
		{
			CDialog::AttachItem( IDC_LIST_MEMBER, m_playerList);
			CDialog::AttachItem( IDC_LIST_CHAT,   m_chatList );
			CDialog::AttachItem( IDC_NETWORK_CLOSE, m_closeButton);
			CDialog::AttachItem( IDC_EDIT_CHAT_INPUT, m_chatInputEdit );
			CDialog::AttachItem( IDC_CHAT_SEND, m_chatSendButton );

			return TRUE;
		}
		BOOL OnCommand( WPARAM wParam, LPARAM lParam )
		{
			switch ( LOWORD( wParam ) )
			{
			case IDC_NETWORK_CLOSE:
				m_closeHandler();
				return TRUE;
			case IDC_CHAT_SEND:
				chatSend();
				return TRUE;
			}

			return FALSE;
		}

		INT_PTR DialogProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
		{
			// Override this function in your class derrived from CWnd to handle
			//  window messages. A typical function might look like this:

			switch ( uMsg )
			{
			case WM_PAINT:
				updateList();
				updateDialog();
				updateChatList();
				break;
			}

			// Always pass unhandled messages on to WndProcDefault
			return DialogProcDefault( uMsg, wParam, lParam );
		}
	private:
		void updateDialog()
		{
			m_closeButton.EnableWindow( m_frontend->hasConnect() );
		}
		void updateList()
		{
			m_playerList.ResetContent();
			std::vector< std::wstring > list;
			m_frontend->getPlayersName( list );


			for (auto const& wstr : list)
			{
				std::wcout << L"name: " << wstr.c_str() << std::endl;
				std::string str( wstr.cbegin(), wstr.cend() );
				m_playerList.AddString( str.c_str());
			}

		}
		void updateChatList()
		{
			m_chatList.updateChatList();

		}
		void chatSend()
		{
			//入力文字取得
			Win32xx::tString str = m_chatInputEdit.GetWindowText().GetString();
			assert( str.size() != 0 );
			str.resize( str.size() - 1); //何故か末尾に一文字付く
			Win32xx::tString const trimStr = boost::algorithm::trim_copy( str );

			std::cout << "'" << trimStr << "'" << std::endl;
			if ( trimStr.size() != 0)
				m_frontend->sendChat( std::wstring(str.cbegin(), str.cend() ) );

			m_chatInputEdit.SetWindowText( "" );
			this->RedrawWindow();
		}

	};

class NetworkConsole
{
	boost::shared_ptr<akasha::AkashaApplication> m_app;
	CWnd* m_parentWnd;
	boost::shared_ptr<NetworkSelectDialog> m_selectDialog;
	boost::shared_ptr<NetworkDialog> m_consoleDialog;

public:

	NetworkConsole( CWnd* parent ) : m_parentWnd( parent )
	{

	}
	void setApplication( boost::shared_ptr<akasha::AkashaApplication> p )
	{
		m_app = p;
	}
	void show()
	{
		if (m_app->getNetworkFrontEnd()->hasConnect())
			showConsole();
		else
			showSelectDialog();
	}
private:
	void showSelectDialog()
	{
		m_selectDialog = boost::make_shared<NetworkSelectDialog>( m_parentWnd );
		m_selectDialog->setNetworkFrontEnd(m_app->getNetworkFrontEnd());
		m_selectDialog->setConnectHandler(
				[this]( NetworkSelectDialog::ConnectData const& d )
				{
				net::ConnectionInfo info;
				info.name_ = std::wstring(
						d.name_.begin(), d.name_.end());
				if (d.isServer_)
				m_app->runNetworkServer( info, d.port_ );
				else
				m_app->runNetworkClient( info, d.address_, d.port_ );

				} );
		m_selectDialog->setConnectedHandler(
				[this](){
				this->showConsole();
				this->m_selectDialog->Destroy();
				});
		m_selectDialog->Create();
	}
	void showConsole()
	{
		m_consoleDialog = boost::make_shared<NetworkDialog>( m_parentWnd );
		m_consoleDialog->setFrontEnd(m_app->getNetworkFrontEnd());
		auto app = m_app;
		m_consoleDialog->setCloseHandle([app](){
				app->closeNetwork();
				});
		m_consoleDialog->Create();
	}
};

}

}
#endif // NETWORKDIALOG_HPP_INCLUDED
