/*
 * (C) Copyright 2009-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "model/GomQG.h"

#include "eckit/config/Configuration.h"
#include "model/LocationsQG.h"
#include "model/QgFortran.h"
#include "model/VariablesQG.h"
#include "oops/base/Variables.h"
#include "util/Logger.h"

namespace qg {

// -----------------------------------------------------------------------------
GomQG::GomQG(const LocationsQG & locs, const oops::Variables & var) {
  const VariablesQG varqg(var);

  // gom_setup just creates and allocates the GeoVaLs object without filling
  // in values
  qg_gom_setup_f90(keyGom_, locs.toFortran(), varqg.toFortran());
}
// -----------------------------------------------------------------------------
GomQG::GomQG(const eckit::Configuration & config, const oops::Variables &) {
  qg_gom_create_f90(keyGom_);
  const eckit::Configuration * conf = &config;
  qg_gom_read_file_f90(keyGom_, &conf);
}
// -----------------------------------------------------------------------------
/*! QG GeoVaLs Constructor with Config
 *
 * \details This constructor can be used to create a GeoVaLs object either
 * by reading it from a file or by using one of several analytic
 * initialization procedures.
 *
 * \sa qg_gom_analytic_init_c() analytic_init()
 */

GomQG::GomQG(const LocationsQG & locs, const oops::Variables & var,
             const eckit::Configuration & config) {
  qg_gom_create_f90(keyGom_);

  // Pass F90 routines a pointer to the configuration object
  const eckit::Configuration * conp = &config;

  if (config.has("analytic_init")) {
    const VariablesQG varqg(var);
    qg_gom_analytic_init_f90(keyGom_, locs.toFortran(), varqg.toFortran(), &conp);
  } else {
    qg_gom_read_file_f90(keyGom_, &conp);
  }
}
// -----------------------------------------------------------------------------
// Copy constructor
GomQG::GomQG(const GomQG & other) {
  // qg_gom_assign(keyGom_, other.keyGom_)
}
// -----------------------------------------------------------------------------
GomQG::~GomQG() {
  qg_gom_delete_f90(keyGom_);
}
// -----------------------------------------------------------------------------
void GomQG::abs() {
  qg_gom_abs_f90(keyGom_);
}
// -----------------------------------------------------------------------------
double GomQG::norm() const {
  double zz;
  qg_gom_rms_f90(keyGom_, zz);
  return zz;
}
// -----------------------------------------------------------------------------
void GomQG::zero() {
  qg_gom_zero_f90(keyGom_);
}
// -----------------------------------------------------------------------------
void GomQG::random() {
  qg_gom_random_f90(keyGom_);
}
// -----------------------------------------------------------------------------
GomQG & GomQG::operator=(const GomQG & rhs) {
  const int keyGomRhs = rhs.keyGom_;
  qg_gom_assign_f90(keyGom_, keyGomRhs);
  return *this;
}
// -----------------------------------------------------------------------------
GomQG & GomQG::operator*=(const double & zz) {
  qg_gom_mult_f90(keyGom_, zz);
  return *this;
}
// -----------------------------------------------------------------------------
GomQG & GomQG::operator+=(const GomQG & other) {
  qg_gom_add_f90(keyGom_, other.keyGom_);
  return *this;
}
// -----------------------------------------------------------------------------
GomQG & GomQG::operator-=(const GomQG & other) {
  qg_gom_diff_f90(keyGom_, other.keyGom_);
  return *this;
}
// -----------------------------------------------------------------------------
GomQG & GomQG::operator/=(const GomQG & other) {
  qg_gom_normalize_f90(keyGom_, other.keyGom_);
  return *this;
}
// -----------------------------------------------------------------------------
double GomQG::dot_product_with(const GomQG & other) const {
  double zz;
  qg_gom_dotprod_f90(keyGom_, other.keyGom_, zz);
  return zz;
}
// -----------------------------------------------------------------------------
void GomQG::read(const eckit::Configuration & config) {
  const eckit::Configuration * conf = &config;
  qg_gom_read_file_f90(keyGom_, &conf);
}
// -----------------------------------------------------------------------------
void GomQG::write(const eckit::Configuration & config) const {
  const eckit::Configuration * conf = &config;
  qg_gom_write_file_f90(keyGom_, &conf);
}
// -----------------------------------------------------------------------------
void GomQG::print(std::ostream & os) const {
  int nn;
  double zmin, zmax, zavg;
  qg_gom_minmaxavg_f90(keyGom_, nn, zmin, zmax, zavg);
  os << " nobs= " << nn << " Min=" << zmin << ", Max=" << zmax << ", RMS="
     << zavg << std::endl;

  // If the min value across all variables is positive, then this may be an
  // error measurement.  If so, print the location and variable where the
  // maximum occurs to the debug stream, for use in debugging

  if (zmin >= 0.0) {
    double mxval;
    int iloc, ivar;

    qg_gom_maxloc_f90(keyGom_, mxval, iloc, ivar);

    oops::Log::debug() << "GomQG: Maximum Value = " << std::setprecision(4)
                       << mxval << " at location = " << iloc
                       << " and variable = " << ivar << std::endl;
  }
}
// -----------------------------------------------------------------------------
}  // namespace qg
