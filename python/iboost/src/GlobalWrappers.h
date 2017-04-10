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
//  General functions to help on the generation of python wrappers.
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#ifndef GLOBAL_WRAPPERS
#define GLOBAL_WRAPPERS

#include "Messenger.h"
#include "ValueTypeDef.h"

#include <boost/python/dict.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/list.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/object.hpp>
#include <boost/python/object_operators.hpp>

#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <map>
#include <vector>


//_______________________________________________________________________________

namespace iboost {

  // Abbreviate namespace
  namespace py = boost::python;
  namespace np = boost::python::numpy;

  // Map containing the way to translate the numpy ndarray dtype objects
  // into characters
  extern const std::map<np::dtype, const char> DTypeMap;

  // Get the size of the lists inside the dictionary. A check is done to see that
  // they match.
  py::ssize_t boostDictListSize( py::dict dict );

  // Check number of arguments foor a function/method
  void checkArgs( py::tuple &args, const unsigned int &nargs );

  // Check the keyword arguments for a function/method
  void checkKwargs( py::dict &kwargs, const std::vector<const char*> &lst );

  //_______________________________________________________________________________
  // Extract the converted value from a python container at the given index
  template<class type, class idxtype>
  inline type extractFromIndex( py::object &obj, idxtype index ) {
    
    py::api::object_item elem = obj[ index ];
    type output = py::extract<type>( elem );
    
    return output;
  }

  //_______________________________________________________________________________
  // Transform a python list into a standard vector
  template<class type>
  inline std::vector<type> boostListToStdVec( py::list &lst ) {

    size_t lgth = py::len( lst );
    std::vector<type> res( lgth );

    size_t i = 0;
    for ( auto it = res.begin(); it != res.end(); ++it, ++i )
      (*it) = extractFromIndex<type>(lst, i);

    return res;
  }
  
  //_______________________________________________________________________________
  // Transform a numpy ndarray object into a standard vector
  template<class type>
  inline std::vector<type> numpyArrayToStdVec( const np::ndarray &array ) {

    auto lgth = py::len(array);
    std::vector<type> result(lgth);

#define I_CREATE_STDVEC_FROM_NUMPYARRAY( type, result ) {		\
    type *type ## _ptr = reinterpret_cast<type*>(array.get_data());	\
    std::copy(type ## _ptr, type ## _ptr + lgth, result.begin());	\
  }
    
    I_SWITCH_BY_DATA_TYPE(DTypeMap.at(array.get_dtype()), result,
			  I_CREATE_STDVEC_FROM_NUMPYARRAY,

			  IError << "Unknown data type" << IEndMsg;
			  return std::vector<type>();
			  );

    return result;

#undef I_CREATE_STDVEC_FROM_NUMPYARRAY
  }

  //_______________________________________________________________________________
  // Transform a standard vector into a python list
  template<class type>
  inline py::list stdVecToBoostList( const std::vector<type> &vector ) {
    
    py::list list;
    for ( const auto &v : vector )
      list.append(v);
    
    return list;
  }

  //_______________________________________________________________________________
  // Transform a standard vector to a numpy ndarray
  template<class type>
  inline np::ndarray stdVecToNumpyArray( const std::vector<type> &vector ) {
    
    long int size = vector.size();
    Py_intptr_t shape[1] = {size};

    np::ndarray result = np::zeros(1, shape, np::dtype::get_builtin<type>());
    
    std::copy(vector.begin(), vector.end(), reinterpret_cast<type*>(result.get_data()));

    return result;
  }
}

#endif
