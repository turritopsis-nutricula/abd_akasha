#ifndef MODEL_VALUE__HPP
#define MODEL_VALUE__HPP

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

namespace akasha
{
namespace model
{

namespace value
{
//! モデル変数
template<typename T,typename S>
	class ModelValue : boost::noncopyable
{
public:
	typedef T valueType;
	typedef S nameType;
private:
	S m_valueName;
	T m_volum;
public:
	T m_min;
	T m_max;
	T m_default;
	T m_step;
	int m_disp;

	ModelValue(const S& name, const T& def, const T& min, const T& max, const T& step,const int disp) :
		m_valueName(name),
		m_volum(def),
		m_min(min),
		m_max(max),
		m_default(def),
		m_step(step),
		m_disp(disp)
	{
	}

	//戻り処理
	void operator()()
	{
		(*this)(m_default, m_step);
	}

	//汎用処理
	void operator()(const T& target, const T& step)
	{
		if (m_volum < m_min)
			m_volum = m_min;
		else if(m_volum > m_max)
			m_volum = m_max;
		else
		{
			const T s = m_volum - target;
			if (s < -step) // 差がステップ値未満の時
				m_volum += step;
			else if ( s > step ) // 差がステップ値より大きいの
				m_volum -= step;
			else
				m_volum = target;
		}
	}

	S& getName()
	{
		return m_valueName;
	}
	const S& getName() const
	{
		return m_valueName;
	}

	const T& getVolum() const
	{
		return m_volum;
	}
	T& getVolum()
	{
		return m_volum;
	}
	void setVolum(const T t)
	{
		m_volum = t;
	}
};

using ModelValueType = value::ModelValue<float, std::string>;
using ModelValueBuffer = boost::ptr_vector<ModelValueType>;
}

}
}
#endif
