/*
 * SystemConstant.hpp
 *
 *  Created on: 2012/01/23
 *      Author: ely
 */

#ifndef SYSTEMCONSTANT_HPP_
#define SYSTEMCONSTANT_HPP_

///�V�X�e���S�̂̒萔���

#include <stdexcept>

#include <boost/fusion/include/value_at_key.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/include/map.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <boost/exception/get_error_info.hpp>
#include <boost/exception/info.hpp>
#include <boost/exception/diagnostic_information.hpp>

#include <boost/filesystem.hpp>


namespace akasha
{

namespace system
{

typedef boost::error_info<struct tag_errno, int> errno_info;  // int�^�̃G���[���


class SystemConstant : boost::noncopyable
	{
		struct impl;

		boost::shared_ptr<impl> m_impl;

		static constexpr char const* getSettingFileName()
		{
			return "./setting.xml";
		}

	public:
		SystemConstant();

		void load();

	private:
		bool xmlVerify(boost::property_tree::ptree& t) const;


	public:

		int getMajorVersion() const;

		int getMinorVersion() const;

		int getRevision() const;

		char const* getAbbCodeName() const;
		char const* getCodeName() const;

		boost::filesystem::path const& getCurrentDirectoryPath() const;

		boost::property_tree::ptree const& getPropertyTree() const;

	};

}

}

#endif /* SYSTEMCONSTANT_HPP_ */
