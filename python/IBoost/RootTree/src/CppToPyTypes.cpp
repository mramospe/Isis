//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 23/03/2017
//
// -------------------------------------------------------------------------------
//
//  Description:
//
//  Functions to transform c++ types in python types.
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#include "CppToPyTypes.h"

#include "Python.h"
#include <boost/python/object.hpp>

namespace py = boost::python;


//_______________________________________________________________________________
// Return a boost object from a given BufferVariable containing its value
py::object IBoost::buffVarToBoostObj( const Isis::BufferVariable &var ) {

  switch (var.GetType()) {
  case 'B':
    {
      char value;
      var.ExtractValue(value);
      return py::object(value);
    }
  case 'b':
    {
      unsigned char value;
      var.ExtractValue(value);
      return py::object(value);
    }
  case 'S':
    {
      short int value;
      var.ExtractValue(value);
      return py::object(value);
    }
  case 's':
    {
      unsigned short int value;
      var.ExtractValue(value);
      return py::object(value);
    }
  case 'I':
    {
      int value;
      var.ExtractValue(value);
      return py::object(value);
    }
  case 'i':
    {
      unsigned int value;
      var.ExtractValue(value);
      return py::object(value);
    }
  case 'F':
    {
      float value;
      var.ExtractValue(value);
      return py::object(value);
    }
  case 'D':
    {
      double value;
      var.ExtractValue(value);
      return py::object(value);
    }
  case 'L':
    {
      long long int value;
      var.ExtractValue(value);
      return py::object(value);
    }
  case 'l':
    {
      unsigned long long int value;
      var.ExtractValue(value);
      return py::object(value);
    }
  case 'O':
    {
      bool value;
      var.ExtractValue(value);
      return py::object(value);
    }
  default:
    std::cerr <<
      "ERROR: Unknown variable type < " << var.GetType() << " >"
					<< std::endl;
    return py::object();
  }
}
