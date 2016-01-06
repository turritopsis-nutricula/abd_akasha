#include "ElementBuffer.hpp"

#include "LegacyElement.hpp"

#include "BasicChipElement.ipp"
#include "UndefineElement.ipp"
//
// #include "BatteryElement.ipp"
//
#include "VoidAdapterElement.ipp"

#include <boost/preprocessor/repetition/repeat_from_to.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/range/algorithm/sort.hpp>
#include <type_traits>

using namespace akasha::model;


extern void* enabler;

using element_v2::IdToType;
using element_v2::TypeToId;
using element_v2::ElementBuffer;
using element_v2::CreateInfo;
// Buffer Impl

namespace
{
template<typename T> struct Remaper{
	typedef std::vector<
		boost::shared_ptr<T>> type; };
typedef rice::mpl::ReMap<Remaper, element_v2::ElementRawTypeTuple>::type value_tuple;
typedef rice::mpl::TypeTupleFold<value_tuple> buffer_type;
}

struct ElementBuffer::BufferImpl
{
	buffer_type m_foldBuffer;
};
//Add Support Struct
struct ElementBuffer::addElement
{

	template<int N, typename std::enable_if<
		rice::mpl::Find<typename IdToType<N>::type,
		element_v2::ElementRawTypeTuple>::value >::type *& = enabler>
			static int addElement_(
					ElementBuffer& this_,
					CreateInfo const& d,
					boost::shared_ptr<akasha::NodeUpdateTask> const& task)
			{
				typedef typename IdToType<N>::type elemType;

				boost::shared_ptr<elemType> e(
						MakeElement(d, task, boost::type<elemType>()));
				element_v2::ElementHolder<elemType> h;
				h.this_ = e;
				this_.m_impl->m_foldBuffer.getValue(
						boost::type<
						std::vector<boost::shared_ptr<elemType>>
						>()).push_back(e);
				this_.m_list.push_back( ElementConcept( h ));
				return this_.m_list.size()-1;
			}

	template<int N, typename std::enable_if<
		! rice::mpl::Find<
		typename IdToType<N>::type,
					element_v2::ElementRawTypeTuple>::value
						>::type *& = enabler>
						static int addElement_(
								ElementBuffer& this_,
								CreateInfo const&,
								boost::shared_ptr<akasha::NodeUpdateTask> const&)
						{
							return this_.m_list.size()-1;
						}
};

template<typename ToConceptType>
struct ConceptVisiter
{
	typedef std::vector< ToConceptType > result_type;
	result_type& result_;
	// 	ConceptVisiter() = delete;
	ConceptVisiter(std::vector<ToConceptType>& b) : result_(b){ }
	template<typename BufferType>
		void operator()(BufferType& elemBuf) const
		{
			typedef typename BufferType::value_type value_type;
			typedef typename value_type::element_type elemType;
			for(auto e : elemBuf)
			{
				element_v2::ElementHolder<elemType> h;
				h.this_ = e;
				result_.push_back( ToConceptType(h) );
			}
		}
};

template<typename ToConceptType>
struct OncePushbackVisiter
{
	typedef std::vector< ToConceptType > result_type;

	int const serchId_;
	result_type& result_;
	bool& resultFlag_;

	template<typename BufferType>
		void operator()(BufferType& elementBuf) const
		{
			typedef typename BufferType::value_type value_type;
			typedef typename value_type::element_type elemType;
			for (auto const& e : elementBuf) {
				if (e->getId()==serchId_)
				{
					element_v2::ElementHolder<elemType> h;
					h.this_=e;
					result_.push_back( ToConceptType(h));
					resultFlag_ = true;
					return;
				}
			}
		}
};

	template<typename T>
std::vector<T> ConceptListCreate(buffer_type& b)
{
	std::vector<T> result;
	ConceptVisiter<T> vis(result);
	b.accept(vis);
	return std::move(result);
}

	template<typename T>
std::vector<T> SortedConceptListCreate(buffer_type& b, unsigned int const elementCount)
{
	std::vector<T> result;
	bool flg(false);
	for ( int i = 0; i < elementCount;++i){
		flg = false;
		OncePushbackVisiter<T> v{i, result, flg};
		b.accept(v);
		if (!flg) break;
	}

	return std::move(result);
}

///////////////////////////////
//ElementBuffer Implements

ElementBuffer::ElementBuffer()
{
	m_impl = boost::make_shared<ElementBuffer::BufferImpl>();

}
ElementBuffer::~ElementBuffer()
{
}
// InitConceptList
	std::vector< element_v2::ElementInitConcept >
ElementBuffer::getInitConceptList()
{
	// 	return ConceptListCreate<element_v2::ElementInitConcept>(m_impl->m_foldBuffer);
	return SortedConceptListCreate<
		element_v2::ElementInitConcept>( m_impl->m_foldBuffer,getCount());
}
//ManipulateConceptList
	std::vector< element_v2::ElementManipulateConcept >
ElementBuffer::getManipulateConceptList()
{
	// 	return ConceptListCreate<element_v2::ElementManipulateConcept>(m_impl->m_foldBuffer);
	return SortedConceptListCreate<
		element_v2::ElementManipulateConcept>( m_impl->m_foldBuffer,getCount());
}


//////////////////////////////////////////////////////////////////////////////////
// template decl
	template<int N>
int ElementBuffer::addFunc(
		ElementBuffer& this_,
		CreateInfo const& d,
		boost::shared_ptr<akasha::NodeUpdateTask> const& task)
{
	assert(task);
	return addElement::addElement_<N>(this_, d, task);
}


template<int N, int BASE>
constexpr int add_index(){ return N+BASE;}
// expand addFunc
#define FUNC_DECL(z, n, base)  \
	template int ElementBuffer::addFunc< add_index<n, base>() >( \
			ElementBuffer&,CreateInfo const&, boost::shared_ptr<akasha::NodeUpdateTask> const&);


// Core - Arm
BOOST_PP_REPEAT_FROM_TO(0,10, FUNC_DECL, 0)

	// Frame - TrimFrame
BOOST_PP_REPEAT_FROM_TO(33,36, FUNC_DECL, 0)

	// Battery -
BOOST_PP_REPEAT_FROM_TO(0,128, FUNC_DECL, 200)

	// HousingElement
BOOST_PP_REPEAT_FROM_TO(0, 99, FUNC_DECL, 500)

	// OutputShaftElement
BOOST_PP_REPEAT_FROM_TO(0, 128, FUNC_DECL, 600)

	// Void Adapter 1000-
BOOST_PP_REPEAT_FROM_TO(0,128, FUNC_DECL, 1000)

BOOST_PP_REPEAT_FROM_TO(0,1, FUNC_DECL, -1)
#undef FUNC_DECL
