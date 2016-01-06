#ifndef MODEL_CONTROL_KEY__HPP
#define MODEL_CONTROL_KEY__HPP

//std
#include <vector>

//boost
#include <boost/foreach.hpp>
//bullet
#include "LinearMath/btScalar.h"

//akasha
#include "../../AkashaSystem/AkashaSystem.hpp"

namespace akasha {

namespace model
{
namespace key
{


//! ���f����key�v���b�N�ɑ�������N���X
/**
 * ��̃��f���ϐ��ɑΉ��B
 * �����̉��z�{�^����i����B
 * TODO:�{�^���𒼐ڎ��̂͂�߂Ď��ʎq���B
 */
template<typename T, typename S>
	class ModelControlKey{

		//! ���̓L�[��step�l�̓����\����
		struct keyItem {
			//pAkashaButton m_button;
			system::ii::DegitalID m_buttonId;
			btScalar m_stepValue;
			explicit keyItem(const int b, const btScalar s) :
				m_buttonId((system::ii::DegitalID)b), m_stepValue(s){ };

		};

	public:
		ModelControlKey( value::ModelValue<T, S>& p) :
			m_param( p ),
			m_defaultStep(p.m_step){ };

		//! �X�V
		void update(bool keyInput){

			bool f(false);
			btScalar add(0.0);

			if (keyInput)
			{
				for (auto& item : m_items)
				{
					if (system::AkashaSystem::Instance().getInput().get(system::ii::DegitalID(item.m_buttonId))){
						add += item.m_stepValue;
						f = true;
						//std::cout << boost::format("%1% push.") % item.m_buttonId.ID_ << std::endl;
					}
				}
			}

			if (f) {
				m_param.m_step = btScalar(0.0);
				m_param.getVolum() +=  add;

			} else
				m_param.m_step = m_defaultStep;
		}

		//! �v�f�ǉ�
		void addItem(int b, const btScalar s){
			m_items.push_back(keyItem(b, s));
		}

		//! �Ώۃp�����[�^���̎擾
		const S& getParameterName() const{
			return m_param.getName();
		}
	private:
		//ModelControlKey(){ };       // ���������̃R���X�g���N�^�͕���
	private:
		value::ModelValue<T,S>& m_param;
		std::vector<keyItem> m_items;

		btScalar m_defaultStep;
	};
}
}
}

#endif //MODEL_CONTROL_KEY__HPP
