#ifndef AKASHA_MODEL_ELEMENTBUFFER_HPP_INCLUDED
#define AKASHA_MODEL_ELEMENTBUFFER_HPP_INCLUDED

// TODO : working commentout
// #include "../element/ElementWrapper.hpp"
#include "CreateInfo.hpp"
#include "type_fwd.hpp"
#include "ElementConcept.hpp"

#include "../ModelObserver.hpp"

#include <boost/shared_ptr.hpp>


namespace akasha
{
namespace model
{
namespace element_v2
{


class ElementBuffer
{
private:
	struct addElement;
	template<int N>
		static int addFunc(
				ElementBuffer& buf, CreateInfo const&,
				boost::shared_ptr<akasha::NodeUpdateTask> const&);
	struct BufferImpl;

private:
	std::vector< ElementConcept > m_list;
	boost::shared_ptr<BufferImpl> m_impl;
	boost::shared_ptr<NodeUpdateTask> m_irrTask;

public:
	ElementBuffer();
	~ElementBuffer(); //PImpl

	void setIrrNodeTask(boost::shared_ptr<NodeUpdateTask> const& n)
	{
		m_irrTask = n;
	}
	//Return addElement id
	template<typename Type>
		int add(CreateInfo const& d, boost::type<Type> const& t)
		{
			return addFunc< TypeToId<Type>::value >(*this, d, m_irrTask);
		}

	unsigned int getCount() const
	{
		return m_list.size();
	}

	boost::shared_ptr<ModelObserver> makeStatusBuffer()
	{
		auto resultBuffer =
			boost::make_shared<ModelObserver>();

		for (auto const& e : m_list)
			resultBuffer->addElement(e.getParameterPtr());

		return resultBuffer;
	}

	std::vector< ElementConcept >&
		getList()
		{
			return m_list;
		}
	std::vector< ElementConcept> const&
		getList() const
		{
			return m_list;
		}
	std::vector< ElementInitConcept >
		getInitConceptList();
	std::vector< ElementManipulateConcept >
		getManipulateConceptList();
};
}
}
}



#endif // AKASHA_MODEL_ELEMENTBUFFER_HPP_INCLUDED
