#ifndef FRONTEND_HPP_INCLUDED
#define FRONTEND_HPP_INCLUDED

#include <foreplay/foreplay.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/atomic.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/unordered_map.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/find.hpp>

#include <boost/range/algorithm/find_if.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <boost/uuid/uuid_serialize.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/date_time/posix_time/time_serialize.hpp>

#include <boost/format.hpp>

#include <algorithm>

namespace akasha{
namespace net{
//Connection時
struct ConnectionInfo
{
	std::wstring name_;
};

//Foreplayへ登録するUuidインデックス
struct PlayerDataIndex
{
	boost::uuids::uuid startTimeId_;
	boost::uuids::uuid name_;
	boost::uuids::uuid chatIndex_;

private:
	friend class boost::serialization::access;
	template<typename ArchiveType>
		void serialize(ArchiveType& ar, unsigned int)
		{
			ar & startTimeId_;
			ar & name_;
			ar & chatIndex_;
		}
};
// FrontEnd用
struct ChatData
{
	std::wstring message_;

private:
	friend class boost::serialization::access;
	template<typename ArchiveType>
		void serialize( ArchiveType& ar, unsigned int)
		{
			ar & message_;
		}
};
struct PlayerData
{
	PlayerDataIndex tableIndex_;

	boost::posix_time::ptime startTime_;
	std::wstring name_;
	std::vector< boost::uuids::uuid > chatIndex_;
	boost::unordered_map<boost::uuids::uuid, ChatData> chat_;

	bool forceCheckout( boost::uuids::uuid const baseId, foreplay::ForeplayClient& client)
	{
		boost::uuids::uuid result;
		result = client.checkoutDispatch(
				baseId,
				tableIndex_);
		std::cout << boost::format("forece chehout:\nresult%1%\nnameID:%2% ")
			% result % tableIndex_.name_ << std::endl;

		client.checkoutDispatch(
				tableIndex_.startTimeId_, startTime_);
		client.checkoutDispatch(
				tableIndex_.name_, name_);
		result = client.checkoutDispatch(
				tableIndex_.chatIndex_, chatIndex_);
		if ( !result.is_nil() )
			updateChat( client );

		return true;
	}
	bool updateIndex( foreplay::ForeplayClient& client, boost::uuids::uuid const id)
	{
		bool const is_nil(client.checkoutDispatch(id, this->tableIndex_ ).is_nil() );
		return is_nil ? false : forceCheckout( id, client );
	}
	bool update( foreplay::ForeplayClient& client, boost::uuids::uuid const id)
	{
		if (tableIndex_.startTimeId_ == id)
			return !( client.checkoutDispatch( id, this->startTime_).is_nil() );
		if (tableIndex_.name_ == id)
			return !( client.checkoutDispatch( id, this->name_ ).is_nil() );

		if (tableIndex_.chatIndex_ == id)
			return updateChat( client );

		return false;
	}

	bool updateChat( foreplay::ForeplayClient& client)
	{
		std::vector<boost::uuids::uuid> newChatIndex;
		newChatIndex.reserve( chatIndex_.size() );

		if ( client.checkoutDispatch( tableIndex_.chatIndex_, newChatIndex).is_nil() )
			return false;

		//比較してremove範囲を取得
		auto endIter = chatIndex_.end();
		for (boost::uuids::uuid const& id : newChatIndex)
			endIter = std::remove(chatIndex_.begin(), endIter, id);

		//結果イテレータからChatDataを削除
		std::for_each(endIter, chatIndex_.end(),
				[this](boost::uuids::uuid const& id){
				chat_.erase(id);
				});
		//erase
		chatIndex_.erase(endIter, chatIndex_.end());

		//新規リストをあぶり出し
		endIter = newChatIndex.end();
		for (boost::uuids::uuid const& id : chatIndex_)
			endIter = std::remove(newChatIndex.begin(), endIter, id);
		chatIndex_.insert(
				chatIndex_.end(), newChatIndex.begin(), endIter);

		//新規範囲を取得
		size_t insertLength = std::distance(
				newChatIndex.begin(), endIter);
		ChatData bufD;
		newChatIndex.clear();
		std::for_each(chatIndex_.end()-insertLength, chatIndex_.end(),
				[this, &client, &bufD, &newChatIndex](boost::uuids::uuid const& id){
				if ( !(client.checkoutDispatch(id, bufD).is_nil() ) )
				chat_[ id ] = bufD;
				else
				newChatIndex.push_back( id );
				});

		//取得出来なかった範囲をリストから削除
		endIter = chatIndex_.end();
		for (boost::uuids::uuid const& id: newChatIndex)
			endIter = std::remove(chatIndex_.begin(),endIter, id);
		chatIndex_.erase(endIter, chatIndex_.end());

		//TODO : ソート処理?


		return true;
	}
};


class NetworkFrontEnd : public boost::enable_shared_from_this< NetworkFrontEnd >
{
	typedef std::vector< boost::uuids::uuid > chat_index_type;

	ConnectionInfo m_info;

	boost::atomic<bool> m_connectStatus;
	boost::uuids::uuid m_baseId;
	PlayerData m_playerData;
	boost::unordered_map<boost::uuids::uuid, PlayerData> m_otherPlayers;

	std::vector< ChatData > m_sendChatBuffer;
	chat_index_type m_chatUuidIndex;

	boost::lockfree::spsc_queue<foreplay::PlayerUpdate> m_updatePlayerBuffer;
	boost::lockfree::spsc_queue<std::pair<boost::uuids::uuid, boost::uuids::uuid> > m_updateTableId;

	boost::shared_ptr< boost::function<void ()> > m_player_and_chat_updateHandler;

public:
	NetworkFrontEnd() :
		m_connectStatus(false),
		m_updatePlayerBuffer(16),
		m_updateTableId(1024)
	{
	}
	///////////////////////////////////////////////////////////
	// Un Thread Safe
	void setInfo( ConnectionInfo const& i)
	{
		m_info = i;
	}
	ConnectionInfo const& getInfo() const
	{
		return m_info;
	}
	///////////////////////////////////////////////////////////
	//Server側メソッド
	void setStatus(bool s)
	{
		m_connectStatus = s;
	}
	void onNetworkConnect( foreplay::ForeplayClient& client, foreplay::PlayerUpdate const& update)
	{
		auto self = shared_from_this();
		client.setPlayerUpdateHandler(
				typename foreplay::ForeplayClient::PlayerUpdateHandlerType(
					[this, self]( foreplay::PlayerUpdate const& d)
					{
					while( !(this->m_updatePlayerBuffer.push( d )) ){ }
					}));
		client.setDispatchHandler(
				typename foreplay::ForeplayClient::UpdateHandlerType(
					[ this, self ]( boost::uuids::uuid id,boost::uuids::uuid owner){
					while ( !(this->m_updateTableId.push( {owner, id} )) ){ }
					}));

		boost::uuids::uuid baseId = *(client.getSessionUuid());
		m_baseId = baseId;

		applyPlayerData( update, client );

		PlayerDataIndex& index = m_playerData.tableIndex_;

		index.name_ = client.commitDispatch( m_info.name_ );
		index.chatIndex_ = client.commitDispatch( chat_index_type() );

		client.registDispatch( baseId, index );
	}
	//////////////////////////////////////////////////////////
	//Thread Safe メソッド
	bool hasConnect() const
	{
		return m_connectStatus;
	}
	//////////////////////////////////////////////////////////
	//Client側メソッド
	void update( foreplay::ForeplayClient& client )
	{
		if (!hasConnect() ) return;

		//送信処理
		if (m_sendChatBuffer.size() != 0)
		{
			boost::uuids::uuid id;
			for ( ChatData const& d : m_sendChatBuffer )
			{
				id = client.commitDispatch( d );
				m_chatUuidIndex.push_back( id );
			}
			client.registDispatch( m_playerData.tableIndex_.chatIndex_, m_chatUuidIndex);
			m_sendChatBuffer.clear();
		}

		//受信処理
		foreplay::PlayerUpdate bufUpdate;
		while ( m_updatePlayerBuffer.pop( bufUpdate ) )
			applyPlayerData( bufUpdate, client );

		m_updateTableId.consume_all(
				[this, &client]( std::pair<boost::uuids::uuid, boost::uuids::uuid> const& d){
				this->updateDispatchData( d.first, d.second, client);
				});
		/*
			std::pair<boost::uuids::uuid, boost::uuids::uuid> bufId;
			while ( m_updateTableId.pop( bufId ) )
			{
			updateDispatchData( bufId.first, bufId.second, client );
			}*/
	}
	void setPlayerUpdateHandler( boost::shared_ptr<boost::function<void ()>> h)
	{
		m_player_and_chat_updateHandler = h;
	}
	unsigned int getPlayerCount() const
	{
		return m_otherPlayers.size() + 1;
	}
	PlayerData const& getMine() const
	{
		return m_playerData;
	}
	void getPlayersName( std::vector<std::wstring>& result) const
	{
		result.push_back( m_playerData.name_ );
		for (auto const& d : m_otherPlayers | boost::adaptors::map_values)
			result.push_back( d.name_ );
	}
	void sendChat(std::wstring const& chat)
	{
		m_sendChatBuffer.push_back( {chat} );
	}
	bool chatPop(std::vector<ChatData>& result)
	{
		for (auto const& c : m_playerData.chat_ | boost::adaptors::map_values)
			result.push_back(c);

		for (auto const& p : m_otherPlayers | boost::adaptors::map_values)
			for (auto const& c: p.chat_ | boost::adaptors::map_values)
				result.push_back( c );

		return true;
	}
private:
	void applyPlayerData( foreplay::PlayerUpdate const& updata,
			foreplay::ForeplayClient& client)
	{
		//assert( hasConnect() );
		assert( m_baseId == updata.mineId_ );


		{ //RemoveID処理
			std::vector< boost::uuids::uuid > disconnectBuf;
			for (boost::uuids::uuid const& id : m_otherPlayers | boost::adaptors::map_keys)
			{
				auto iter = boost::find(updata.otherIds_, id);
				if (iter == updata.otherIds_.cend())
					disconnectBuf.push_back( id );
			}
			for (auto const& remove : disconnectBuf){
				m_otherPlayers.erase( remove );
				std::cout << "delete id: " << remove << std::endl;}
		}
		{
			for ( boost::uuids::uuid const& id : updata.otherIds_)
			{
				if (m_otherPlayers.count( id ) > 0)
				{
				}
				else
				{
					m_otherPlayers[ id ] = PlayerData();
					m_otherPlayers[ id ].forceCheckout(id, client );
					std::cout << "new id: " << id << std::endl;
					if (m_player_and_chat_updateHandler)
						(*m_player_and_chat_updateHandler)();
				}
			}
		}
	}
	void updateDispatchData(boost::uuids::uuid const owner, boost::uuids::uuid const id, foreplay::ForeplayClient& client)
	{
		assert( hasConnect() );
		assert( m_baseId == owner || m_otherPlayers.count( owner ) >0);

		bool needUpdate = false;
		if (m_baseId == id)
		{// 自身のIndex
			needUpdate = m_playerData.updateIndex(client, id);
		}
		else if(m_baseId == owner)
		{// 自身のPlayerData
			needUpdate = m_playerData.update(client, id );
		}
		else if(owner==id)
		{// 他のIndex
			assert( m_otherPlayers.count( owner ) > 0);
			needUpdate = m_otherPlayers[ owner ].updateIndex(
					client, id);
		}
		else
		{// 他のPlayerData
			assert( m_otherPlayers.count( owner ) > 0);
			PlayerData& data = m_otherPlayers[ owner ];
			needUpdate = data.update(client, id );

			std::cout << boost::format("owner: %1% \n id: %2%")
				% owner % id;
		}

		if ( needUpdate && m_player_and_chat_updateHandler)
			(*m_player_and_chat_updateHandler)();
	}


};
}
}

#endif // FRONTEND_HPP_INCLUDED
