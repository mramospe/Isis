///////////////////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 21/03/2017
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#include "Definitions.h"
#include "TreeBuffer.h"
#include "TreeManagement.h"
#include "Utils.h"


//_______________________________________________________________________________

namespace Isis {

  //_______________________________________________________________________________
  //
  TreeBuffer::TreeBuffer() : BufferArray() { }

  //_______________________________________________________________________________
  //
  TreeBuffer::TreeBuffer( TTree *tree, SortFunc func ) :
    BufferArray( func ), fTree( tree ) { }

  //_______________________________________________________________________________
  //
  TreeBuffer::~TreeBuffer() { }

  //_______________________________________________________________________________
  //
  void TreeBuffer::AttachTree( TTree *tree ) {

    for ( auto it = fVarMap.begin(); it != fVarMap.end(); ++it )
      delete it -> second;
  
    fVarMap.clear();

    fTree = tree;
  }

  //_______________________________________________________________________________
  //
  BufferVariable* TreeBuffer::CreateVariable( const std::string &name, const char &type ) {

    BufferVariable *var = this -> AddVariable( name, type );
    void *path      = var -> PathToValue();
    TBranch *branch = fTree -> Branch( name.c_str(), path, (name + '/' + type).c_str() );
    fBranchVector.push_back( branch );
    return var;
  }

  //_______________________________________________________________________________
  //
  void TreeBuffer::Load( const std::string &expr ) {
  
    Strings variables;
    GetBranchNames( variables, fTree, expr );

    for ( auto it = variables.begin(); it != variables.end(); ++it )
      this -> LoadVariable( *it );
  }

  //_______________________________________________________________________________
  //
  BufferVariable* TreeBuffer::LoadVariable( const std::string &name ) {

    BufferVariable *var = this ->
      AddVariable( name, GetVarType( fTree, name ) );
    void *path = var -> PathToValue();
    fTree -> SetBranchAddress( name.c_str(), path );
    return var;
  }

  //_______________________________________________________________________________
  //
  void TreeBuffer::SetBranchStatus( const bool &dec ) {
    if ( dec )
      fTree -> SetBranchStatus( "*", dec );
    else {
      fTree -> SetBranchStatus( "*", false );
      for ( auto it = fVarMap.begin(); it != fVarMap.end(); ++it ) {
	const char *name = it -> first.c_str();
	if ( fTree -> GetLeaf( name ) )
	  fTree -> SetBranchStatus( name, true );
      }
    }
  }

}
