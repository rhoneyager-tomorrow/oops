/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "model/ObsWSpeedTLAD.h"

#include <vector>

#include "eckit/config/Configuration.h"
#include "model/GomQG.h"
#include "model/ObsBias.h"
#include "model/ObsBiasIncrement.h"
#include "model/ObsSpaceQG.h"
#include "model/ObsVecQG.h"
#include "model/QgFortran.h"
#include "oops/base/Variables.h"
#include "oops/util/Logger.h"

// -----------------------------------------------------------------------------
namespace qg {
// -----------------------------------------------------------------------------
static ObsOpTLADMaker<ObsWSpeedTLAD> makerWSpeedTL_("WSpeed");
// -----------------------------------------------------------------------------

ObsWSpeedTLAD::ObsWSpeedTLAD(const ObsSpaceQG & odb, const eckit::Configuration & config)
  : traj_(), varin_(std::vector<std::string>{"u", "v"})
{
  qg_wspeed_gettraj_f90(odb.nobs(), varin_, traj_.toFortran());
  oops::Log::trace() << "ObsWSpeedTLAD created" << std::endl;
}

// -----------------------------------------------------------------------------

void ObsWSpeedTLAD::setTrajectory(const GomQG & gom, const ObsBias &) {
  qg_wspeed_settraj_f90(gom.toFortran(), traj_.toFortran());
  oops::Log::trace() << "ObsWSpeedTLAD trajectory was set " << traj_ << std::endl;
}

// -----------------------------------------------------------------------------

void ObsWSpeedTLAD::simulateObsTL(const GomQG & gom, ObsVecQG & ovec,
                                  const ObsBiasIncrement & bias) const {
  qg_wspeed_equiv_tl_f90(gom.toFortran(), ovec.toFortran(),
                         traj_.toFortran(), bias.wspd());
}

// -----------------------------------------------------------------------------

void ObsWSpeedTLAD::simulateObsAD(GomQG & gom, const ObsVecQG & ovec,
                                  ObsBiasIncrement & bias) const {
  qg_wspeed_equiv_ad_f90(gom.toFortran(), ovec.toFortran(),
                         traj_.toFortran(), bias.wspd());
}

// -----------------------------------------------------------------------------

void ObsWSpeedTLAD::print(std::ostream & os) const {
  os << "ObsStreamTLAD::print not implemented" << std::endl;
}

// -----------------------------------------------------------------------------

}  // namespace qg
