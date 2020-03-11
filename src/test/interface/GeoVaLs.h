/*
 * (C) Copyright 2017-2018 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef TEST_INTERFACE_GEOVALS_H_
#define TEST_INTERFACE_GEOVALS_H_

#include <cmath>
#include <memory>
#include <string>
#include <vector>

#define ECKIT_TESTING_SELF_REGISTER_CASES 0

#include <boost/noncopyable.hpp>

#include "eckit/config/LocalConfiguration.h"
#include "eckit/testing/Test.h"
#include "oops/base/ObsSpaces.h"
#include "oops/base/Variables.h"
#include "oops/interface/GeoVaLs.h"
#include "oops/parallel/mpi/mpi.h"
#include "oops/runs/Test.h"
#include "oops/util/dot_product.h"
#include "oops/util/Logger.h"
#include "test/TestEnvironment.h"

namespace test {

// -----------------------------------------------------------------------------

template <typename MODEL>
class GeoVaLsFixture : private boost::noncopyable {
  typedef oops::ObsSpaces<MODEL>  ObsSpaces_;

 public:
  static ObsSpaces_         & obspace() {return *getInstance().ospaces_;}
  static eckit::LocalConfiguration conf(const size_t ii) {return getInstance().confs_[ii];}

 private:
  static GeoVaLsFixture<MODEL>& getInstance() {
    static GeoVaLsFixture<MODEL> theGeoVaLsFixture;
    return theGeoVaLsFixture;
  }

  GeoVaLsFixture(): ospaces_() {
    const util::DateTime tbgn(TestEnvironment::config().getString("window_begin"));
    const util::DateTime tend(TestEnvironment::config().getString("window_end"));

    const eckit::LocalConfiguration conf(TestEnvironment::config(), "Observations");
    ospaces_.reset(new ObsSpaces_(conf, oops::mpi::comm(), tbgn, tend));

    conf.get("ObsTypes", confs_);
  }

  ~GeoVaLsFixture() {}

  std::unique_ptr<ObsSpaces_> ospaces_;
  std::vector<eckit::LocalConfiguration> confs_;
};

// -----------------------------------------------------------------------------

template <typename MODEL> void testConstructor() {
  typedef GeoVaLsFixture<MODEL>   Test_;
  typedef oops::GeoVaLs<MODEL>    GeoVaLs_;

  for (std::size_t jj = 0; jj < Test_::obspace().size(); ++jj) {
    eckit::LocalConfiguration gconf(Test_::conf(jj), "GeoVaLs");
    oops::Variables geovars(gconf);
    std::unique_ptr<GeoVaLs_> ov(new GeoVaLs_(gconf, Test_::obspace()[jj], geovars));
    EXPECT(ov.get());

    ov.reset();
    EXPECT(!ov.get());
  }
}

// -----------------------------------------------------------------------------

template <typename MODEL> void testUtils() {
  typedef GeoVaLsFixture<MODEL>   Test_;
  typedef oops::GeoVaLs<MODEL>    GeoVaLs_;

  const double tol = 1e-6;

  for (std::size_t jj = 0; jj < Test_::obspace().size(); ++jj) {
    eckit::LocalConfiguration gconf(Test_::conf(jj), "GeoVaLs");
    oops::Variables geovars(gconf);
    GeoVaLs_ gval(gconf, Test_::obspace()[jj], geovars);

    const double zz = dot_product(gval, gval);

    oops::Log::trace() << "Testing copy constructor (=) " << std::endl;

    GeoVaLs_ gval2 = gval;

    double zz0 = dot_product(gval2, gval2);

    EXPECT(zz0 == zz);

    oops::Log::trace() << "Testing *= double" << std::endl;

    gval2 *= 2.0;

    const double zz1 = dot_product(gval2, gval2);

    EXPECT(zz1/zz - 4.0 < tol);

    oops::Log::trace() << "Testing += GeoVals" << std::endl;

    gval2 += gval;

    const double zz2 = dot_product(gval2, gval2);

    EXPECT(zz2/zz - 9.0  < tol);

    oops::Log::trace() << "Testing -= GeoVals" << std::endl;

    gval2 -= gval;

    const double zz3 = dot_product(gval2, gval2);

    EXPECT(zz3/zz - 4.0  < tol);

    oops::Log::trace() << "Testing *= GeoVals" << std::endl;

    gval2 = gval;

    gval2 *= gval;

    GeoVaLs_ gval3 = gval2;

    gval3 *= gval;

    const double zz4 = dot_product(gval2, gval2);

    const double zz5 = dot_product(gval3, gval);

    EXPECT(zz4/zz5 - 1.0  < tol);

    oops::Log::trace() << "Testing random" << std::endl;

    gval.random();
    const double zz6 = dot_product(gval, gval);
    EXPECT(zz6 > 0.0);

    oops::Log::trace() << "Testing zero" << std::endl;

    gval.zero();
    const double zz7 = dot_product(gval, gval);
    EXPECT(zz7 == 0.0);
  }
}

// -----------------------------------------------------------------------------

template <typename MODEL> void testRead() {
  typedef GeoVaLsFixture<MODEL> Test_;
  typedef oops::GeoVaLs<MODEL>  GeoVaLs_;

  const double tol = 1.0e-9;
  for (std::size_t jj = 0; jj < Test_::obspace().size(); ++jj) {
    eckit::LocalConfiguration gconf(Test_::conf(jj), "GeoVaLs");
    oops::Variables geovars(gconf);
    GeoVaLs_ gval(gconf, Test_::obspace()[jj], geovars);

    const double xx = gconf.getDouble("norm");
    const double zz = sqrt(dot_product(gval, gval));

    oops::Log::debug() << "xx: " << std::fixed << std::setprecision(8) << xx << std::endl;
    oops::Log::debug() << "zz: " << std::fixed << std::setprecision(8) << zz << std::endl;

    EXPECT(oops::is_close(xx, zz, tol));
  }
}

// -----------------------------------------------------------------------------

template <typename MODEL>
class GeoVaLs : public oops::Test {
 public:
  GeoVaLs() {}
  virtual ~GeoVaLs() {}
 private:
  std::string testid() const {return "test::GeoVaLs<" + MODEL::name() + ">";}

  void register_tests() const {
    std::vector<eckit::testing::Test>& ts = eckit::testing::specification();

    ts.emplace_back(CASE("interface/GeoVaLs/testConstructor")
      { testConstructor<MODEL>(); });
    ts.emplace_back(CASE("interface/GeoVaLs/testUtils")
      { testUtils<MODEL>(); });
    ts.emplace_back(CASE("interface/GeoVaLs/testRead")
      { testRead<MODEL>(); });
  }
};

// =============================================================================

}  // namespace test

#endif  // TEST_INTERFACE_GEOVALS_H_
