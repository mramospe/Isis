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
namespace ib = IBoost;


//_______________________________________________________________________________

BOOST_PYTHON_FUNCTION_OVERLOADS(treeToBoostDict_Overloads,
				ib::treeToBoostDict, 3, 4);

BOOST_PYTHON_FUNCTION_OVERLOADS(treeToBoostList_Overloads,
				ib::treeToBoostList, 3, 4);

// Definition of the python module
BOOST_PYTHON_MODULE( RootTree ) {
  
  py::def("treeToDict", ib::treeToBoostDict, treeToBoostDict_Overloads());
  py::def("dictToTree", py::raw_function(ib::boostDictToTree, 2));
  py::def("treeToList", ib::treeToBoostList, treeToBoostList_Overloads());
  py::def("listToTree", py::raw_function(ib::boostListToTree, 2));
  
}
