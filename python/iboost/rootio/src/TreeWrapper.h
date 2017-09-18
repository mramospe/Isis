//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 18/09/2017
//
// -------------------------------------------------------------------------------
//
//  Description:
//
//  Functions to convert python dictionaries and lists onn branches in a Root
//  tree. The output is saved on the current directory.
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#ifndef TREE_WRAPPER
#define TREE_WRAPPER

#include <boost/python/dict.hpp>
#include <boost/python/list.hpp>
#include <boost/python/tuple.hpp>

#include "BuffVarWriter.h"
#include "GlobalWrappers.h"

#include "BufferVariable.h"
#include "Definitions.h"
#include "ValueTypeDef.h"

#include "TBranch.h"
#include "TFile.h"
#include "TTree.h"

#include <iostream>
#include <string>
#include <vector>


//_______________________________________________________________________________

namespace iboost {
  
  // Store in a dictionary the lists with the values for each of the given
  // variables stored in a Root tree. A set of cuts can be specified.
  py::dict treeToBoostDict( std::string fpath,
			    std::string tpath,
			    py::object &vars,
			    std::string cuts = std::string(),
			    bool use_regex = false );
  
  // Write a python dictionary to a Root tree. Since in python there are only four
  // numeric types: bool, int, long and float; only the associated c++ types
  // are used.
  py::object boostDictToTree( py::tuple args,
			      py::dict kwargs );
  

  // Store in a list the values for a variable present in a Root tree
  np::ndarray treeToNumpyArray( std::string fpath,
				std::string tpath,
				std::string var,
				std::string cuts = std::string() );
  
  // Create/update a Root tree adding a variable with the given list of values. To
  // see the possible < kwargs > arguments see < BoostDictToTree >.
  py::object numpyArrayToTree( py::tuple args, py::dict kwargs );

}
  
#endif
