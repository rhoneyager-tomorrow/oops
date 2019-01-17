! (C) Copyright 2009-2016 ECMWF.
! 
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
! In applying this licence, ECMWF does not waive the privileges and immunities 
! granted to it by virtue of its status as an intergovernmental organisation nor
! does it submit to any jurisdiction.

!>  Define interface for C++ config code called from Fortran

!-------------------------------------------------------------------------------
interface
!-------------------------------------------------------------------------------

type(c_ptr) function c_config_get_testdata() bind(C,name='config_get_testdata_f')
  use, intrinsic :: iso_c_binding
  implicit none
end function c_config_get_testdata

!-------------------------------------------------------------------------------

integer(kind=c_int) function c_config_get_data_length(dom, str) &
              & bind(C,name='config_get_data_length_f')
  use, intrinsic :: iso_c_binding
  implicit none
  type(c_ptr), value :: dom
  character(kind=c_char, len=1), intent(in) :: str(*)
end function c_config_get_data_length

!-------------------------------------------------------------------------------

logical(kind=c_bool) function c_config_element_exists(dom, str) &
              & bind(C,name='config_element_exists_f')
  use, intrinsic :: iso_c_binding
  implicit none
  type(c_ptr), value :: dom
  character(kind=c_char, len=1), intent(in) :: str(*)
end function c_config_element_exists

!-------------------------------------------------------------------------------

integer(kind=c_int) function c_config_get_data_as_int(dom, str) &
              & bind(C,name='config_get_data_as_int_f')
  use, intrinsic :: iso_c_binding
  implicit none
  type(c_ptr), value :: dom
  character(kind=c_char, len=1), intent(in) :: str(*)
end function c_config_get_data_as_int

!-------------------------------------------------------------------------------

real(kind=c_double) function c_config_get_data_as_double(dom, str) &
              & bind(C,name='config_get_data_as_double_f')
  use, intrinsic :: iso_c_binding
  implicit none
  type(c_ptr), value :: dom
  character(kind=c_char, len=1), intent(in) :: str(*)
end function c_config_get_data_as_double

!-------------------------------------------------------------------------------

subroutine c_config_get_data(dom, str, output) bind(C,name='config_get_data_f')
  use, intrinsic :: iso_c_binding
  implicit none
  type(c_ptr), value :: dom
  character(kind=c_char, len=1), intent(in)  :: str(*)
  character(kind=c_char, len=1), intent(out) :: output(*)
end subroutine c_config_get_data

!-------------------------------------------------------------------------------

integer(kind=c_int) function c_config_get_data_dimension(dom, str) &
              & bind(C,name='config_get_data_dimension_f')
  use, intrinsic :: iso_c_binding
  implicit none
  type(c_ptr), value :: dom
  character(kind=c_char, len=1), intent(in) :: str(*)
end function c_config_get_data_dimension

!-------------------------------------------------------------------------------

integer(kind=c_int) function c_config_get_data_element_length(dom, str, index) &
              & bind(C,name='config_get_data_element_length_f')
  use, intrinsic :: iso_c_binding
  implicit none
  type(c_ptr), value :: dom
  character(kind=c_char, len=1), intent(in) :: str(*)
  integer(kind=c_int), intent(in) :: index
end function c_config_get_data_element_length

!-------------------------------------------------------------------------------

subroutine c_config_get_data_element(dom, str, index, output) bind(C,name='config_get_data_element_f')
  use, intrinsic :: iso_c_binding
  implicit none
  type(c_ptr), value :: dom
  character(kind=c_char, len=1), intent(in)  :: str(*)
  integer(kind=c_int), intent(in) :: index
  character(kind=c_char, len=1), intent(out) :: output(*)
end subroutine c_config_get_data_element

!-------------------------------------------------------------------------------

subroutine c_config_get_double_vector(dom, str, length, vect) &
                                    & bind(C,name='config_get_double_vector_f')
  use, intrinsic :: iso_c_binding, only : c_ptr, c_char, c_size_t, c_double
  implicit none
  type(c_ptr), value :: dom
  character(kind=c_char, len=1), intent(in) :: str(*)
  integer(c_size_t), intent(in) :: length
  real(c_double), intent(inout) :: vect(length)
end subroutine c_config_get_double_vector

!-------------------------------------------------------------------------------

subroutine c_config_get_float_vector(dom, str, length, vect) &
                                   & bind(C,name='config_get_float_vector_f')
  use, intrinsic :: iso_c_binding, only : c_ptr, c_char, c_size_t, c_float
  implicit none
  type(c_ptr), value :: dom
  character(kind=c_char, len=1), intent(in) :: str(*)
  integer(c_size_t), intent(in) :: length
  real(c_float), intent(inout) :: vect(length)
end subroutine c_config_get_float_vector

!-------------------------------------------------------------------------------

subroutine c_config_get_int_vector(dom, str, length, vect) &
                                 & bind(C,name='config_get_int_vector_f')
  use, intrinsic :: iso_c_binding, only : c_ptr, c_char, c_size_t, c_int32_t
  implicit none
  type(c_ptr), value :: dom
  character(kind=c_char, len=1), intent(in) :: str(*)
  integer(c_size_t), intent(in) :: length
  integer(c_int32_t), intent(inout) :: vect(length)
end subroutine c_config_get_int_vector

!-------------------------------------------------------------------------------
end interface
!-------------------------------------------------------------------------------
