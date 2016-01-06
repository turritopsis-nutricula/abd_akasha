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


//! モデルのkeyプロックに相当するクラス
/**
 * 一つのモデル変数に対応。
 * 複数の仮想ボタンを擁する。
 * TODO:ボタンを直接持つのはやめて識別子を。
 */
template<typename T, typename S>
	class ModelControlKey{

		//! 入力キーとstep値の内部構造体
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

		//! 更新
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

		//! 要素追加
		void addItem(int b, const btScalar s){
			m_items.push_back(keyItem(b, s));
		}

		//! 対象パラメータ名の取得
		const S& getParameterName() const{
			return m_param.getName();
		}
	private:
		//ModelControlKey(){ };       // 引数無しのコンストラクタは封印
	private:
		value::ModelValue<T,S>& m_param;
		std::vector<keyItem> m_items;

		btScalar m_defaultStep;
	};
}
}
}

#endif //MODEL_CONTROL_KEY__HPP
