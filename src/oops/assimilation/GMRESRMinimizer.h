/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef OOPS_ASSIMILATION_GMRESRMINIMIZER_H_
#define OOPS_ASSIMILATION_GMRESRMINIMIZER_H_

#include <string>

#include "oops/assimilation/ControlIncrement.h"
#include "oops/assimilation/CostFunction.h"
#include "oops/assimilation/GMRESR.h"
#include "oops/assimilation/HessianMatrix.h"
#include "oops/assimilation/PMatrix.h"
#include "oops/assimilation/PrimalMinimizer.h"

namespace oops {

/// GMRESR Minimizer
/*!
 * Implements the GMRESR algorithm.
 */

// -----------------------------------------------------------------------------

template<typename MODEL, typename OBS> class GMRESRMinimizer : public PrimalMinimizer<MODEL, OBS> {
  typedef CostFunction<MODEL, OBS>        CostFct_;
  typedef ControlIncrement<MODEL, OBS>    CtrlInc_;
  typedef HessianMatrix<MODEL, OBS>       Hessian_;
  typedef PMatrix<MODEL, OBS>             Pmat_;

 public:
  const std::string classname() const override {return "GMRESRMinimizer";}
  GMRESRMinimizer(const eckit::Configuration &, const CostFct_ & J)
     : PrimalMinimizer<MODEL, OBS>(J) {}
  ~GMRESRMinimizer() {}

 private:
  double solve(CtrlInc_ &, const CtrlInc_ &,
               const Hessian_ &, const Pmat_ &,
               const int, const double) override;
};

// =============================================================================

template<typename MODEL, typename OBS>
double GMRESRMinimizer<MODEL, OBS>::solve(CtrlInc_ & dx, const CtrlInc_ & rhs,
                                     const Hessian_ & hessian, const Pmat_ & P,
                                     const int ninner, const double gnreduc) {
// Solve the linear system
  const double reduc = GMRESR(dx, rhs, hessian, P, ninner, gnreduc);
  return reduc;
}

// -----------------------------------------------------------------------------

}  // namespace oops

#endif  // OOPS_ASSIMILATION_GMRESRMINIMIZER_H_
