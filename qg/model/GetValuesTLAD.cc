/*
 * (C) Copyright 2019-2020 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "model/GetValuesTLAD.h"

#include "oops/util/Logger.h"

#include "model/GeometryQG.h"
#include "model/GomQG.h"
#include "model/IncrementQG.h"
#include "model/LocationsQG.h"
#include "model/StateQG.h"

namespace qg {

// -----------------------------------------------------------------------------
/// Constructor, destructor
// -----------------------------------------------------------------------------
GetValuesTLAD::GetValuesTLAD(const GeometryQG & geom, const LocationsQG & locs) {
  qg_getvalues_create_f90(key_, geom.toFortran(), locs.toFortran());
}
// -----------------------------------------------------------------------------
GetValuesTLAD::~GetValuesTLAD() {
  qg_getvalues_delete_f90(key_);
}
// -----------------------------------------------------------------------------
void GetValuesTLAD::setTrajectory(const StateQG & state, const util::DateTime & t1,
                                  const util::DateTime & t2, GomQG & geovals) {
  qg_getvalues_interp_f90(key_, state.fields().toFortran(),
                          t1, t2, geovals.toFortran());
}
// -----------------------------------------------------------------------------
/// Get increment values at observation locations
// -----------------------------------------------------------------------------
void GetValuesTLAD::fillGeoVaLsTL(const IncrementQG & inc, const util::DateTime & t1,
                                  const util::DateTime & t2, GomQG & geovals) const {
  qg_getvalues_interp_tl_f90(key_, inc.fields().toFortran(),
                             t1, t2, geovals.toFortran());
}
// -----------------------------------------------------------------------------
void GetValuesTLAD::fillGeoVaLsAD(IncrementQG & inc, const util::DateTime & t1,
                                  const util::DateTime & t2, const GomQG & geovals) const {
  qg_getvalues_interp_ad_f90(key_, inc.fields().toFortran(),
                             t1, t2, geovals.toFortran());
}
// -----------------------------------------------------------------------------
void GetValuesTLAD::print(std::ostream & os) const {
  os << "GetValuesTLAD " << std::endl;
}
// -----------------------------------------------------------------------------

}  // namespace qg
