#ifndef CHATLISTVIEW_HPP_INCLUDED
#define CHATLISTVIEW_HPP_INCLUDED

#include "../../AkashaApplication/net/FrontEnd.hpp"

#include <WinSock2.h> //Winsock.hを読ませない
#include <wincore.h>
#include <listview.h>

namespace akasha
{
namespace win32
{
class ChatListView : public Win32xx::CListView
	{
		boost::shared_ptr<net::NetworkFrontEnd> m_frontend;

	protected:
		void  OnCreate()
		{
		}
	public:
		void setFrontEnd( boost::shared_ptr<net::NetworkFrontEnd> e)
		{
			m_frontend = e;
		}
		void updateChatList()
		{
			std::vector< net::ChatData > dataBuffer;
			m_frontend->chatPop( dataBuffer );

			clearAll();
			for ( net::ChatData const& d : dataBuffer)
				Win32xx::CListView::InsertItem(
						Win32xx::CListView::GetItemCount(),
						Win32xx::tString(
							d.message_.cbegin(),d.message_.cend()).c_str());

		}
		void clearAll()
		{
			Win32xx::CListView::DeleteAllItems();
		}
	};
}

}

#endif // CHATLISTVIEW_HPP_INCLUDED
