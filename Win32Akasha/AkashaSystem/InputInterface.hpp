/*
 * InputInterface.hpp
 *
 *  Created on: 2012/01/08
 *      Author: ely
 */

#ifndef INPUTINTERFACE_HPP_
#define INPUTINTERFACE_HPP_

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/array.hpp>
#include <boost/foreach.hpp>

#include <boost/signals2.hpp>

namespace akasha
{
namespace system
{
namespace ii
{

struct DegitalID
{

	enum EDegitalInput
	{
		VButton_0,  //!< モデル操作ボタン
		VButton_1,
		VButton_2,
		VButton_3,
		VButton_4,
		VButton_5,
		VButton_6,
		VButton_7,
		VButton_8,
		VButton_9,
		VButton_10,
		VButton_11,
		VButton_12,
		VButton_13,
		VButton_14,
		VButton_15,
		VButton_16,
		VButton_17,
		VButton_18,
		VButton_19,
		VButton_20,
		VButton_ModelButtonCount,  //!< モデル操作キーカウント

		VButton_Ctrl_Modify, //!< コントロール
		VButton_Alt_Modify, //!< オルト

		VButton_POV_Up,  //!< モデル操作十字キー
		VButton_POV_Down,  //!< モデル操作十字キー
		VButton_POV_Right,  //!< モデル操作十字キー
		VButton_POV_Left,  //!< モデル操作十字キー

		VButton_YForce,  //!<  Yフォース発生
		VButton_Reset,  //!< モデル姿勢をリセット
		VButton_Init,  //!< モデルを初期位置へリセット
		VButton_Reload,  //!< モデルをその場で再読込

		VButton_ViewA,  //!< 視点変更
		VButton_ViewB,
		VButton_ViewD,
		VButton_ViewE,
		VButton_ViewF,
		VButton_ViewG,
		VButton_ViewH,
		VButton_ViewI,
		VButton_ViewJ,
		VButton_ViewX,

		VButton_ViewUp,  //!< 視点を見上げ視点へオフセット
		VButton_ViewDown,  //!< 視点を見下げ視点へオフセット
		VButton_ViewLeft,  //!< 視点を左向きへオフセット
		VButton_ViewRight,  //!< 視点を右向きへオフセット
		VButton_ViewZoomOut,  //!< 視点をズームアウト
		VButton_ViewZoomIn,  //!< 視点をズームイン
		VButton_ViewReset,   //!< 視点オフセットをリセット

		VButton_S0,  //!< シナリオ操作キー
		VButton_S1,
		VButton_S2,
		VButton_S3,

		VButton_Gravity,  //!< 重力
		VButton_Air,  //!< 空気抵抗
		VButton_Tourq,  //!< 回転出力
		VButton_Jet,  //!< Jet出力
		VButton_Unbrake,  //!< 破損
		VButton_CCD,  //!< CCD
		VButton_Script,  //!< スクリプト
		VButton_Fual,  //!< 燃料

		VButton_Mouse0,  //!< マウスボタン
		VButton_Mouse1,  //
		VButton_Mouse2,  //
		VButton_Mouse3,  //
		VButton_Mouse4,  //

		VButton_No
	};
	static const unsigned int count = 65;


	//TODO : 値範囲でassert
	DegitalID() :
		ID_(EDegitalInput::VButton_No)
	{
	}
	DegitalID(const int& i) :
		ID_((EDegitalInput) i)
	{
	}
	DegitalID(const EDegitalInput& i) :
		ID_(i)
	{
	}
	DegitalID(unsigned int& i) :
		ID_((EDegitalInput) i)
	{
	}
	bool
		operator==(const DegitalID& rh) const
		{
			return ID_ == rh.ID_;
		}
	EDegitalInput ID_;
};

struct ModifyKey
{
	char key_;
	bool isCtrl() const { return key_ & 0x1; };
	bool isAlt() const { return key_ & 0x10; };
};

	inline static size_t
hash_value(const DegitalID& h)
{
	return static_cast<std::size_t>(h.ID_);
}
struct AnalogID
{
	enum EAnalogInput
	{
		VAnalog_1,
		VAnalog_2,
		VAnalog_3,
		VAnalog_4,
		VAnalog_5,
		VAnalog_6,
		VAnalog_7,
		VAnalog_8,
		VAnalog_9,
		VAnalog_10,
		VAnalog_No
	};
	static const unsigned int count = 10;
	EAnalogInput ID_;
};

class VirtualInput
{
	boost::array<bool, ii::DegitalID::count> m_button;
	boost::array<bool, ii::DegitalID::count> m_buttonBuf;
	ModifyKey m_modify;

	typedef boost::signals2::signal<void
		(const ModifyKey&)> signal_type;

	boost::array<signal_type, ii::DegitalID::count> m_buttonSignal;
	boost::array<signal_type, ii::DegitalID::count> m_buttonPushDownSignal;
	boost::array<signal_type, ii::DegitalID::count> m_buttonReleaseUpSignal;

	boost::array<int, ii::AnalogID::count> m_analog;
public:
	explicit
		VirtualInput()
		{
			reset();
		}

	void
		reset()
		{
			for (unsigned int i = 0; i < m_button.size(); i++) {
				m_button[i] = false;
				m_buttonBuf[i] = false;
			}
			for (unsigned int i = 0; i < m_analog.size(); i++)
				m_analog[i] = 0;
		}

public:
	//登録メソッド
	template<typename V>
		void
		setAnalog(const V& v)
		{
			BOOST_STATIC_ASSERT(
					(!boost::is_same<typename V::value_type,int>::type));
			for (unsigned int i = 0; i < ii::AnalogID::count; i++)
				m_analog[i] = v[i];
		}
	template<typename V>
		void
		setButton(const V& v)
		{
			BOOST_STATIC_ASSERT(
					(!boost::is_same<typename V::value_type,bool>::type));
			for (unsigned int i = 0; i < ii::DegitalID::count;
					i++) {
				m_buttonBuf[i] = m_button[i];
				m_button[i] = v[i];
			}
		}
	template<typename M>
		void
		setButtonUpdateMap(const M& m)
		{
			//BOOST_STATIC_ASSERT((boost::is_same<typename boost::remove_cv<M>::type::key_type, ii::DegitalID>::type));

			m_modify.key_ =
				(m_button[DegitalID::VButton_Alt_Modify] || // mapがtrue
				 (!m_button[DegitalID::VButton_Alt_Modify] &&    //mapがfalseで
				  m.count(DegitalID::VButton_Alt_Modify)>0 && //入力に存在していて
				  m.at(DegitalID::VButton_Alt_Modify))) //trueであれば
				? 0x10 : 0 +
				(m_button[DegitalID::VButton_Ctrl_Modify] || // mapがtrue
				 (!m_button[DegitalID::VButton_Ctrl_Modify] &&    //mapがfalseで
				  m.count(DegitalID::VButton_Ctrl_Modify)>0 && //入力に存在していて
				  m.at(DegitalID::VButton_Ctrl_Modify)) ) //trueであれば
				? 0x1 : 0;



			typedef typename M::value_type valueType;
			BOOST_FOREACH(const valueType& i, m){
				if (m_button[i.first.ID_] && !i.second)
					m_buttonReleaseUpSignal[i.first.ID_](m_modify);

				if (i.second)
				{
					if ( !m_button[i.first.ID_] )
						m_buttonPushDownSignal[i.first.ID_](m_modify);
					m_buttonSignal[i.first.ID_](m_modify);
				}

				m_buttonBuf[i.first.ID_] =
					m_button[i.first.ID_];
				m_button[i.first.ID_] = i.second;

			}



		}

	//取得側メソッド
	int
		get(const ii::AnalogID& id) const
		{
			return m_analog[id.ID_];
		}
	bool
		get(const ii::DegitalID& id) const
		{
			return m_button[id.ID_];
		}

	//ボタンプッシュ
	template<typename O>
		boost::signals2::connection
		atach(const DegitalID& id, O&& observer)
		{
			return m_buttonSignal[id.ID_].connect(observer);
		}
	//Button Down
	template<typename O>
		boost::signals2::connection
		downAtach(DegitalID const& id, O observer)
		{
			return m_buttonPushDownSignal[id.ID_].connect(std::move(observer));
		}
	//ポタンリリース
	template<typename O>
		boost::signals2::connection
		releaseAtach(const DegitalID& id, O&& observer)
		{
			return m_buttonReleaseUpSignal[id.ID_].connect(observer);
		}

};

}
}
}

#endif /* INPUTINTERFACE_HPP_ */
