/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef TEST_INTERFACE_INCREMENT_H_
#define TEST_INTERFACE_INCREMENT_H_

#include <cmath>
#include <iostream>
#include <string>

#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

#include "eckit/config/LocalConfiguration.h"
#include "oops/base/Variables.h"
#include "oops/interface/Geometry.h"
#include "oops/interface/Increment.h"
#include "oops/interface/InterpolatorTraj.h"
#include "oops/interface/State.h"
#include "oops/runs/Test.h"
#include "oops/util/DateTime.h"
#include "oops/util/dot_product.h"
#include "test/TestEnvironment.h"

namespace test {

// =============================================================================

template <typename MODEL> class IncrementFixture : private boost::noncopyable {
  typedef oops::Geometry<MODEL>       Geometry_;

 public:
  static const Geometry_       & resol()   {return *getInstance().resol_;}
  static const oops::Variables & ctlvars() {return *getInstance().ctlvars_;}
  static const util::DateTime  & time()    {return *getInstance().time_;}

 private:
  static IncrementFixture<MODEL>& getInstance() {
    static IncrementFixture<MODEL> theIncrementFixture;
    return theIncrementFixture;
  }

  IncrementFixture<MODEL>() {
//  Setup a geometry
    const eckit::LocalConfiguration resolConfig(TestEnvironment::config(), "Geometry");
    resol_.reset(new Geometry_(resolConfig));

    const eckit::LocalConfiguration varConfig(TestEnvironment::config(), "Variables");
    ctlvars_.reset(new oops::Variables(varConfig));

    time_.reset(new util::DateTime(TestEnvironment::config().getString("TestDate")));
  }

  ~IncrementFixture<MODEL>() {}

  boost::scoped_ptr<Geometry_>       resol_;
  boost::scoped_ptr<oops::Variables> ctlvars_;
  boost::scoped_ptr<util::DateTime>  time_;
};

// =============================================================================

template <typename MODEL> void testIncrementConstructor() {
  typedef IncrementFixture<MODEL>   Test_;
  typedef oops::Increment<MODEL>    Increment_;

  Increment_ dx(Test_::resol(), Test_::ctlvars(), Test_::time());

  BOOST_CHECK_EQUAL(dx.norm(), 0.0);
}

// -----------------------------------------------------------------------------

template <typename MODEL> void testIncrementCopyConstructor() {
  typedef IncrementFixture<MODEL>   Test_;
  typedef oops::Increment<MODEL>    Increment_;

  Increment_ dx1(Test_::resol(), Test_::ctlvars(), Test_::time());
  dx1.random();
  BOOST_CHECK(dx1.norm() > 0.0);

  Increment_ dx2(dx1);
  BOOST_CHECK(dx2.norm() > 0.0);

// Check that the copy is equal to the original
  dx2 -= dx1;
  BOOST_CHECK_EQUAL(dx2.norm(), 0.0);
}

// -----------------------------------------------------------------------------

template <typename MODEL> void testIncrementTriangle() {
  typedef IncrementFixture<MODEL>   Test_;
  typedef oops::Increment<MODEL>    Increment_;

  Increment_ dx1(Test_::resol(), Test_::ctlvars(), Test_::time());
  dx1.random();
  Increment_ dx2(Test_::resol(), Test_::ctlvars(), Test_::time());
  dx2.random();

// test triangle inequality
  double dot1 = dx1.norm();
  BOOST_CHECK(dot1 > 0.0);

  double dot2 = dx2.norm();
  BOOST_CHECK(dot2 > 0.0);

  dx2 += dx1;
  double dot3 = dx2.norm();
  BOOST_CHECK(dot3 > 0.0);

  BOOST_CHECK(dot3 <= dot1 + dot2);
}

// -----------------------------------------------------------------------------

template <typename MODEL> void testIncrementOpPlusEq() {
  typedef IncrementFixture<MODEL>   Test_;
  typedef oops::Increment<MODEL>    Increment_;

  Increment_ dx1(Test_::resol(), Test_::ctlvars(), Test_::time());
  dx1.random();
  Increment_ dx2(dx1);

// test *= and +=
  dx2 += dx1;
  dx1 *= 2.0;

  dx2 -= dx1;
  BOOST_CHECK_SMALL(dx2.norm(), 1e-8);
}

// -----------------------------------------------------------------------------

template <typename MODEL> void testIncrementDotProduct() {
  typedef IncrementFixture<MODEL>   Test_;
  typedef oops::Increment<MODEL>    Increment_;

  Increment_ dx1(Test_::resol(), Test_::ctlvars(), Test_::time());
  dx1.random();
  Increment_ dx2(Test_::resol(), Test_::ctlvars(), Test_::time());
  dx2.random();

// test symmetry of dot product
  double zz1 = dot_product(dx1, dx2);
  double zz2 = dot_product(dx2, dx1);

  BOOST_CHECK_EQUAL(zz1, zz2);
}

// -----------------------------------------------------------------------------

template <typename MODEL> void testIncrementZero() {
  typedef IncrementFixture<MODEL>   Test_;
  typedef oops::Increment<MODEL>    Increment_;

  Increment_ dx(Test_::resol(), Test_::ctlvars(), Test_::time());
  dx.random();
  BOOST_CHECK(dx.norm() > 0.0);

// test zero
  dx->zero();
  BOOST_CHECK_EQUAL(dx.norm(), 0.0);
}

// -----------------------------------------------------------------------------

template <typename MODEL> void testIncrementAxpy() {
  typedef IncrementFixture<MODEL>   Test_;
  typedef oops::Increment<MODEL>    Increment_;

  Increment_ dx1(Test_::resol(), Test_::ctlvars(), Test_::time());
  dx1.random();

// test axpy
  Increment_ dx2(dx1);
  dx2.axpy(2.0, dx1);

  dx2 -= dx1;
  dx2 -= dx1;
  dx2 -= dx1;

  BOOST_CHECK_SMALL(dx2.norm(), 1e-8);
}

// -----------------------------------------------------------------------------

template <typename MODEL> void testIncrementInterpAD() {
  typedef IncrementFixture<MODEL>       Test_;
  typedef oops::Increment<MODEL>        Increment_;
  typedef oops::Locations<MODEL>        Locations_;
  typedef oops::GeoVaLs<MODEL>          GeoVaLs_;
  typedef oops::InterpolatorTraj<MODEL> InterpolatorTraj_;
  typedef oops::State<MODEL>            State_;
  typedef eckit::LocalConfiguration     LocalConf_;

  // Check if "InterpTest" is present
  if (!TestEnvironment::config().has("InterpTest")) {
      oops::Log::warning() << "Bypassing test for adjoint of interpolation";
      return;
  }

  // Create Background
  const LocalConf_ confstate(TestEnvironment::config(), "State");
  const State_ xx(Test_::resol(), Test_::ctlvars(), confstate);

  // Locations from config
  const LocalConf_ configlocs(TestEnvironment::config(), "InterpTest.Locations");
  const Locations_ locs(configlocs);

  // Variables from config
  const LocalConf_ configvars(TestEnvironment::config(), "InterpTest.GeoVaLs");
  const oops::Variables vars(configvars);

  // Setup Increments
  Increment_ dx(Test_::resol(), Test_::ctlvars(), Test_::time());
  Increment_ Htdg(dx);
  Htdg.zero();

  // Get tolerance of dot product test
  const double tol = TestEnvironment::config().getDouble("InterpTest.tolerance");

  // Call NL getvalues to setup and initialize trajectory
  InterpolatorTraj_ traj;
  GeoVaLs_ hofx(locs, vars);
  xx.getValues(locs, vars, hofx, traj);

  // Randomize increments
  dx.random();

  // Create geovals from locs and vars
  GeoVaLs_ Hdx(locs, vars);

  // Forward getValues (state to geovals)
  dx.getValuesTL(locs, vars, Hdx, traj);

  // Setup and randomize geoval increments
  GeoVaLs_ dg(Hdx);
  dg.zero();
  dg.random();

  // Backward getValues (geovals to state)
  Htdg.getValuesAD(locs, vars, dg, traj);

  // Check adjoint: <Htdg,dx>=<dg,Hdx>
  double zz1 = dot_product(Htdg, dx);
  double zz2 = dot_product(dg, Hdx);

  oops::Log::debug() << "Adjoint test result: (<HTdg,dx>-<dg,Hdx>) = "
                       << zz1-zz2 << std::endl;

  BOOST_CHECK(zz1 != 0.0);
  BOOST_CHECK(zz2 != 0.0);
  BOOST_CHECK_CLOSE(zz1, zz2, tol);
}

// =============================================================================

template <typename MODEL> class Increment : public oops::Test {
 public:
  Increment() {}
  virtual ~Increment() {}
 private:
  std::string testid() const {return "test::Increment<" + MODEL::name() + ">";}

  void register_tests() const {
    boost::unit_test::test_suite * ts = BOOST_TEST_SUITE("interface/Increment");

    ts->add(BOOST_TEST_CASE(&testIncrementConstructor<MODEL>));
    ts->add(BOOST_TEST_CASE(&testIncrementCopyConstructor<MODEL>));
    ts->add(BOOST_TEST_CASE(&testIncrementTriangle<MODEL>));
    ts->add(BOOST_TEST_CASE(&testIncrementOpPlusEq<MODEL>));
    ts->add(BOOST_TEST_CASE(&testIncrementDotProduct<MODEL>));
    ts->add(BOOST_TEST_CASE(&testIncrementAxpy<MODEL>));
    ts->add(BOOST_TEST_CASE(&testIncrementInterpAD<MODEL>));

    boost::unit_test::framework::master_test_suite().add(ts);
  }
};

// =============================================================================

}  // namespace test

#endif  // TEST_INTERFACE_INCREMENT_H_
