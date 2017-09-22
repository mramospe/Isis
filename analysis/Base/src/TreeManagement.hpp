//////////////////////////////////////////////////////////
//
//  Analysis package
//
// -------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 18/09/2017
//
// -------------------------------------------------------
//
//  Description:
//
//  Here are defined different tools to manage TTree
//  objects.
//
// ------------------------------------------------------
/////////////////////////////////////////////////////////


#ifndef TREE_MANAGEMENT
#define TREE_MANAGEMENT

#include "Definitions.hpp"

#include "TTree.h"

#include <string>
#include <vector>


//_______________________________________________________________________________

namespace isis {

  // Appends to the given vector all the names of the branches in a tree. Returns
  // the number of branches appended.
  size_t getBranchNames( Strings &vector,
			 TTree *inputTree,
			 const std::string &regex = ""
			 );

  // Appends to the given vector all the titles of the branches in a tree. Returns
  // the number of branches appended.
  size_t getBranchTitles( Strings &vector,
			  TTree *inputTree,
			  const std::string &regex = ""
			  );

  // Gets the number of variables in a tree whose name contains a given keyword
  size_t getNvarsWithExpr( TTree *inputTree, const std::string &regex );

  // Returns the number of variables of a given type
  size_t getNvarsWithType( TTree *inputTree, const char &type );

  // Returns the number of variables of a certain type in a given vector and tree
  size_t getNvarsWithTypeIn( TTree *inputTree,
			     const char &type,
			     const Strings &vector );

  // Gets the type of a variable in a tree
  char getVarType( TTree *inputTree, const std::string &var );

}

#endif
