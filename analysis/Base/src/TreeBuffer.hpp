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
//
//  Description:
//
//  Defines the class to help when performing input/output operations from Root
//  TTree objects. It is meant to be used to allow the user to work with the
//  variables that are present in the tree without knowing its type.
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#ifndef TREE_BUFFER
#define TREE_BUFFER

#include "BufferArray.hpp"

#include "TBranch.h"
#include "TTree.h"


//_______________________________________________________________________________

namespace isis {
  
  class TreeBuffer: public BufferArray {
    
  public:
    
    // Definition of internal variables
    typedef std::vector<TBranch*> BranchVector;
    
    // Main constructor
    TreeBuffer();

    // Constructor given the tree to be attached and the function to sort the booked
    // variables.
    TreeBuffer( TTree *tree,
		SortFunc func = [] ( const std::string &strA,
				     const std::string &strB ) { return strA < strB; } );

    // Destructor
    ~TreeBuffer();
    
    // Attaches a tree to this class. All the variables stored in this class are
    // removed.
    void attachTree( TTree *tree );

    // Creates a new variable in the attached tree given the name and its type
    BufferVariable* createVariable( const std::string &name, const char &type );

    // Loads the variables satisfying the requirements in the given expression
    void load( const std::string &expr );

    // Loads a variable from the attached tree. Automatically determines the type of
    // the variable and constructs a BufferVariable class to set the branch
    // address to.
    BufferVariable* loadVariable( const std::string &name );

    // Changes the status of the branches in the attached tree to that given to the
    // function
    void setBranchStatus( const bool &dec );

    // Fills the attached tree
    inline void fill();

    // Fills the branches stored in the class
    inline void fillBranches();

    // Returns the vector with the branches attached to this class
    inline BranchVector& getBranchVector();

    // Returns the attached tree
    inline TTree* getTree();
    
  protected:
    
    // Root tree
    TTree *fTree;

    // Vector of branches
    BranchVector fBranchVector;
    
  };

  //_______________________________________________________________________________
  //
  inline void TreeBuffer::fill() { fTree->Fill(); }

  //_______________________________________________________________________________
  //
  inline void TreeBuffer::fillBranches() {
    
    for ( auto it = fBranchVector.begin(); it != fBranchVector.end(); ++it )
      (*it)->Fill();
  }

  //_______________________________________________________________________________
  //
  inline TreeBuffer::BranchVector& TreeBuffer::getBranchVector() {
    return fBranchVector;
  }

  //_______________________________________________________________________________
  //
  inline TTree* TreeBuffer::getTree() { return fTree; }
  
}

#endif
