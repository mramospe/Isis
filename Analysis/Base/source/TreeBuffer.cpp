///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  Analysis package                                                             //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  AUTHOR: Miguel Ramos Pernas                                                  //
//  e-mail: miguel.ramos.pernas@cern.ch                                          //
//                                                                               //
//  Last update: 21/06/2016                                                      //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  Description:                                                                 //
//                                                                               //
//  Defines the class to help when performing input/output operations from Root  //
//  TTree objects. It is meant to be used to allow the user to work with the     //
//  variables that are present in the tree without knowing its type.             //
//                                                                               //
// ----------------------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////////////////


#include "TreeBuffer.h"
#include "TreeManagement.h"


//_______________________________________________________________________________


// -- CONSTRUCTOR AND DESTRUCTOR

//_______________________________________________________________________________
// Main constructor
Analysis::TreeBuffer::TreeBuffer() : General::BufferArray() { }

//_______________________________________________________________________________
// Constructor given the tree to be attached and the function to sort the booked
// variables.
Analysis::TreeBuffer::TreeBuffer( TTree *tree, SortFunc func ) :
  BufferArray( func ), fTree( tree ) { }

//_______________________________________________________________________________
// Destructor
Analysis::TreeBuffer::~TreeBuffer() { }

//_______________________________________________________________________________


// -- PUBLIC METHODS

//_______________________________________________________________________________
// Attaches a tree to this class. All the variables stored in this class are
// removed.
void Analysis::TreeBuffer::AttachTree( TTree *tree ) {

  for ( auto it = fVarMap.begin(); it != fVarMap.end(); ++it )
    delete it -> second;
  
  fVarMap.clear();

  fTree = tree;
}

//_______________________________________________________________________________
// Creates a new variable in the attached tree given the name and its type
void Analysis::TreeBuffer::CreateVariable( const std::string &name, const char &type ) {

  this -> AddVariable( name, type );
  void *path      = fVarMap[ name ] -> PathToValue();
  TBranch *branch = fTree -> Branch( name.c_str(), path, (name + '/' + type).c_str() );
  fBranchVector.push_back( branch );
}

//_______________________________________________________________________________
// Loads a variable from the attached tree. Automatically determines the type of
// the variable and constructs a General::BufferVariable class to set the branch
// address to.
void Analysis::TreeBuffer::LoadVariable( const std::string &name ) {

  this -> AddVariable( name, Analysis::GetVarType( fTree, name ) );
  void *path = fVarMap[ name ] -> PathToValue();
  fTree -> SetBranchAddress( name.c_str(), path );
}

//_______________________________________________________________________________
// Changes the status of the branches in the attached tree to that given to the funcion
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
