//////////////////////////////////////////////////////////
//                                                      //
//  Analysis package                                    //
//                                                      //
// ---------------------------------------------------- //
//                                                      //
//  AUTHOR: Miguel Ramos Pernas                         //
//  e-mail: miguel.ramos.pernas@cern.ch                 //
//                                                      //
//  Last update: 30/09/2015                             //
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

  std::vector<std::string> GetAllVarsWith( TTree *inputTree, std::string kw );
  std::vector<std::string> GetBranchNames( TTree *inputTree );
  std::vector<std::string> GetBranchTitles( TTree *inputTree );
  std::string              GetVarType( TTree *inputTree, std::string var );
  size_t                   GetNvarsWithString( TTree *inputTree, std::string str );
  size_t                   GetNvarsWithType( TTree *inputTree, std::string type );
  size_t                   GetNvarsWithTypeIn( TTree *inputTree,
					       std::string type,
					       std::vector<std::string> &vector );
  void                     MakeTreeChangingNames( TTree *input_tree,
						  std::vector< std::string > ivars,
						  std::vector< std::string > ovars );

}

#endif
