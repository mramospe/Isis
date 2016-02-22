//////////////////////////////////////////////////////////
//                                                      //
//  Analysis package                                    //
//                                                      //
// ---------------------------------------------------- //
//                                                      //
//  AUTHOR: Miguel Ramos Pernas                         //
//  e-mail: miguel.ramos.pernas@cern.ch                 //
//                                                      //
//  Last update: 22/02/2016                             //
//                                                      //
// ---------------------------------------------------- //
//                                                      //
//  Description:                                        //
//                                                      //
//  Here are defined different tools to manage TTree    //
//  objects.                                            //
//                                                      //
// ---------------------------------------------------- //
/////////////////////////////////////////////////////////


#ifndef TREE_MANAGEMENT
#define TREE_MANAGEMENT

#include "TTree.h"

#include <string>
#include <vector>


//_______________________________________________________________________________

namespace Analysis {

  std::vector<std::string> GetAllVarsWith( TTree *inputTree, const std::string &kw );
  std::vector<std::string> GetBranchNames( TTree *inputTree );
  std::vector<std::string> GetBranchTitles( TTree *inputTree );
  std::string              GetVarType( TTree *inputTree, const std::string &var );
  size_t                   GetNvarsWithString( TTree *inputTree, const std::string &kw );
  size_t                   GetNvarsWithType( TTree *inputTree, std::string type );
  size_t                   GetNvarsWithTypeIn( TTree *inputTree,
					       std::string type,
					       const std::vector<std::string> &vector );
  void                     MakeTreeChangingNames( TTree *input_tree,
						  const std::vector< std::string > &ivars,
						  const std::vector< std::string > &ovars );
  TTree*                   MakeTreeConvertingVars( TTree *inputTree, const char &itype = 'F' );

}

#endif
