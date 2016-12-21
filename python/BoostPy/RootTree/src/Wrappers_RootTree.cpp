//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 10/11/2016
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

BOOST_PYTHON_FUNCTION_OVERLOADS(BoostDictFromTree_Overloads,
				ib::BoostDictFromTree, 3, 4);

BOOST_PYTHON_FUNCTION_OVERLOADS(BoostListFromTree_Overloads,
				ib::BoostListFromTree, 3, 4);

// Definition of the python module
BOOST_PYTHON_MODULE( RootTree ) {

  py::def("DictFromTree", ib::BoostDictFromTree, BoostDictFromTree_Overloads());
  py::def("TreeFromDict", py::raw_function(ib::BoostDictToTree, 2));
  py::def("ListFromTree", ib::BoostListFromTree, BoostListFromTree_Overloads());
  py::def("TreeFromList", py::raw_function(ib::BoostListToTree, 2));
  
}
