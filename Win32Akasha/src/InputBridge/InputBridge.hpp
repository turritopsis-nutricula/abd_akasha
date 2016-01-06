/*
 * InputBridge.hpp
 *
 *  Created on: 2012/01/08
 *      Author: ely
 */

#ifndef INPUTBRIDGE_HPP_
#define INPUTBRIDGE_HPP_

#include "../../AkashaSystem/AkashaSystem.hpp"

#include <nainai/nainai.hpp>
#include <boost/unordered_map.hpp>

namespace akasha
{
namespace win32
{
class InputBridge
{
	static const int keyboradBufferSize = 10;
	typedef unsigned short dword_t;
	boost::unordered_map<dword_t, akasha::system::ii::DegitalID> m_keyMap;

	boost::shared_ptr<nai::NainaiManager> m_naiManager;
	boost::shared_ptr<nai::Keyboard> m_keyboard;

public:
	InputBridge() = default;

	bool init(void* hInstance, void* hwnd)
	{
		m_naiManager = nai::CreateNainai(hInstance, hwnd);
		if (m_naiManager)
		{
			m_keyboard = boost::make_shared<nai::Keyboard>();
			m_naiManager->attachDefaultKeyboard(m_keyboard);
		}

		return m_naiManager.get();
	}



	//! キーマップを登録
	void
		setKeyMap(const dword_t& dinputKey, const akasha::system::ii::DegitalID::EDegitalInput& id)
		{
			m_keyMap[dinputKey] = akasha::system::ii::DegitalID(id);
		}
	void
		setKeyMap(const nai::keycode& keycode, const akasha::system::ii::DegitalID::EDegitalInput& id)
		{
			m_keyMap[static_cast<dword_t>(keycode)] = akasha::system::ii::DegitalID(id);
		}

	void
		update()
		{

			if ( !(m_keyboard && m_naiManager)) return;
			m_naiManager->update();

			boost::array<bool, 256> buffer;
			m_keyboard->getState(buffer);

			boost::unordered_map<akasha::system::ii::DegitalID, unsigned short> updateMap;
			for (std::size_t i=0; i < buffer.size(); ++i)
				if (m_keyMap.count(i)>0)
					updateMap[ m_keyMap[i]] = buffer[i] ? 0x80 : 0;
			akasha::system::AkashaSystem::Instance().getInput().setButtonUpdateMap(updateMap);

		}
};
}
}

#endif /* INPUTBRIDGE_HPP_ */
