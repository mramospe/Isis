//////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -----------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 20/09/2017
//
// -----------------------------------------------------------------------
//
//  Description:
//
//  Define a class to parse numpy array dtype objects into a character,
//  and a macro to initialize python modules.
//
// -----------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////


#ifndef INIT_MODULE
#define INIT_MODULE

#include "ValueTypeDef.h"

#include <boost/python.hpp>
#include <boost/python/numpy.hpp>

#include <Python.h>

#include <map>
#include <string>


//________________________________________________________________________

namespace iboost {

  // Abbreviate namespace
  namespace py = boost::python;
  namespace np = boost::python::numpy;
  
  // Class containing the way to translate the numpy ndarray dtype objects
  // into characters. A class must save the map since now numpy functions
  // may be called without calling "numpy::initialize" first. Since
  // "dtype" objects are sorted in a map by their size in bins (which makes
  // float32 and int32 be overlapped), strings must be used.
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
	  {this->dtypeToStr< isis::Char   >(), 'B'},
	  {this->dtypeToStr< isis::uChar  >(), 'b'},
	  {this->dtypeToStr< isis::sInt   >(), 'S'},
	  {this->dtypeToStr< isis::usInt  >(), 's'},
	  {this->dtypeToStr< isis::Int    >(), 'I'},
	  {this->dtypeToStr< isis::uInt   >(), 'i'},
	  {this->dtypeToStr< isis::Float  >(), 'F'},
	  {this->dtypeToStr< isis::Double >(), 'D'},
	  {this->dtypeToStr< isis::llInt  >(), 'L'},
	  {this->dtypeToStr< isis::ullInt >(), 'l'},
	  {this->dtypeToStr< isis::Bool   >(), 'O'}
	};
      
    }
    
    // Return the character associated to the given numpy type
    inline char parse_numpy_type( const np::dtype &type ) const {
      
      std::string str_type = this->dtypeToStr(type);
      
      return fMap.at(str_type);
    }

    // Return the dtype object associated with the given character
    inline np::dtype parse_root_type( const char &t ) const {

#define I_ROOT_TO_NUMPY_TYPE(var, ptr) return np::dtype::get_builtin<var>();
    
      I_SWITCH_BY_DATA_TYPE(t, NULL,
			    I_ROOT_TO_NUMPY_TYPE,
			    
			    return np::dtype::get_builtin<bool>()
			    );

#undef I_ROOT_TO_NUMPY_TYPE
    }
    
  protected:

    // Map with the parser information
    std::map<std::string, const char> fMap;

    // Get the string related to a given dtype object
    template<class type>
    inline std::string dtypeToStr() const {
      
      np::dtype dtype = np::dtype::get_builtin<type>();

      return this->dtypeToStr(dtype);
    }

    inline std::string dtypeToStr( const np::dtype &dtype ) const {
      
      return py::extract<std::string>(py::str(dtype));
    }
  };

  // Declare the class to parse from numpy-array dtype objects to characters
  // with the information of the value type
  extern DTypeParser NUMPY_TYPE_CONVERTER;
  
}

//________________________________________________________________________
// This is the macro which may be called to initialize a module
#define I_INIT_MODULE				\
  Py_Initialize();				\
  np::initialize();				\
  iboost::NUMPY_TYPE_CONVERTER.initialize();

#endif
