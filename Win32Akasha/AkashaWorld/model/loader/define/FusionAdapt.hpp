/*
 * FusionAdapt.hpp
 *
 *  Created on: 2011/12/26
 *      Author: ely
 */

#ifndef FUSIONADAPT_HPP_
#define FUSIONADAPT_HPP_

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/adapt_adt.hpp>
#include "ModelDefineStruct.hpp"

BOOST_FUSION_ADAPT_TPL_STRUCT(
    (String),
    (akasha::model::loader::ModelExpressionDefine) (String),
    (String, name_) (typename akasha::model::loader::ModelExpressionDefine<String>::adaptType, value_))

BOOST_FUSION_ADAPT_TPL_STRUCT(
    (StringType),
    (akasha::model::loader::ModelValueDefine) (StringType),
    (StringType, valname_) (std::vector<akasha::model::loader::ModelExpressionDefine<StringType>>, propatyes_))

BOOST_FUSION_ADAPT_TPL_STRUCT(
    (StringType),
    (akasha::model::loader::ModelKeyDefine) (StringType),
    (unsigned int, keyNumber_) (std::vector<typename akasha::model::loader::ModelKeyDefine<StringType>::control_type>, controlValues_))

BOOST_FUSION_ADAPT_TPL_STRUCT(
    (StringType),
    (akasha::model::loader::ModelElementDefine) (StringType),
    (StringType, first_) (boost::optional<StringType>, second_) (std::vector<typename akasha::model::loader::ModelElementDefine<StringType>::propaty_type>, propaties_) (std::vector<akasha::model::loader::ModelElementDefine<StringType> >, children_))

#endif /* FUSIONADAPT_HPP_ */
