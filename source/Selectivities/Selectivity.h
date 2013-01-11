/*
 * Selectivity.h
 *
 *  Created on: 21/12/2012
 *      Author: Admin
 */

#ifndef SELECTIVITY_H_
#define SELECTIVITY_H_

// Headers
#include <map>

#include "BaseClasses/Object.h"
#include "Utilities/Types.h"

// Namespaces
namespace isam {

// Using
using std::map;
using isam::utilities::Double;

/**
 * Class Definition
 */
class Selectivity : public isam::base::Object {
public:
  // Methods
  Selectivity();
  virtual                     ~Selectivity();
  virtual void                Validate() = 0;
  virtual void                Build() = 0;
  virtual Double              GetResult(unsigned age_or_length) = 0;

protected:
  // Members
  map<unsigned, Double>       values_;

};

/**
 * Typedef
 */
typedef boost::shared_ptr<Selectivity> SelectivityPtr;

} /* namespace isam */
#endif /* SELECTIVITY_H_ */
