///////////////////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 10/04/2017
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#include "Definitions.hpp"
#include "TreeBuffer.hpp"
#include "TreeManagement.hpp"
#include "Utils.hpp"


//_______________________________________________________________________________

namespace isis {

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
  void TreeBuffer::attachTree( TTree *tree ) {

    for ( auto it = fVarMap.begin(); it != fVarMap.end(); ++it )
      delete it->second;
  
    fVarMap.clear();

    fTree = tree;
  }

  //_______________________________________________________________________________
  //
  BufferVariable* TreeBuffer::createVariable( const std::string &name, const char &type ) {

    BufferVariable *var = this->addVariable( name, type );
    void *path      = var->pathToValue();
    TBranch *branch = fTree->Branch( name.c_str(), path, (name + '/' + type).c_str() );
    fBranchVector.push_back( branch );
    
    return var;
  }

  //_______________________________________________________________________________
  //
  void TreeBuffer::load( const std::string &expr ) {
  
    Strings variables;
    getBranchNames( variables, fTree, expr );

    for ( auto it = variables.begin(); it != variables.end(); ++it )
      this->loadVariable( *it );
  }

  //_______________________________________________________________________________
  //
  BufferVariable* TreeBuffer::loadVariable( const std::string &name ) {

    BufferVariable *var = this ->
      addVariable( name, getVarType( fTree, name ) );
    void *path = var->pathToValue();
    fTree->SetBranchAddress( name.c_str(), path );
    
    return var;
  }

  //_______________________________________________________________________________
  //
  void TreeBuffer::setBranchStatus( const bool &dec ) {
    
    if ( dec )
      fTree->SetBranchStatus( "*", dec );
    else {
      fTree->SetBranchStatus( "*", false );
      
      for ( auto it = fVarMap.begin(); it != fVarMap.end(); ++it ) {
	
	const char *name = it->first.c_str();
	
	if ( fTree->GetLeaf( name ) )
	  fTree->SetBranchStatus( name, true );
      }
    }
  }

}
