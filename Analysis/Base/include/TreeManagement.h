//////////////////////////////////////////////////////////
//
//  Analysis package
//
// -------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 10/03/2016
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

  void   GetBranchNames( std::vector<std::string> &vector,
			 TTree *inputTree,
			 const std::string &expr = "" );
  void   GetBranchTitles( std::vector<std::string> &vector,
			  TTree *inputTree,
			  const std::string &expr = "" );
  size_t GetNvarsWithExpr( TTree *inputTree, const std::string &expr );
  size_t GetNvarsWithType( TTree *inputTree, const char &type );
  size_t GetNvarsWithTypeIn( TTree *inputTree,
			     const char &type,
			     const std::vector<std::string> &vector );
  char   GetVarType( TTree *inputTree, const std::string &var );
  void   MakeTreeChangingNames( TTree *input_tree,
				const std::vector< std::string > &ivars,
				const std::vector< std::string > &ovars );
  TTree* MakeTreeConvertingVars( TTree *inputTree, const char &itype = 'F' );

}

#endif
