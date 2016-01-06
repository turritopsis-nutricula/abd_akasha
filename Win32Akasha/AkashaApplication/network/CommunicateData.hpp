/*
 * CommunicateData.hpp
 *
 *  Created on: 2012/05/02
 *      Author: ely
 */

#ifndef COMMUNICATEDATA_HPP_
#define COMMUNICATEDATA_HPP_

#include "WorldData.hpp"

#include <boost/uuid/uuid.hpp>
#include <boost/variant.hpp>

#include <boost/date_time/posix_time/time_serialize.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/variant.hpp>

#include <boost/chrono.hpp>

namespace akasha
{
namespace network
{
typedef std::string StringType;

namespace data
{
struct ChatData
{
	boost::uuids::uuid senderID_;
	boost::uuids::uuid chatID_;
	boost::posix_time::ptime time_;
	StringType chat_;


private:
	friend class boost::serialization::access;

	template<class Archive>
		void
		serialize(Archive& ar, unsigned int ver)
		{
			ar & senderID_;
			ar & chatID_;
			ar & time_;
			ar & chat_;
		}
};

//プレイヤーデータ
struct Player
{
	StringType name_;
	boost::uuids::uuid HostID_;
	boost::uuids::uuid modelID_;
	std::string globalAddressString_;

private:
	friend class boost::serialization::access;

	template<typename Archive>
		void
		serialize(Archive& ar, unsigned int ver)
		{
			ar & name_;
			ar & HostID_;
			ar & modelID_;
			ar & globalAddressString_;
		}
};

//サーバーステータス
struct ServerState
{

	std::vector<Player> players_;
	std::vector<ChatData> chats_;
	boost::posix_time::ptime lastChatTime_;

private:
	friend class boost::serialization::access;
	template<typename Archive>
		void
		serialize(Archive& ar, unsigned int ver)
		{
			ar & players_;
			ar & chats_;
			ar & lastChatTime_;
		}
};

}

namespace detail
{
struct emptySerialize
{

protected:
	friend class boost::serialization::access;
	template<class Archive>
		void
		serialize(Archive& ar, unsigned int ver)
		{
		}

};
}
namespace server
{

namespace notify
{
//変更通知
struct ServerNotify
{
	std::vector<boost::uuids::uuid> playerIDList_;
	boost::posix_time::ptime lastChat_;

private:
	friend class boost::serialization::access;
	template<typename Archive>
		void
		serialize(Archive& ar, unsigned int ver)
		{
			ar & playerIDList_;
			ar & lastChat_;
		}
};
}
namespace reply
{
//OK
struct RequestOK : detail::emptySerialize
	{
	};
//Failed
struct RequestFailed : detail::emptySerialize
	{
	};

//Pingリプライ
struct PingReply
{
	boost::posix_time::ptime clientSendTime_;
private:
	friend class boost::serialization::access;
	template<typename Archive>
		void
		serialize(Archive& ar, unsigned int ver)
		{
			ar & clientSendTime_;
		}
};

struct SendServerStatus
{
};
//チャットデータ
struct ChatLog
{
	typedef std::vector<data::ChatData> data_type;
	data_type chat_;

private:
	friend class boost::serialization::access;

	template<typename Archive>
		void
		serialize(Archive& ar, unsigned int ver)
		{
			ar & chat_;
		}
};

//プレイヤーデータ
struct ReplyPlayerData
{
	data::Player player_;

private:
	friend class boost::serialization::access;

	template<typename Archive>
		void
		serialize(Archive& ar, unsigned int ver)
		{
			ar & player_;
		}
};

//モデル基礎データ
struct ReplyModelData
{
	OtherModelBase<std::string> data_; //TODO:とりあえずstring

private:
	friend class boost::serialization::access;
	template<typename Archive>
		void
		serialize(Archive& ar, unsigned int const ver)
		{
			ar & data_;
		}
};

///////////////////////////////////////////////////////////////////
// データ型
struct ReplyData
{
	typedef boost::variant<RequestOK, RequestFailed, PingReply, ChatLog, ReplyPlayerData,ReplyModelData> reply_type;
	boost::uuids::uuid replyID_;
	reply_type data_;

private:
	friend class boost::serialization::access;
	template<typename Archive>
		void
		serialize(Archive& ar, unsigned int ver)
		{
			ar & replyID_;
			ar & data_;
		}
};

}

typedef boost::variant<notify::ServerNotify, reply::ReplyData> ServerSendData;

/*
	struct ServerSendData
	{
	typedef boost::variant<
	notify::ServerNotify,
	reply::ReplyData> send_type;
	send_type data_;

	private:
	friend class boost::serialization::access;
	template<typename Archive>
	void
	serialize(Archive& ar, unsigned int var)
	{
	ar & data_;
	}
	};*/
}

namespace client
{
namespace request
{

struct SessionEnter
{
	StringType name_;
	boost::uuids::uuid HostID_;
	std::string localAddressString_;

private:
	friend class boost::serialization::access;
	template<typename Archive>
		void
		serialize(Archive& ar, unsigned int ver)
		{
			ar & name_;
			ar & HostID_;
			ar & localAddressString_;
		}
};

struct SessionLeave : detail::emptySerialize
{
};

struct Ping
{
	//boost::chrono::steady_clock::time_point sendTime_;
	boost::posix_time::ptime sendTime_;

private:
	friend class boost::serialization::access;
	template<typename Archive>
		void
		serialize(Archive& ar, unsigned int ver)
		{
			ar & sendTime_;
		}

};

struct SendChat
{
	data::ChatData chat_;

private:
	friend class boost::serialization::access;
	template<typename Archive>
		void
		serialize(Archive& ar, unsigned int ver)
		{
			ar & chat_;
		}
};


struct GetChatLog
{
	boost::posix_time::ptime borderTime_;
private:
	friend class boost::serialization::access;
	template<typename Archive>
		void
		serialize(Archive& ar, unsigned int ver)
		{
			ar & borderTime_;
		}
};

struct GetPlayerData
{
	boost::uuids::uuid targetID_;
private:
	friend class boost::serialization::access;
	template<typename Archive>
		void
		serialize(Archive& ar, unsigned int ver)
		{
			ar & targetID_;
		}
};
struct GetModelData : detail::emptySerialize
{
};

typedef boost::variant<SessionEnter, SessionLeave, Ping,
		  SendChat, GetChatLog, GetPlayerData, GetModelData> Request;

}

//! クライアントのSend
struct ClientSendData
{
	typedef request::Request request_type;
	boost::uuids::uuid id_;
	request_type data_;

private:
	friend class boost::serialization::access;
	template<typename Archive>
		void
		serialize(Archive& ar, unsigned int ver)
		{
			ar & id_;
			ar & data_;
		}
};

}
}
}

#endif /* COMMUNICATEDATA_HPP_ */
