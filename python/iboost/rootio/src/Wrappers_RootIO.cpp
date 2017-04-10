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
//  Configuration of the functions converting Root tree objects into python
//  objects.
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#include "TreeWrapper.h"

#include <boost/python.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/list.hpp>
#include <boost/python/object.hpp>
#include <boost/python/raw_function.hpp>

namespace py = boost::python;
namespace ib = iboost;


//_______________________________________________________________________________

BOOST_PYTHON_FUNCTION_OVERLOADS(treeToBoostDict_Overloads,
				ib::treeToBoostDict, 3, 4);

BOOST_PYTHON_FUNCTION_OVERLOADS(treeToNumpyArray_Overloads,
				ib::treeToNumpyArray, 3, 4);

// Definition of the python module
BOOST_PYTHON_MODULE( rootio ) {
  
  py::def("treeToDict", ib::treeToBoostDict, treeToBoostDict_Overloads());
  py::def("dictToTree", py::raw_function(ib::boostDictToTree, 2));
  py::def("treeToList", ib::treeToNumpyArray, treeToNumpyArray_Overloads());
  py::def("listToTree", py::raw_function(ib::numpyArrayToTree, 2));
  
}
