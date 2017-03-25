//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 25/03/2017
//
// -------------------------------------------------------------------------------
//
//  Description:
//
//  Functions to transform c++ types in python types.
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#ifndef CPP_TO_PY_TYPES
#define CPP_TO_PY_TYPES

#include <boost/python/object.hpp>

#include "BufferVariable.h"

#include "GlobalWrappers.h"


//_______________________________________________________________________________
// Define the action to be done for each possible value type. New token
// definition is necessary since it will be executed on a "switch" statement.
#define I_BUFFVAR_TO_BOOST_OBJ_ACTION( type, var )   \
  type type ## _value;				     \
  var.extractValue(type ## _value);		     \
  return py::object(type ## _value);

//_______________________________________________________________________________

namespace IBoost {

  // Return a boost object from a given BufferVariable containing its value
  py::object buffVarToBoostObj( const Isis::BufferVariable &var );

}

#endif
