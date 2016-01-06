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
		VButton_0,  //!< ���f������{�^��
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
		VButton_ModelButtonCount,  //!< ���f������L�[�J�E���g

		VButton_Ctrl_Modify, //!< �R���g���[��
		VButton_Alt_Modify, //!< �I���g

		VButton_POV_Up,  //!< ���f������\���L�[
		VButton_POV_Down,  //!< ���f������\���L�[
		VButton_POV_Right,  //!< ���f������\���L�[
		VButton_POV_Left,  //!< ���f������\���L�[

		VButton_YForce,  //!<  Y�t�H�[�X����
		VButton_Reset,  //!< ���f���p�������Z�b�g
		VButton_Init,  //!< ���f���������ʒu�փ��Z�b�g
		VButton_Reload,  //!< ���f�������̏�ōēǍ�

		VButton_ViewA,  //!< ���_�ύX
		VButton_ViewB,
		VButton_ViewD,
		VButton_ViewE,
		VButton_ViewF,
		VButton_ViewG,
		VButton_ViewH,
		VButton_ViewI,
		VButton_ViewJ,
		VButton_ViewX,

		VButton_ViewUp,  //!< ���_�����グ���_�փI�t�Z�b�g
		VButton_ViewDown,  //!< ���_�����������_�փI�t�Z�b�g
		VButton_ViewLeft,  //!< ���_���������փI�t�Z�b�g
		VButton_ViewRight,  //!< ���_���E�����փI�t�Z�b�g
		VButton_ViewZoomOut,  //!< ���_���Y�[���A�E�g
		VButton_ViewZoomIn,  //!< ���_���Y�[���C��
		VButton_ViewReset,   //!< ���_�I�t�Z�b�g�����Z�b�g

		VButton_S0,  //!< �V�i���I����L�[
		VButton_S1,
		VButton_S2,
		VButton_S3,

		VButton_Gravity,  //!< �d��
		VButton_Air,  //!< ��C��R
		VButton_Tourq,  //!< ��]�o��
		VButton_Jet,  //!< Jet�o��
		VButton_Unbrake,  //!< �j��
		VButton_CCD,  //!< CCD
		VButton_Script,  //!< �X�N���v�g
		VButton_Fual,  //!< �R��

		VButton_Mouse0,  //!< �}�E�X�{�^��
		VButton_Mouse1,  //
		VButton_Mouse2,  //
		VButton_Mouse3,  //
		VButton_Mouse4,  //

		VButton_No
	};
	static const unsigned int count = 65;


	//TODO : �l�͈͂�assert
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
	//�o�^���\�b�h
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
				(m_button[DegitalID::VButton_Alt_Modify] || // map��true
				 (!m_button[DegitalID::VButton_Alt_Modify] &&    //map��false��
				  m.count(DegitalID::VButton_Alt_Modify)>0 && //���͂ɑ��݂��Ă���
				  m.at(DegitalID::VButton_Alt_Modify))) //true�ł����
				? 0x10 : 0 +
				(m_button[DegitalID::VButton_Ctrl_Modify] || // map��true
				 (!m_button[DegitalID::VButton_Ctrl_Modify] &&    //map��false��
				  m.count(DegitalID::VButton_Ctrl_Modify)>0 && //���͂ɑ��݂��Ă���
				  m.at(DegitalID::VButton_Ctrl_Modify)) ) //true�ł����
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

	//�擾�����\�b�h
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

	//�{�^���v�b�V��
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
	//�|�^�������[�X
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
