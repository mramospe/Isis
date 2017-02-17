//////////////////////////////////////////////////////////
//
//  Analysis package
//
// -------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 17/02/2017
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

#include "TTree.h"

#include <string>
#include <vector>


//_______________________________________________________________________________

namespace Analysis {

  // Appends to the given vector all the names of the branches in a tree. Returns
  // the number of branches appended.
  size_t GetBranchNames( std::vector<std::string> &vector,
			 TTree *inputTree,
			 const std::string &expr = "" );

  // Appends to the given vector all the titles of the branches in a tree. Returns
  // the number of branches appended.
  size_t GetBranchTitles( std::vector<std::string> &vector,
			  TTree *inputTree,
			  const std::string &expr = "" );

  // Gets the number of variables in a tree whose name contains a given keyword
  size_t GetNvarsWithExpr( TTree *inputTree, const std::string &expr );

  // Returns the number of variables of a given type
  size_t GetNvarsWithType( TTree *inputTree, const char &type );

  // Returns the number of variables of a certain type in a given vector and tree
  size_t GetNvarsWithTypeIn( TTree *inputTree,
			     const char &type,
			     const std::vector<std::string> &vector );

  // Gets the type of a variable in a tree
  char GetVarType( TTree *inputTree, const std::string &var );

  // Makes and saves a new tree with a given set of names changed by anothers. The
  // output tree is going to be saved in the current directory, so an output file
  // must be opened first.
  void MakeTreeChangingNames( TTree *input_tree,
			      const std::vector< std::string > &ivars,
			      const std::vector< std::string > &ovars );

  // Creates and saves a clone of the input tree where all the input variables of
  // the given type, specified by 'F' ( float ) or 'D' ( double ), are changed to
  // the other one
  TTree* MakeTreeConvertingVars( TTree *inputTree, const char &itype = 'F' );

}

#endif
