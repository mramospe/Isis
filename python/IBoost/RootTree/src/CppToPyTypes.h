//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 21/03/2017
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


//_______________________________________________________________________________

namespace IBoost {

  boost::python::object BoostObjectFromBufferVariable( const Isis::BufferVariable &var );

}

#endif
