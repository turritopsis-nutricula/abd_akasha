/*
 * AkashaSystem.hpp
 *
 *  Created on: 2012/01/05
 *      Author: ely
 */

#ifndef AKASHASYSTEM_HPP_
#define AKASHASYSTEM_HPP_

#include "AkashaLogger.hpp"
#include "SystemConstant.hpp"
#include "InputInterface.hpp"
#include <Riceball/singleton/SingletonHolder.hpp>

#include <boost/noncopyable.hpp>


namespace akasha
{
namespace system
{
namespace detail
{
class AkashaSystemImpl : boost::noncopyable
	{
		AkashaLogger logger_;
		SystemConstant constant_;
		ii::VirtualInput virtualInput_;

	public:
		explicit AkashaSystemImpl(){ };
		~AkashaSystemImpl(){
		}

		//ÉçÉKÅ[
		AkashaLogger& getLogger()
		{
			return logger_;
		}

		//âºëzì¸óÕ
		ii::VirtualInput& getInput()
		{
			return virtualInput_;
		}
		const ii::VirtualInput& getInput() const
		{
			return virtualInput_;
		}

		//íËêî
		SystemConstant& getConstant()
		{
			return constant_;
		}
		const SystemConstant& getConstant() const
		{
			return constant_;
		}


	};
}

using AkashaSystem = akasha::singleton::SingletonHolder<detail::AkashaSystemImpl>;

inline boost::optional<boost::property_tree::ptree const&> GetSettingTree(
		std::string const& path)
{
	return
		AkashaSystem::Instance().getConstant().getPropertyTree().get_child_optional(path);
}
template<typename T>
inline boost::optional<T> GetSetting(
		std::string const& path)
{
	return
		AkashaSystem::Instance().getConstant().
		getPropertyTree().get_optional<T>(path);
}

inline boost::filesystem::path const&
GetCurrentDirectoryPath()
{
	return AkashaSystem::Instance().getConstant().getCurrentDirectoryPath();
}

template<typename IrrDev>
inline void SetCurrentDirectory(IrrDev* irr)
{
	std::string path(GetCurrentDirectoryPath().string());
	irr->getFileSystem()->changeWorkingDirectoryTo( path.c_str());
}

}
}

extern template class akasha::singleton::SingletonHolder<
akasha::system::detail::AkashaSystemImpl>;
#endif /* AKASHASYSTEM_HPP_ */
