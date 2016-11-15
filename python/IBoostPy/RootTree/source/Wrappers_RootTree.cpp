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

// Some thin wrappers
py::dict ThinWrapper_BoostDictFromTree_YCT( const char *fpath,
					    const char *tpath,
					    py::object vars,
					    const char *cuts ) {
  return ib::BoostDictFromTree(fpath, tpath, vars, cuts);
}
py::dict ThinWrapper_BoostDictFromTree_NCT( const char *fpath,
					    const char *tpath,
					    py::object vars ) {
  return ib::BoostDictFromTree(fpath, tpath, vars);
}
py::list ThinWrapper_BoostListFromTree_YCT( const char *fpath,
					    const char *tpath,
					    const char *var,
					    const char *cuts ) {
  return ib::BoostListFromTree(fpath, tpath, var, cuts);
}
py::list ThinWrapper_BoostListFromTree_NCT( const char *fpath,
					    const char *tpath,
					    const char *var ) {
  return ib::BoostListFromTree(fpath, tpath, var);
}

// Definition of the python module
BOOST_PYTHON_MODULE( RootTree ) {

  py::def("TreeToDict", ThinWrapper_BoostDictFromTree_YCT );
  py::def("TreeToDict", ThinWrapper_BoostDictFromTree_NCT );
  py::def("DictToTree", py::raw_function(ib::BoostDictToTree, 2) );
  py::def("TreeToList", ThinWrapper_BoostListFromTree_YCT );
  py::def("TreeToList", ThinWrapper_BoostListFromTree_NCT );
  py::def("ListToTree", py::raw_function(ib::BoostListToTree, 2) );

}
