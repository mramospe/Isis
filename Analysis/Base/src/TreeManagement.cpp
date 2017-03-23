//////////////////////////////////////////////////////////
//
//  Analysis package
//
// -------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 23/03/2017
//
// ------------------------------------------------------
/////////////////////////////////////////////////////////


#include "Definitions.h"
#include "Messenger.h"
#include "TreeManagement.h"
#include "Utils.h"

#include "TBranch.h"
#include "TDirectory.h"
#include "TLeaf.h"
#include "TObjArray.h"

#include <iostream>
#include <map>
#include <utility>


//_______________________________________________________________________________

namespace Isis {

  //_______________________________________________________________________________
  //
  size_t getBranchNames( Strings &vector,
			 TTree *inputTree,
			 const std::string &expr ) {
  
    TObjArray *brList = inputTree->GetListOfBranches();
    Strings brVector( brList->GetEntries() );
    for ( int ibr = 0; ibr < brList->GetEntries(); ++ibr )
      brVector[ ibr ] = brList->At( ibr )->GetName();
  
    size_t n = vector.size();
    if ( expr.size() )
      stringVectorFilter( vector, brVector, expr );
    else
      vector.insert( vector.end(), brVector.begin(), brVector.end() );
  
    return vector.size() - n;
  }

  //_______________________________________________________________________________
  //
  size_t getBranchTitles( Strings &vector,
			  TTree *inputTree,
			  const std::string &expr ) {
  
    TObjArray *brList = inputTree->GetListOfBranches();
    Strings brVector( brList->GetEntries() );
    for ( int ibr = 0; ibr < brList->GetEntries(); ++ibr )
      brVector[ ibr ] = brList->At( ibr )->GetTitle();

    size_t n = vector.size();
    if ( expr.size() )
      stringVectorFilter( vector, brVector, expr );
    else
      vector.insert( vector.end(), brVector.begin(), brVector.end() );

    return vector.size() - n;
  }

  //_______________________________________________________________________________
  //
  size_t getNvarsWithExpr( TTree *inputTree, const std::string &expr ) {
  
    Strings vector;
    getBranchNames( vector, inputTree, expr );
    return vector.size();
  }

  //_______________________________________________________________________________
  //
  size_t getNvarsWithType( TTree *inputTree, const char &type ) {
  
    TObjArray *brList = inputTree->GetListOfBranches();
    std::string title;
    size_t      counter( 0 );
    for ( int ibr = 0; ibr < brList->GetEntries(); ++ibr ) {
      title = brList->At( ibr )->GetTitle();
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
    size_t      counter( 0 );
    for ( auto it = vector.begin(); it != vector.end(); ++it ) {
      title = brList->FindObject( it->c_str() )->GetTitle();
      if ( title.back() == type )
	++counter;
    }
    return counter;
  }

  //_______________________________________________________________________________
  //
  char getVarType( TTree *inputTree, const std::string &var ) {
  
    TObjArray *brList = inputTree->GetListOfBranches();
    TObject *obj = brList->FindObject( var.c_str() );
    
    if ( !obj )
      IError << "Unable to get branch with name < " << var << " >" << IEndMsg;
    
    std::string type = obj->GetTitle();
    type.replace( 0, var.length(), "" );
    
    return type[ 1 ];
  }

}
