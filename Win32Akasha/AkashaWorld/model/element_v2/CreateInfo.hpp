#ifndef AKASHA_MODEL_ELEMENT_CREATEINFO_HPP_INCLUDED
#define AKASHA_MODEL_ELEMENT_CREATEINFO_HPP_INCLUDED

#include "../element/ElementData.hpp"
//Wisp
#include <wisp_v3/wisp.hpp>
#include <wisp_v3/irrExt.hpp>

namespace akasha
{
namespace model
{
namespace element_v2
{
struct CreateInfo
{
	element::ElementData data_;
	wisp::v3::WispHandle& wisp_;
	irrExt::IrrExtention& irrEx_;
};
}
}
}


#endif // AKASHA_MODEL_ELEMENT_CREATEINFO_HPP_INCLUDED
