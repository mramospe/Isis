//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 20/09/2017
//
// -------------------------------------------------------------------------------
//
//  Description:
//
//  Configuration of the functions converting Root tree objects into python
//  objects.
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#include "GlobalWrappers.h"
#include "InitModule.h"
#include "TreeWrapper.h"

#include <boost/python.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/list.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/object.hpp>
#include <boost/python/raw_function.hpp>

namespace py = boost::python;
namespace np = boost::python::numpy;
namespace ib = iboost;


//_______________________________________________________________________________

BOOST_PYTHON_FUNCTION_OVERLOADS(treeToNumpyArray_Overloads,
				ib::treeToNumpyArray, 3, 5);

// Definition of the python module
BOOST_PYTHON_MODULE( rootio ) {

  // Initialize python and numpy (to prevent a segmentation fault)
  I_INIT_MODULE;
  
  py::def("treeToArray", ib::treeToNumpyArray, treeToNumpyArray_Overloads());
  py::def("arrayToTree", py::raw_function(ib::numpyArrayToTree, 2));
  
}
