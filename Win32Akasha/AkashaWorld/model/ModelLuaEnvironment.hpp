/*
 * ModelLuaEnvironment.hpp
 *
 *  Created on: 2012/03/17
 *      Author: ely
 */

#ifndef MODELLUAENVIRONMENT_HPP_
#define MODELLUAENVIRONMENT_HPP_


#include "ModelValue.hpp"

#include "../environment/EnvironmentLand.hpp"

#include "../../akashaApplication/lua/LuaEnvironment.hpp"

#include "ModelObserver.hpp"
// #include "../convert/ConvertImpl_narowchar_widechar.hpp"

//wisp
#include <wisp_v3/wisp.hpp>
#include <wisp_v3/irrExt.hpp>

//luabind

#include <luabind/tag_function.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/range/algorithm/fill.hpp>


namespace akasha
{
namespace lua
{



class ModelLuaEnvironment
{
	//TODO: 描画フレームスキップした場合OutのFontDrawerが同じ場所二回描画される
	boost::optional<lua::LuaEnvironment> m_lua;
	std::wstring m_errorMessage;

	irrExt::IrrExtention& m_irrEx;
	wisp::v3::WispHandle& m_wisp;

	using inputBufferType =
		boost::array<bool, system::ii::DegitalID::VButton_ModelButtonCount>;

	inputBufferType m_inputDown;
	inputBufferType m_inputUp;
	inputBufferType m_inputBuffer;

	boost::array<std::string, 22> m_outBuffer;

	boost::shared_ptr<land::EnvironmentLand> m_landPtr;

	boost::function<void (bool)> m_simulationContext;

public:
	ModelLuaEnvironment() = delete;
	ModelLuaEnvironment(
			irrExt::IrrExtention& c,
			wisp::v3::WispHandle& wisp,
			boost::shared_ptr<land::EnvironmentLand> l) :
		m_irrEx(c),m_wisp(wisp), m_landPtr(l){ }


	bool setup(
			std::string const& luaCode,
			boost::shared_ptr<model::ModelObserver> const& statusBuffer,
			boost::filesystem::path const& modelFilePath,
			boost::function<void (bool)>&& simulationContext);


	//OnInit Call
	void onInit( model::value::ModelValueBuffer& values);

	//ModelLua OnFrame
	void onFrame(model::value::ModelValueBuffer& values);

	//LuaState Close
	void release()
	{
		if (existLua())
			m_lua = boost::none;
	}
	void drawOutMessage()
	{

		int count(0);
		for(auto& item : m_outBuffer)
		{
			m_irrEx.drawScreenFont(
					irrExt::ScreenFontData{ 12, ++count * 15 + 2, irr::video::SColor(255,0,0,0), item});
			m_irrEx.drawScreenFont(
					irrExt::ScreenFontData{ 10, count * 15, irr::video::SColor(255,255,255,255), item});
			item = "";
		}
	}

	void resetInputStatus()
	{
		boost::fill(m_inputDown,  false);
		boost::fill(m_inputUp,   false);

		const auto& input = system::AkashaSystem::Instance().getInput();
		for(unsigned int i=0; i<system::ii::DegitalID::VButton_ModelButtonCount; i++)
		{
			const bool& now = input.get(i);
			m_inputDown[i] = now && !m_inputBuffer[i];
			m_inputUp[i] = !now && m_inputBuffer[i];
			m_inputBuffer[i] = now;
		}
	}

	lua::LuaEnvironment&
		getLuaEnviroment()
		{
			return *m_lua;
		}

	lua::LuaEnvironment const&
		getLuaEnviroment() const
		{
			return *m_lua;
		}

	bool existLua() const
	{
		return m_lua.is_initialized();
	}

	/////////////////////
	// Lua Handle
	//Out表示に登録
	void registOutString(unsigned int n, std::string const& s)
	{
		m_outBuffer[n] = s;
	}

	//入力を取得
	bool getInput(std::size_t i) const
	{
		assert( i < system::ii::DegitalID::VButton_ModelButtonCount);
		return m_inputBuffer[i];
	}
	bool getInputDown(unsigned int i) const
	{
		assert( i < system::ii::DegitalID::VButton_ModelButtonCount);
		return m_inputDown[i];
	}
	bool getInputUp(unsigned int i) const
	{
		assert( i < system::ii::DegitalID::VButton_ModelButtonCount);
		return m_inputUp[i];
	}

	void simulationContextHandle( bool const t) const
	{
		m_simulationContext(t);
	}

	//Environment
	void changeEnvironmentLand(boost::shared_ptr<land::EnvironmentLand> l)
	{
		m_landPtr.swap(l);
	}
	//! 真下方向の距離を計測
	boost::optional<btScalar>
		getLandAlt(btVector3 const& point, btScalar depth) const
		{
			return m_landPtr->getLandHeight(point, depth);
		}

private:

	//ModelValues write
	void writeModelValueToLua( model::value::ModelValueBuffer& values);

	//ModelValues read
	void readModelValueToLua( model::value::ModelValueBuffer& values);


};
}
}

#endif /* MODELLUAENVIRONMENT_HPP_ */
