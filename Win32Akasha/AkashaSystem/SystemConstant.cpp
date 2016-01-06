#include "SystemConstant.hpp"

#include "../BuildConfig.hpp"
#include <boost/make_shared.hpp>
#include <iostream>

using akasha::system::SystemConstant;

namespace ct
{
struct MajorVersion
{
	typedef int value_type;
};

struct MinorVersion
{
	typedef int value_type;
};

struct Revision
{
	typedef int value_type;
};

struct AbbCodeName
{
	using value_type = char const*;
};

struct CodeName
{
	using value_type = char const*;
};

struct CurrentDirectoryPath
{
	typedef boost::filesystem::path value_type;
};
struct PropertyTree
{
	typedef boost::property_tree::ptree value_type;
};

}

template<typename CT>
struct mPair
{
	typedef boost::fusion::pair<CT, typename CT::value_type> type;
};
struct SystemConstant::impl
{
	typedef boost::fusion::map<
		typename mPair<ct::MajorVersion>::type,
					typename mPair<ct::MinorVersion>::type,
					typename mPair<ct::Revision>::type,
					typename mPair<ct::AbbCodeName>::type,
					typename mPair<ct::CodeName>::type,
					typename mPair<ct::CurrentDirectoryPath>::type,
					typename mPair<ct::PropertyTree>::type> map_type;
	map_type constMap_;
};



SystemConstant::SystemConstant()
{
	m_impl = boost::make_shared< impl>();

	using boost::fusion::at_key;
	at_key<ct::MajorVersion>(m_impl->constMap_) = AKASHA_VERSION_MAJOR;
	at_key<ct::MinorVersion>(m_impl->constMap_) = AKASHA_VERSION_MINER;
	at_key<ct::Revision>(m_impl->constMap_) = AKASHA_VERSION_HOTFIX;
	at_key<ct::CurrentDirectoryPath>(m_impl->constMap_) =
		boost::filesystem::complete(boost::filesystem::current_path());

	load();
}

void SystemConstant::load()
{
	using boost::fusion::at_key;

	namespace pt = boost::property_tree;
	pt::ptree t;
	if (!xmlVerify(t))
		return;

	//バージョン確認
	bool version = false;
	if (auto major = t.get_optional<int>("MIZUNUKI.MajorVersion")) {
		if (auto minor = t.get_optional<int>(
					"MIZUNUKI.MinorVersion")) {
			if (auto revision = t.get_optional<int>(
						"MIZUNUKI.Revision")) {
				if (
						major!= at_key<ct::MajorVersion>(m_impl->constMap_) ||
						minor!= at_key<ct::MinorVersion>( m_impl->constMap_) ||
						revision != at_key<ct::Revision>(m_impl->constMap_)
					)
					{
						std::cout << "バージョン不一致" << std::endl;
					}
				else
					{
						version = true;
						at_key<ct::AbbCodeName>(m_impl->constMap_) =
							t.get<std::string>("MIZUNUKI.AbbCodeName").c_str();
						at_key<ct::CodeName>(m_impl->constMap_) =
							t.get<std::string>("MIZUNUKI.CodeName").c_str();
					}

			}
		}
	}

	//TODO :: 例外出して落とす?
	if (!version)
		std::cout << "バージョン情報が不正です" << std::endl;

	//保存
	at_key<ct::PropertyTree>(m_impl->constMap_) = t;

}
bool SystemConstant::xmlVerify(boost::property_tree::ptree& t) const
{
	namespace pt = boost::property_tree;
	namespace fs = boost::filesystem;
	using boost::fusion::at_key;
	try {

		pt::read_xml(
				fs::complete( fs::path(SystemConstant::getSettingFileName())).string()
				, t);
	}
	catch (boost::exception& e) {
		std::cout << boost::diagnostic_information(e) << std::endl;  // 表示
		return false;
	}
	return true;
}


int SystemConstant::getMajorVersion() const
{
	return boost::fusion::at_key<ct::MajorVersion>(m_impl->constMap_);
}
int SystemConstant::getMinorVersion() const
{
	return boost::fusion::at_key<ct::MinorVersion>(m_impl->constMap_);
}
int SystemConstant::getRevision() const
{
	return boost::fusion::at_key<ct::Revision>(m_impl->constMap_);
}
char const* SystemConstant::getAbbCodeName() const
{
	return boost::fusion::at_key<ct::AbbCodeName>(m_impl->constMap_);
}
char const* SystemConstant::getCodeName() const
{
	return boost::fusion::at_key<ct::CodeName>(m_impl->constMap_);
}
boost::filesystem::path const& SystemConstant::getCurrentDirectoryPath() const
{
	return boost::fusion::at_key<ct::CurrentDirectoryPath>(m_impl->constMap_);
}
boost::property_tree::ptree const& SystemConstant::getPropertyTree() const
{
	return boost::fusion::at_key<ct::PropertyTree>(m_impl->constMap_);
}
