/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef QG_MODEL_OBSWINDTLAD_H_
#define QG_MODEL_OBSWINDTLAD_H_

#include <string>

#include "model/ObsOpBaseTLAD.h"
#include "oops/base/Variables.h"
#include "oops/util/ObjectCounter.h"

// Forward declarations
namespace eckit {
  class Configuration;
}

namespace qg {
  class GomQG;
  class ObsBias;
  class ObsBiasIncrement;
  class ObsSpaceQG;
  class ObsVecQG;

// -----------------------------------------------------------------------------
/// Wind TL/AD observation operator for QG model.

class ObsWindTLAD : public ObsOpBaseTLAD,
                    private util::ObjectCounter<ObsWindTLAD> {
 public:
  static const std::string classname() {return "qg::ObsWindTLAD";}

  ObsWindTLAD(const ObsSpaceQG &, const eckit::Configuration &);

// Obs Operators
  void setTrajectory(const GomQG &, const ObsBias &) override;
  void simulateObsTL(const GomQG &, ObsVecQG &, const ObsBiasIncrement &) const override;
  void simulateObsAD(GomQG &, const ObsVecQG &, ObsBiasIncrement &) const override;

// Other
  const oops::Variables & requiredVars() const override {return varin_;}

 private:
  void print(std::ostream &) const override;
  const oops::Variables varin_;
};
// -----------------------------------------------------------------------------

}  // namespace qg
#endif  // QG_MODEL_OBSWINDTLAD_H_
