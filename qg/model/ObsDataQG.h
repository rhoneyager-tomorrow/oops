/*
 * (C) Copyright 2019  UCAR
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 */

#ifndef QG_MODEL_OBSDATAQG_H_
#define QG_MODEL_OBSDATAQG_H_

#include <math.h>
#include <ostream>
#include <string>
#include <vector>

#include "model/ObsSpaceQG.h"
#include "model/ObsVecQG.h"
#include "model/QgFortran.h"
#include "oops/base/Variables.h"
#include "oops/util/Logger.h"
#include "oops/util/ObjectCounter.h"
#include "oops/util/Printable.h"

namespace qg {

// -----------------------------------------------------------------------------
/// Data in observation space

template<typename DATATYPE>
class ObsDataQG : public util::Printable,
                  private util::ObjectCounter<ObsDataQG<DATATYPE> > {
 public:
  static const std::string classname() {return "qg::ObsDataQG";}

  ObsDataQG(const ObsSpaceQG &, const oops::Variables &, const std::string &);
  ObsDataQG(const ObsDataQG &);
  ~ObsDataQG() {}

  ObsDataQG & operator= (const ObsDataQG &);

  void zero();
  void mask(const ObsDataQG<int>);

// I/O
  void save(const std::string &) const;

 private:
  void print(std::ostream &) const;

  ObsVecQG data_;
};
//-----------------------------------------------------------------------------

template<typename DATATYPE>
ObsDataQG<DATATYPE>::ObsDataQG(const ObsSpaceQG & os, const oops::Variables & var,
                               const std::string & name): data_(os, var, name) {
}
// -----------------------------------------------------------------------------
template<typename DATATYPE>
ObsDataQG<DATATYPE>::ObsDataQG(const ObsDataQG & other): data_(other.data_) {
}
// -----------------------------------------------------------------------------
template<typename DATATYPE>
ObsDataQG<DATATYPE> & ObsDataQG<DATATYPE>::operator= (const ObsDataQG & rhs) {
  data_ = rhs.data_;
  return *this;
}
// -----------------------------------------------------------------------------
template<typename DATATYPE>
void ObsDataQG<DATATYPE>::zero() {
  data_.zero();
}
// -----------------------------------------------------------------------------
template<typename DATATYPE>
void ObsDataQG<DATATYPE>::mask(const ObsDataQG<int>) {
}
// -----------------------------------------------------------------------------
template<typename DATATYPE>
void ObsDataQG<DATATYPE>::save(const std::string & name) const {
  data_.save(name);
}
// -----------------------------------------------------------------------------
template<typename DATATYPE>
void ObsDataQG<DATATYPE>::print(std::ostream & os) const {
  os << data_;
}
// -----------------------------------------------------------------------------
}  // namespace qg

#endif  // QG_MODEL_OBSDATAQG_H_