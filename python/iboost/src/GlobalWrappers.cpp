//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 10/04/2017
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#include "GlobalWrappers.h"

#include <boost/python/dict.hpp>
#include <boost/python/list.hpp>
#include <boost/python/object.hpp>

#include "Messenger.h"
#include "ValueTypeDef.h"

#include <iostream>
#include <map>

namespace py = boost::python;


//_______________________________________________________________________________

namespace iboost {

  //_______________________________________________________________________________
  // Declaration follows "ValueTypeDef.h"
  extern const std::map<np::dtype, const char> DTypeMap = {
    {np::dtype::get_builtin< isis::Char   >(), 'B'},
    {np::dtype::get_builtin< isis::uChar  >(), 'b'},
    {np::dtype::get_builtin< isis::sInt   >(), 'S'},
    {np::dtype::get_builtin< isis::usInt  >(), 's'},
    {np::dtype::get_builtin< isis::Int    >(), 'I'},
    {np::dtype::get_builtin< isis::uInt   >(), 'i'},
    {np::dtype::get_builtin< isis::Float  >(), 'F'},
    {np::dtype::get_builtin< isis::Double >(), 'D'},
    {np::dtype::get_builtin< isis::llInt  >(), 'L'},
    {np::dtype::get_builtin< isis::ullInt >(), 'l'},
    {np::dtype::get_builtin< isis::Bool   >(), 'O'}
  };
  
  //_______________________________________________________________________________
  //
  py::ssize_t boostDictListSize( py::dict dict ) {

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
  //
  void checkArgs( py::tuple &args,
		  const unsigned int &nargs ) {
  
    py::ssize_t n = py::len( args );
    if ( n != nargs )
      IError << "Incorrect number of arguments " << n << " (" << nargs << ")" << IEndMsg;
  }

  //_______________________________________________________________________________
  //
  void checkKwargs( py::dict &kwargs,
		    const std::vector<const char*> &lst ) {
  
    py::object keys = kwargs.keys();
    for ( int i = 0; i < py::len( keys ); ++i ) {

      py::object arg = extractFromIndex<py::object>(keys, i);
    
      const char *name = py::extract<const char*>( arg );
    
      if ( std::find(lst.begin(), lst.end(), name) != lst.end() )
	IWarning << "Unknown input keyword argument < " << name << " >" << IEndMsg;
    }
  }

}
