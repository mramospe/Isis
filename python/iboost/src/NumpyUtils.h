////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 20/09/2017
//
// -------------------------------------------------------------------------
//
//  Description:
//
//  Define functions to manage numpy instances and types
//
// -------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////

#ifndef NUMPY_CONVERTERS_H
#define NUMPY_CONVERTERS_H

#include <Python.h>
#include <numpy/ndarrayobject.h>
#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/numpy/dtype.hpp>
#include <boost/python/numpy/ndarray.hpp>
#include <boost/python/detail/raw_pyobject.hpp>

#include "ValueTypeDef.h"
#include "TreeBuffer.h"

#include <string>


namespace {
  namespace np = boost::python::numpy;
  namespace dt = boost::python::detail;
}


//_______________________________________________________________________

namespace iboost {

  //_______________________________________________________________________
  // Return the keys of the given structured array
  inline py::list struct_array_keys( const np::ndarray &arr ) {

    py::list keys;
    
    auto ptr = reinterpret_cast<PyArray_Descr*>(arr.get_dtype().ptr());
    py::tuple tp(dt::borrowed_reference(ptr->names));
    size_t n = py::len(tp);
    for ( size_t i = 0; i < n; ++i )
      keys.append(extractFromIndex<std::string>(tp, i));
    
    return keys;
  }

  //_______________________________________________________________________
  // Return a void (zero filled) numpy array given the size and a tree
  // buffer
  inline np::ndarray void_ndarray( const int &n,
				   const isis::TreeBuffer &buffer ) {

    py::list dtypes;
    for ( const auto &el : buffer.getMap() ) {
      auto t = NUMPY_TYPE_CONVERTER.parse_root_type(el.second->getType());
      dtypes.append(py::make_tuple(el.first, t));
    }
    
    Py_intptr_t shape[1] = {n};
    np::dtype tp{dtypes};
    
    return np::zeros(1, shape, tp);
  }
}

#endif
