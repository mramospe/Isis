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
//
//  Description:
//
//  Define a class to parse numpy array dtype objects into a character, and a
//  macro to initialize python modules.
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#ifndef INIT_MODULE
#define INIT_MODULE

#include "ValueTypeDef.h"

#include <boost/python.hpp>
#include <boost/python/numpy.hpp>

#include <Python.h>

#include <map>


//_______________________________________________________________________________

namespace iboost {

  // Abbreviate namespace
  namespace py = boost::python;
  namespace np = boost::python::numpy;
  
  // Class containing the way to translate the numpy ndarray dtype objects
  // into characters. A class must save the map since now numpy functions
  // may be called without calling "numpy::initialize" first.
  class DTypeParser {

  public:

    // Constructor
    DTypeParser() { }

    // Destructor
    ~DTypeParser() { }

    // Initialize the class
    inline void initialize() {

      if ( !fMap.size() )
	fMap = {
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
      
    }

    // Return the character associated to the given numpy type
    inline char parse( const np::dtype &type ) { return fMap.at(type); }
    
  protected:

    // Map with the parser information
    std::map<np::dtype, const char> fMap;
    
  };

  // Declare the class to parse from numpy-array dtype objects to characters
  // with the information of the value type
  extern DTypeParser DTYPE_TO_TYPE;
  
}

//_______________________________________________________________________________
// This is the macro which may be called to initialize a module
#define I_INIT_MODULE				\
  Py_Initialize();				\
  np::initialize();				\
  iboost::DTYPE_TO_TYPE.initialize();

#endif
