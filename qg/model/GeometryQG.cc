/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "util/Logger.h"
#include "model/GeometryQG.h"
#include "model/QgFortran.h"
#include "eckit/config/Configuration.h"

// -----------------------------------------------------------------------------
namespace qg {
// -----------------------------------------------------------------------------
GeometryQG::GeometryQG(const eckit::Configuration & conf) {
  const eckit::Configuration * configc = &conf;
  qg_geo_setup_f90(keyGeom_, &configc);
}
// -----------------------------------------------------------------------------
GeometryQG::GeometryQG(const GeometryQG & other) {
  const int key_geo = other.keyGeom_;
  qg_geo_clone_f90(key_geo, keyGeom_);
}
// -----------------------------------------------------------------------------
GeometryQG::~GeometryQG() {
  qg_geo_delete_f90(keyGeom_);
}
// -----------------------------------------------------------------------------
std::vector<double> GeometryQG::getLats() const {
  int nx;
  int ny;
  qg_geo_info_f90(keyGeom_, nx, ny);
  std::vector<double> lats(nx * ny);
  const double dy = 80.0 / double(ny);
  int jj = 0;
  for (int jy = 0; jy < ny; ++jy) {
    for (int jx = 0; jx < nx; ++jx) {
      lats[jj] = jy * dy - 40.0;
      ++jj;
    }
  }
  return lats;
}
// -----------------------------------------------------------------------------
std::vector<double> GeometryQG::getLons() const {
  int nx;
  int ny;
  qg_geo_info_f90(keyGeom_, nx, ny);
  std::vector<double> lons(nx * ny);
  const double dx = 360.0 / double(nx);
  int jj = 0;
  for (int jy = 0; jy < ny; ++jy) {
    for (int jx = 0; jx < nx; ++jx) {
      lons[jj] = jx * dx;
      ++jj;
    }
  }
  return lons;
}
// -----------------------------------------------------------------------------
std::vector<double> GeometryQG::getLevs() const {
  std::vector<double> levs(2);
  levs[0] = 0.0;
  levs[1] = 1.0;
  return levs;
}
// -----------------------------------------------------------------------------
std::vector<int> GeometryQG::getMask(const int &) const {
  int nx;
  int ny;
  qg_geo_info_f90(keyGeom_, nx, ny);
  std::vector<int> mask(nx * ny);
  for (unsigned int jj = 0; jj < mask.size(); ++jj) mask[jj] = 1;
  return mask;
}
// -----------------------------------------------------------------------------
void GeometryQG::print(std::ostream & os) const {
  int nx;
  int ny;
  qg_geo_info_f90(keyGeom_, nx, ny);
  os << "nx = " << nx << ", ny = " << ny;
}
// -----------------------------------------------------------------------------
}  // namespace qg
