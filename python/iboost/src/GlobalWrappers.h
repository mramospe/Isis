//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 30/06/2017
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

#include "Definitions.h"
#include "InitModule.h"
#include "Messenger.h"

#include <boost/python.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/list.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/object.hpp>
#include <boost/python/object_operators.hpp>

#include <algorithm>
#include <iostream>
#include <vector>


//_______________________________________________________________________________

namespace iboost {

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
  // Transform a python list into a standard container
  template<template<class ...> class cont, class type>
  inline cont<type> boostListToStdCont( py::list &lst ) {

    size_t lgth = py::len(lst);
    
    cont<type> res(lgth);

    size_t i = 0;
    std::generate(std::begin(res), std::end(res),
		  [&i, &lst] () {

		    return extractFromIndex<type>(lst, i++);

		  });

    return res;
  }
  
  //_______________________________________________________________________________
  // Transform a numpy ndarray object into a standard container
  template<class type>
  inline type numpyArrayToStdCont( const np::ndarray &array ) {

    auto lgth = py::len(array);
    type result(lgth);
    const char data_type = iboost::DTYPE_TO_TYPE.parse(array.get_dtype());

#define I_CREATE_STDCONT_FROM_NUMPYARRAY( type, result ) {		\
      type *type ## _ptr = reinterpret_cast<type*>(array.get_data());	\
      std::copy(type ## _ptr, type ## _ptr + lgth, std::begin(result)); \
    }

    I_SWITCH_BY_DATA_TYPE(data_type, result,
			  I_CREATE_STDCONT_FROM_NUMPYARRAY,
			  
			  IError << "Unknown array data type" << IEndMsg;
			  return type();
			  );
    
    return result;

#undef I_CREATE_STDCONT_FROM_NUMPYARRAY
  }

  //_______________________________________________________________________________
  // Transform a standard container into a python list
  template<class type>
  inline py::list stdContToBoostList( const type &container ) {
    
    py::list list;
    for ( const auto &v : container )
      list.append(v);
    
    return list;
  }

  //_______________________________________________________________________________
  // Transform a standard container into a numpy ndarray
  template<template<class ...> class cont, class type>
  inline np::ndarray stdContToNumpyArray( const cont<type> &container ) {
    
    long int size = container.size();
    Py_intptr_t shape[1] = {size};

    np::ndarray result = np::zeros(1, shape, np::dtype::get_builtin<type>());
    
    std::copy(std::begin(container), std::end(container),
	      reinterpret_cast<type*>(result.get_data()));

    return result;
  }
}

#endif
