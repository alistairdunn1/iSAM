/**
 * @file ParameterList-inl.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 19/09/2012
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/�2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef PARAMETERLIST_PARAMETER_INL_H_
#define PARAMETERLIST_PARAMETER_INL_H_

// Headers
#include <typeinfo>

#include "../Logging/Logging.h"
#include "../Utilities/To.h"
#include "../Utilities/Types.h"

// Namespaces
namespace niwa {
namespace parameterlist {

using niwa::utilities::Double;

/**
 *
 */
template <typename T>
void Parameter::RequireValueType() const {
  T result_value;

  for (size_t i = 0; i < values_.size(); ++i) {
    if (!niwa::utilities::To<T>(values_[i], result_value)) {
      LOG_ERROR() << "At line " << line_number_ << " in file " << file_name_ << ": " << label_ << " cannot be converted to type " << typeid(T).name() << " because the value "
                  << values_[i] << " is invalid";
    }
  }
}

/**
 * Specialisation: unsigned
 */
template <>
inline void Parameter::RequireValueType<unsigned>() const {
  unsigned result_value;

  for (size_t i = 0; i < values_.size(); ++i) {
    if (!niwa::utilities::To<unsigned>(values_[i], result_value)) {
      LOG_ERROR() << "At line " << line_number_ << " in file " << file_name_ << ": " << label_ << " cannot be converted to an unsigned integer because the value " << values_[i]
                  << " is invalid";
    }
  }
}

} /* namespace parameterlist */
} /* namespace niwa */
#endif /* PARAMETERLIST_PARAMETER_INL_H_ */
