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
//  Functions to convert python dictionaries and lists onn branches in a Root
//  tree. The output is saved on the current directory.
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#ifndef TREE_WRAPPER_H
#define TREE_WRAPPER_H

#include "BuffVarWriter.hpp"
#include "GlobalWrappers.hpp"

#include <boost/python/dict.hpp>
#include <boost/python/list.hpp>
#include <boost/python/tuple.hpp>

#include "BufferVariable.hpp"
#include "Definitions.hpp"
#include "ValueTypeDef.hpp"

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
  np::ndarray treeToNumpyArray( std::string fpath,
				std::string tpath,
				py::object &vars,
				std::string cuts = std::string(),
				bool use_regex = false );
  
  // Write a python dictionary to a Root tree. Since in python there are only four
  // numeric types: bool, int, long and float; only the associated c++ types
  // are used.
  py::object numpyArrayToTree( py::tuple args,
			       py::dict kwargs );
  
}
  
#endif
