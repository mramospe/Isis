//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 15/11/2016
//
// -------------------------------------------------------------------------------
//
//  Description:
//
//  General functions to help on the generation of python wrappers.
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#include "GlobalWrappers.h"

#include <boost/python/dict.hpp>
#include <boost/python/list.hpp>
#include <boost/python/object.hpp>

#include <iostream>

namespace py = boost::python;


//_______________________________________________________________________________
// Get the size of the lists inside the dictionary. A check is done to see that
// they match.
py::ssize_t IBoost::BoostDictListSize( py::dict dict ) {

  py::list keys  = dict.keys();
  py::ssize_t nk = py::len( keys );

  py::ssize_t length = py::len( dict[ keys[ 0 ] ] );
  for ( ssize_t i = 1; i < nk; ++i ) {
    py::object list  = dict[ keys[ i ] ];
    if ( py::len( list ) != length ) {
      std::cerr << "ERROR: Lists in dictionary do not have the same length" << std::endl;
      return 0;
    }
  }
  
  return length;
}

//_______________________________________________________________________________
// Check number of arguments foor a function/method
void IBoost::CheckArgs( boost::python::tuple &args,
			const unsigned int &nargs ) {
  boost::python::ssize_t n = boost::python::len( args );
  if ( n != nargs )
    std::cerr << "ERROR: Incorrect number of arguments " << n
	      << " (" << nargs << ")" << std::endl;
}

//_______________________________________________________________________________
// Check the keyword arguments for a function/method
void IBoost::CheckKwargs( boost::python::dict &kwargs,
			  const std::vector<const char*> &lst ) {
  
  boost::python::object keys = kwargs.keys();
  for ( int i = 0; i < boost::python::len( keys ); ++i ) {

    boost::python::object arg =
      IBoost::ExtractFromIndex<boost::python::object>(keys, i);
    
    const char *name = boost::python::extract<const char*>( arg );
    
    if ( std::find(lst.begin(), lst.end(), name) != lst.end() )
      std::cout << "WARNING: Unknown input keyword argument < "
		<< name << " >" << std::endl;
  }
}
