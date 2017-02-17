///////////////////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 17/02/2017
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#include "TreeBuffer.h"
#include "TreeManagement.h"
#include "Utils.h"


//_______________________________________________________________________________
//
Analysis::TreeBuffer::TreeBuffer() : General::BufferArray() { }

//_______________________________________________________________________________
//
Analysis::TreeBuffer::TreeBuffer( TTree *tree, SortFunc func ) :
  BufferArray( func ), fTree( tree ) { }

//_______________________________________________________________________________
//
Analysis::TreeBuffer::~TreeBuffer() { }

//_______________________________________________________________________________
//
void Analysis::TreeBuffer::AttachTree( TTree *tree ) {

  for ( auto it = fVarMap.begin(); it != fVarMap.end(); ++it )
    delete it -> second;
  
  fVarMap.clear();

  fTree = tree;
}

//_______________________________________________________________________________
//
General::BufferVariable* Analysis::TreeBuffer::CreateVariable( const std::string &name, const char &type ) {

  General::BufferVariable *var = this -> AddVariable( name, type );
  void *path      = var -> PathToValue();
  TBranch *branch = fTree -> Branch( name.c_str(), path, (name + '/' + type).c_str() );
  fBranchVector.push_back( branch );
  return var;
}

//_______________________________________________________________________________
//
void Analysis::TreeBuffer::Load( const std::string &expr ) {
  
  std::vector<std::string> variables;
  Analysis::GetBranchNames( variables, fTree, expr );

  for ( auto it = variables.begin(); it != variables.end(); ++it )
    this -> LoadVariable( *it );
}

//_______________________________________________________________________________
//
General::BufferVariable* Analysis::TreeBuffer::LoadVariable( const std::string &name ) {

  General::BufferVariable *var = this ->
    AddVariable( name, Analysis::GetVarType( fTree, name ) );
  void *path = var -> PathToValue();
  fTree -> SetBranchAddress( name.c_str(), path );
  return var;
}

//_______________________________________________________________________________
//
void Analysis::TreeBuffer::SetBranchStatus( const bool &dec ) {
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
