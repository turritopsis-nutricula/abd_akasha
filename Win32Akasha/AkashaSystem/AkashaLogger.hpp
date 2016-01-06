/*
 * AkashaLogger.hpp
 *
 *  Created on: 2012/01/05
 *      Author: ely
 */

#ifndef AKASHALOGGER_HPP_
#define AKASHALOGGER_HPP_

#include <boost/variant.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem/path.hpp>

/*#ifdef BOOST_PARAMETER_MAX_ARITY
#   undef BOOST_PARAMETER_MAX_ARITY
#define BOOST_PARAMETER_MAX_ARITY 10
#endif*/
#include <boost/signals2.hpp>

namespace akasha
{
namespace system
{
namespace log
{
typedef std::string string_type;

//! �e�X�g���O
struct NullNotify
{
};
//! �f�o�b�N
struct DebugInfo
{
	string_type message_;
};
//! �V�X�e���C���t�H
struct ApplicationInfo
{
	string_type message_;
};
//! �V�X�e���G���[
struct ApplicationError
{
	string_type message_;
};
//! ���f���t�@�C�����@�G���[
struct ModelFileSyntaxError
{
	string_type message_;
	boost::iterator_range<typename string_type::iterator> range_;
};
//! ���f���t�@�C�����[�j���O
struct ModelFileWarning
{
	boost::filesystem::path file_path_;
	string_type message_;
};
//! ���f���C���t�H
struct ModelInfo
{
	string_type message_;
};
}

class AkashaLogger
{
public:
	typedef boost::variant<log::NullNotify, log::DebugInfo, log::ApplicationInfo, log::ApplicationError, log::ModelFileSyntaxError,log::ModelFileWarning, log::ModelInfo> logVariant;

	struct logType
	{
		boost::posix_time::ptime time_;
		logVariant body_;

		explicit
			logType(logVariant l) :
				time_(boost::posix_time::second_clock::local_time()), body_(l)
		{
		}
	};

private:

	std::vector<logType> m_buffer;
	boost::signals2::signal<void
		(int)> m_notifySignal;

public:
	//! ���O�ɒǉ�
	template<typename LogBodyType>
		int
		operator()(LogBodyType log)
		{
			m_buffer.push_back(logType
					{ log });
			m_notifySignal(1);
			return m_buffer.size();
		}

	const std::vector<logType>&
		getLog() const
		{
			return m_buffer;
		}


	template<typename T>
		boost::signals2::connection updateConnect(T&& s)
		{
			boost::signals2::connection c = m_notifySignal.connect(s);
			s(m_buffer.size()); //����Ăяo��
			return c;
		}



};
}
}

#endif /* AKASHALOGGER_HPP_ */
