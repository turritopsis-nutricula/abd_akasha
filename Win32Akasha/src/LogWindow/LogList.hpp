/*
 * LogList.hpp
 *
 *  Created on: 2012/01/06
 *      Author: ely
 */

#ifndef LOGLIST_HPP_
#define LOGLIST_HPP_

#include <WinSock2.h>//Winsock.hを読ませないために
#include <wincore.h>
#include <listview.h>
#include <boost/variant.hpp>

namespace
{


template<typename LogType>
	struct LogVisiter : boost::static_visitor<void>
{
	LogType& list_;
	boost::posix_time::ptime date_;

	LogVisiter(LogType& l): list_(l){ }

	template<typename T>
		void operator()(const T& t ) const
		{
			Do(t);
		}
	template <typename T>
		void Do(const T& t) const
		{
		}
	void Do(const akasha::system::log::DebugInfo& info) const
	{
		list_.addLog(boost::posix_time::to_simple_string(date_), info.message_);
	}
	void Do(const akasha::system::log::ApplicationInfo& info) const
	{
		list_.addLog(boost::posix_time::to_simple_string(date_), info.message_);
	}
	void Do(const akasha::system::log::ApplicationError& info) const
	{
		list_.addLog(boost::posix_time::to_simple_string(date_), info.message_);
	}
	void Do(const akasha::system::log::ModelFileSyntaxError& error) const
	{
		list_.addLog(boost::posix_time::to_simple_string(date_), error.message_);
	}
	void Do(const akasha::system::log::ModelFileWarning& warn) const
	{
		list_.addLog(boost::posix_time::to_simple_string(date_),warn.message_);
	}
};


}
namespace akasha
{
namespace win32
{
class LogList : public Win32xx::CListView
	{



	public:
		~LogList()
		{
		}

	protected:
		void
			PreCreate(CREATESTRUCT &cs)
			{
				cs.style = WS_TABSTOP | WS_CHILD | WS_VISIBLE | LVS_AUTOARRANGE
					| LVS_REPORT | LVS_SHAREIMAGELISTS | LVS_SHOWSELALWAYS;
				cs.dwExStyle = WS_EX_CLIENTEDGE;
				CListView::PreCreate(cs);
			}

		void
			OnCreate()
			{
				InsertColumn(0, "date", 0, 150, 0);
				InsertColumn(1, "body", 0, 400, 1);


			}
		//TODO: ウィンドウ隠す処理とかでどうするか

	public:
		//アップデートハンドル
		void logUpdateNotify(int logCount)
		{

			typedef akasha::system::AkashaLogger::logType log_type;
			LogVisiter<LogList> visiter(*this);

			const std::vector<log_type>& logs = akasha::system::AkashaSystem::Instance().getLogger().getLog();

			for(int i=logCount-1; i>=0; i--)
				//for(int i=0; i<logCount;i++)
			{
				const log_type& log = logs[logs.size()-i-1];
				visiter.date_ = log.time_;
				boost::apply_visitor( visiter, log.body_);
			}
		}

		//visiter向けインターフェイス
		template<typename StringType>
			void addLog(const StringType& date, const StringType& body)
			{
				int count = GetItemCount();
				InsertItem(count, date.c_str());
				SetItemText(count,1, body.c_str());

				//        Update(0);
			}
	};

}
}

#endif /* LOGLIST_HPP_ */
