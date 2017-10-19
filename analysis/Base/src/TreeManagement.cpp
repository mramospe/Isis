//////////////////////////////////////////////////////////
//
//  Analysis package
//
// -------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 19/10/2017
//
// ------------------------------------------------------
/////////////////////////////////////////////////////////


#include "Definitions.hpp"
#include "Exceptions.hpp"
#include "TreeManagement.hpp"
#include "Utils.hpp"

#include "TBranch.h"
#include "TDirectory.h"
#include "TLeaf.h"
#include "TObjArray.h"

#include <iostream>
#include <map>
#include <utility>


//_______________________________________________________________________________

namespace isis {

  //_______________________________________________________________________________
  //
  size_t getBranchNames( Strings &vector,
			 TTree *inputTree,
			 const std::string &regex
			 ) {
    
    TObjArray *brList = inputTree->GetListOfBranches();
    Strings brVector(brList->GetEntries());
    for ( int i = 0; i < brList->GetEntries(); ++i )
      brVector[i] = brList->At(i)->GetName();
    
    size_t n = vector.size();
    if ( regex.size() )
      stringVectorFilter(vector, brVector, regex);
    else
      vector.insert(vector.end(), brVector.begin(), brVector.end());
  
    return vector.size() - n;
  }

  //_______________________________________________________________________________
  //
  size_t getBranchTitles( Strings &vector,
			  TTree *inputTree,
			  const std::string &regex ) {
  
    TObjArray *brList = inputTree->GetListOfBranches();
    Strings brVector(brList->GetEntries());
    for ( int i = 0; i < brList->GetEntries(); ++i )
      brVector[i] = brList->At(i)->GetTitle();

    size_t n = vector.size();
    if ( regex.size() )
      stringVectorFilter(vector, brVector, regex);
    else
      vector.insert(vector.end(), brVector.begin(), brVector.end());

    return vector.size() - n;
  }

  //_______________________________________________________________________________
  //
  size_t getNvarsWithExpr( TTree *inputTree, const std::string &regex ) {
  
    Strings vector;
    getBranchNames(vector, inputTree, regex);
    return vector.size();
  }

  //_______________________________________________________________________________
  //
  size_t getNvarsWithType( TTree *inputTree, const char &type ) {
  
    TObjArray *brList = inputTree->GetListOfBranches();
    std::string title;
    size_t      counter(0);
    for ( int i = 0; i < brList->GetEntries(); ++i ) {
      title = brList->At(i)->GetTitle();
      if ( title.back() == type )
	++counter;
    }
    return counter;
  }

  //_______________________________________________________________________________
  //
  size_t getNvarsWithTypeIn( TTree *inputTree,
			     const char &type,
			     const Strings &vector ) {
  
    TObjArray *brList = inputTree->GetListOfBranches();
    std::string title;
    size_t      counter(0);
    for ( auto it = vector.begin(); it != vector.end(); ++it ) {
      title = brList->FindObject(it->c_str())->GetTitle();
      if ( title.back() == type )
	++counter;
    }
    return counter;
  }

  //_______________________________________________________________________________
  //
  char getVarType( TTree *inputTree, const std::string &var ) {
  
    TObjArray *brList = inputTree->GetListOfBranches();
    TObject *obj = brList->FindObject(var.c_str());
    
    if ( !obj )
      throw NotFound("branch", var);
    
    std::string type = obj->GetTitle();
    type.replace(0, var.length(), "");
    
    return type[1];
  }

}
