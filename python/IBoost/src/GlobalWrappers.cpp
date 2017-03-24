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
//  General functions to help on the generation of python wrappers.
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#include "Messenger.h"
#include "GlobalWrappers.h"

#include <boost/python/dict.hpp>
#include <boost/python/list.hpp>
#include <boost/python/object.hpp>

#include <iostream>

namespace py = boost::python;


//_______________________________________________________________________________
// Get the size of the lists inside the dictionary. A check is done to see that
// they match.
py::ssize_t IBoost::boostDictListSize( py::dict dict ) {

  py::list keys  = dict.keys();
  py::ssize_t nk = py::len( keys );

  py::ssize_t length = py::len( dict[ keys[ 0 ] ] );
  for ( ssize_t i = 1; i < nk; ++i ) {
    
    py::object list  = dict[ keys[ i ] ];
    
    if ( py::len( list ) != length ) {
      IError << "Lists in dictionary do not have the same length" << IEndMsg;
      return 0;
    }
  }
  
  return length;
}

//_______________________________________________________________________________
// Check number of arguments foor a function/method
void IBoost::checkArgs( py::tuple &args,
			const unsigned int &nargs ) {
  
  py::ssize_t n = py::len( args );
  if ( n != nargs )
    IError << "Incorrect number of arguments " << n << " (" << nargs << ")" << IEndMsg;
}

//_______________________________________________________________________________
// Check the keyword arguments for a function/method
void IBoost::checkKwargs( py::dict &kwargs,
			  const std::vector<const char*> &lst ) {
  
  py::object keys = kwargs.keys();
  for ( int i = 0; i < py::len( keys ); ++i ) {

    py::object arg = IBoost::extractFromIndex<py::object>(keys, i);
    
    const char *name = py::extract<const char*>( arg );
    
    if ( std::find(lst.begin(), lst.end(), name) != lst.end() )
      IWarning << "Unknown input keyword argument < " << name << " >" << IEndMsg;
  }
}
