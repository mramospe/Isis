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

  py::def("TreeToDict", ib::BoostDictFromTree, BoostDictFromTree_Overloads());
  py::def("DictToTree", py::raw_function(ib::BoostDictToTree, 2));
  py::def("TreeToList", ib::BoostListFromTree, BoostListFromTree_Overloads());
  py::def("ListToTree", py::raw_function(ib::BoostListToTree, 2));

}
