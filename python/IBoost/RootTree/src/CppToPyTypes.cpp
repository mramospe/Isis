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
#include "ValueTypeDef.h"

#include "Python.h"
#include <boost/python/object.hpp>


//_______________________________________________________________________________

namespace IBoost {

  //_______________________________________________________________________________
  //
  py::object buffVarToBoostObj( const Isis::BufferVariable &var ) {

    const char tp = var.getType();
    
    I_SWITCH_BY_DATA_TYPE(tp, var, I_BUFFVAR_TO_BOOST_OBJ_ACTION,
			  
			  IError <<
			  "Unknown variable type < " << tp << " >"
			  << IEndMsg;
			  return py::object();
			  );
  }

}
