#ifndef SELECTORDIALOG_HPP_INCLUDED
#define SELECTORDIALOG_HPP_INCLUDED

#include "../res/resource.h"

#include <boost/uuid/uuid_io.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/optional.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/range/algorithm/sort.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/ref.hpp>
#include <boost/unordered_map.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/identity.hpp>

#include <WinSock2.h>
#include <wincore.h>
#include <control.h>

namespace akasha
{
namespace win32
{
class NetworkSelectDialog : public Win32xx::CDialog
	{
		typedef std::string string_type;

	public:
		struct ConnectData
		{
			bool isServer_;
			string_type address_;
			string_type port_;
			string_type name_;
			string_type color_;
		};

	private:
		typedef boost::function<void ( ConnectData const&)> NetworkSelectHandlerType;
		typedef boost::function<void ()> NetworkConnectedHandlerType;

		struct HistoryData
		{
			bool isServer_;
			string_type address_;
			string_type port_;
			string_type name_;
			string_type color_;
			boost::posix_time::ptime lastConnectedTime_;
		};
		struct HistoryDataHasher
		{// server, address, portのみで一致
			std::size_t operator()(HistoryData const& d) const
			{
				std::size_t h=0;
				boost::hash_combine(h, d.isServer_);
				boost::hash_combine(h, d.address_);
				boost::hash_combine(h, d.port_);
				return h;
			}
		};
		struct HistoryDataEqualer
		{// server, address, portのみで一致
			bool operator()(HistoryData const& l, HistoryData const& r) const
			{
				return
					( (l.isServer_ && r.isServer_) || ( !l.isServer_ && !r.isServer_) ) &&
					l.address_ == r.address_ &&
					l.port_ == r.port_;
			}
		};
		struct unique_tag{};
		struct sort_tag{};

		typedef boost::multi_index_container<
			HistoryData,
			boost::multi_index::indexed_by<
				boost::multi_index::hashed_unique<
				boost::multi_index::tag<unique_tag>,
			boost::multi_index::identity< HistoryData >,
			HistoryDataHasher,
			HistoryDataEqualer
				>,
			boost::multi_index::random_access<
				boost::multi_index::tag<sort_tag>>
				>
				> history_list_type;

		boost::shared_ptr<net::NetworkFrontEnd> m_frontEnd;
		NetworkSelectHandlerType m_connect;
		NetworkConnectedHandlerType m_connected;
		unsigned int m_timerID;
		unsigned int m_timerCount;

		CButton m_serverRadio;
		CButton m_clientRadio;
		CButton m_okButton;
		CButton m_cancelButton;
		CEdit m_adressEdit;
		CEdit m_portEdit;
		CEdit m_nameEdit;
		CComboBox m_historyCombobox;

		boost::filesystem::path m_historyFilePath;
		history_list_type m_history;
		HistoryData m_bufConnecting;
		boost::unordered_map<int, int> m_combobox_to_history;

	public:
		NetworkSelectDialog() = delete;
		NetworkSelectDialog( CWnd* parent ) :
			CDialog( IDD_NETWORK_SELECT, parent ),
			m_historyFilePath( "./Resources/history.xml" )
		{

		}
		void setNetworkFrontEnd(boost::shared_ptr<net::NetworkFrontEnd> n)
		{
			m_frontEnd = n;
		}
		BOOL OnInitDialog()
		{
			// HistoryLoad
			if( !loadHistory() ) saveHistory();

			//Dialog Setup
			SendDlgItemMessage( IDC_RADIO_SERVER, BM_SETCHECK, BST_CHECKED, 0l );
			SendDlgItemMessage( IDC_RADIO_CLIENT, BM_SETCHECK, BST_UNCHECKED, 0l );

			CDialog::AttachItem( IDC_EDIT_ADRESS, m_adressEdit );
			m_adressEdit.EnableWindow( false );
			CDialog::AttachItem( IDC_EDIT_PORT, m_portEdit );
			CDialog::AttachItem( IDC_EDIT_NETWORK_NAME, m_nameEdit );
			CDialog::AttachItem( IDC_RADIO_SERVER, m_serverRadio);
			CDialog::AttachItem( IDC_RADIO_CLIENT, m_clientRadio);
			CDialog::AttachItem( IDOK, m_okButton);
			CDialog::AttachItem( IDCANCEL, m_cancelButton);
			CDialog::AttachItem( IDC_NETWORK_HISTORY, m_historyCombobox);

			{
				auto rect = m_historyCombobox.GetWindowRect();
				m_historyCombobox.SetWindowPos(
						0,
						rect.left,
						rect.top,
						rect.right-rect.left,
						150,
						SWP_NOMOVE | SWP_NOOWNERZORDER);
			}

			refreshHistroySelector(true);
			return TRUE;
		}

		INT_PTR DialogProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
		{
			// Override this function in your class derrived from CWnd to handle
			//  window messages. A typical function might look like this:

			switch ( uMsg )
			{
			case WM_TIMER:
				if (m_frontEnd->hasConnect())
				{
					successConnect();
				}
				else
				{
					if(++m_timerCount > 5)
						timeoutConnect();
					break;
				}
			}

			// Always pass unhandled messages on to WndProcDefault
			return DialogProcDefault( uMsg, wParam, lParam );
		}

		BOOL OnCommand( WPARAM wParam, LPARAM lParam )
		{
			switch ( LOWORD( wParam ) )
			{
			case IDC_RADIO_SERVER:
				selectType( true );
				return TRUE;
			case IDC_RADIO_CLIENT:
				selectType( false );
				return TRUE;
			case IDC_NETWORK_HISTORY:
				if (HIWORD( wParam ) == CBN_SELENDOK )
					applyHistoryItem();
				return TRUE;
			}

			return FALSE;
		}

		void OnOK()
		{
			bool isServer =
				static_cast<int>( SendDlgItemMessage( IDC_RADIO_SERVER, BM_GETCHECK, 0l, 0l ) ) == BST_CHECKED;
			std::string address;
			std::string port;
			std::string name;

			address = m_adressEdit.GetWindowText().c_str();
			port = m_portEdit.GetWindowText().c_str();
			name = m_nameEdit.GetWindowText().c_str();

			if ( ( isServer || address.size() > 2 ) && port.size() > 0 )
			{
				startConnecct( { isServer, address, port, name} );
			}
			else
			{
				std::cout << "plase input address and port.";
			}


		}

		template<typename FuncType>
			void setConnectHandler( FuncType&& f)
			{
				m_connect=std::move(f);
			}
		template<typename FuncType>
			void setConnectedHandler( FuncType && f )
			{
				m_connected = std::move( f );
			}
	private:
		void selectType( bool isServer )
		{
			m_adressEdit.EnableWindow( !isServer );
			refreshHistroySelector( isServer );
		}

		void startTimer()
		{

			m_timerCount = 0;
			m_timerID = this->SetTimer( 1, 1000, nullptr);
			m_adressEdit.EnableWindow( false );
			m_portEdit.EnableWindow( false );
			m_serverRadio.EnableWindow( false );
			m_clientRadio.EnableWindow(false );
			m_okButton.EnableWindow( false );
		}
		void releaseTimer()
		{
			this->KillTimer(m_timerID);
			m_adressEdit.EnableWindow( true );
			m_portEdit.EnableWindow( true );
			m_serverRadio.EnableWindow( true );
			m_clientRadio.EnableWindow( true );
			m_okButton.EnableWindow( true );
		}
		void startConnecct(ConnectData const& d)
		{
			startTimer();
			m_bufConnecting = HistoryData{ d.isServer_, d.address_, d.port_,d.name_ };
			m_connect( d );
		}

		void successConnect()
		{
			releaseTimer();

			m_bufConnecting.lastConnectedTime_
				= boost::posix_time::second_clock::local_time();
			auto result = m_history.insert( m_bufConnecting );
			if ( !result.second )
			{
				m_history.erase( result.first );
				m_history.insert( m_bufConnecting );
			}

			saveHistory();
			m_connected();
		}
		void timeoutConnect()
		{
			releaseTimer();
		}

		// History Save Load
		void saveHistory()
		{
			using boost::property_tree::ptree;
			ptree pt;
			ptree& history_pt = pt.add("network_history.connect", "");
			for (HistoryData const& h : m_history)
			{
				ptree& item = history_pt.add("history", "");
				item.put( "isServer", h.isServer_ );
				item.put("address", h.address_);
				item.put("port",    h.port_);
				item.put("name",    h.name_);
				item.put("lastConnectedTime",
						boost::posix_time::to_iso_string(h.lastConnectedTime_));
			}

			try
			{
				int const indentSize = 2;
				boost::property_tree::xml_parser::write_xml(
						m_historyFilePath.string(), pt,
						std::locale(),
						boost::property_tree::xml_writer_make_settings(
							' ',indentSize,
							boost::property_tree::xml_parser::widen<char>("utf-8")));
			} catch( boost::property_tree::xml_parser_error const& e)
			{
				std::cout << e.what() << std::endl;
			}
		}
		bool loadHistory()
		{
			using boost::property_tree::ptree;
			//Load Network History
			boost::property_tree::ptree pt;
			try {
				boost::property_tree::read_xml( m_historyFilePath.string(), pt);
			} catch( boost::property_tree::xml_parser_error const& e)
			{
				std::cout << e.what();
				return false;
			}

			if (pt.empty()) return false;

			m_history.clear();
			for (boost::property_tree::ptree::value_type const& c :
					pt.get_child( "network_history.connect" ))
			{
				ptree const& h = c.second;
				HistoryData d{
					h.get("isServer", false),
						h.get("address", string_type("")),
						h.get("port", string_type("")),
						h.get("name", string_type("NoBody"))
				};
				d.lastConnectedTime_ =
					boost::posix_time::ptime(boost::posix_time::from_iso_string(
								h.get("lastConnectedTime", std::string("19981010T235959"))));
				m_history.insert( d );
			}
			//sort
			std::vector<boost::reference_wrapper<const HistoryData> > sortBuf;
			for (HistoryData const& d : m_history)
				sortBuf.push_back( boost::cref( d ));
			boost::sort( sortBuf, [](HistoryData const& l, HistoryData const& r){
					return l.lastConnectedTime_ < r.lastConnectedTime_;});
			m_history.get< sort_tag >().rearrange( sortBuf.begin() );

			return true;

		}
		void refreshHistroySelector(bool const isServer)
		{
			while( m_historyCombobox.GetCount()!=0)
				m_historyCombobox.DeleteString(0);

			m_combobox_to_history.clear();

			string_type buf;
			int indexCounter = 0;
			int dataReverceIndex = m_history.get<sort_tag>().size();
			for (HistoryData const& h : m_history.get<sort_tag>() | boost::adaptors::reversed)
			{
				--dataReverceIndex;

				if ( ( h.isServer_ && isServer) || ( !h.isServer_ && !isServer) )
				{
					if (isServer)
						buf =(boost::format("%1% port:%2%")  %
								boost::posix_time::to_simple_string(h.lastConnectedTime_) %
								h.port_).str();
					else
						buf =(boost::format("%1% IP:%2% port:%3%") %
								boost::posix_time::to_simple_string(h.lastConnectedTime_) %
								h.address_ % h.port_).str();

					m_historyCombobox.AddString( buf.c_str() );
					m_combobox_to_history[ indexCounter++ ] = dataReverceIndex;
				}
			}
			m_historyCombobox.RedrawWindow();
		}
		void applyHistoryItem()
		{
			int comboboxIndex = m_historyCombobox.GetCurSel();
			if( m_combobox_to_history.count(comboboxIndex) == 0)
				return;
			HistoryData const& d =
				m_history.get<sort_tag>().at( m_combobox_to_history[comboboxIndex] );
			auto const f =[](string_type const& str, Win32xx::CEdit& edit){
				edit.Clear();
				edit.SetSel(0,-1, FALSE);
				edit.ReplaceSel( str.c_str(), TRUE);
			};
			f( d.port_, m_portEdit);
			f( d.address_, m_adressEdit);
			f( d.name_, m_nameEdit );
		}

	};
}
}

#endif // SELECTORDIALOG_HPP_INCLUDED
