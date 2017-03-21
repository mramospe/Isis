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

#include "BufferArray.h"

#include "TBranch.h"
#include "TTree.h"


//_______________________________________________________________________________

namespace Isis {
  
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
    void AttachTree( TTree *tree );

    // Creates a new variable in the attached tree given the name and its type
    BufferVariable* CreateVariable( const std::string &name, const char &type );

    // Loads the variables satisfying the requirements in the given expression
    void Load( const std::string &expr );

    // Loads a variable from the attached tree. Automatically determines the type of
    // the variable and constructs a BufferVariable class to set the branch
    // address to.
    BufferVariable* LoadVariable( const std::string &name );

    // Changes the status of the branches in the attached tree to that given to the
    // function
    void SetBranchStatus( const bool &dec );

    // Fills the attached tree
    inline void Fill();

    // Fills the branches stored in the class
    inline void FillBranches();

    // Returns the vector with the branches attached to this class
    inline BranchVector& GetBranchVector();

    // Returns the attached tree
    inline TTree* GetTree();
    
  protected:
    
    // Root tree
    TTree *fTree;

    // Vector of branches
    BranchVector fBranchVector;
    
  };

  //_______________________________________________________________________________
  //
  inline void TreeBuffer::Fill() { fTree->Fill(); }

  //_______________________________________________________________________________
  //
  inline void TreeBuffer::FillBranches() {
    for ( auto it = fBranchVector.begin(); it != fBranchVector.end(); ++it )
      (*it)->Fill();
  }

  //_______________________________________________________________________________
  //
  inline TreeBuffer::BranchVector& TreeBuffer::GetBranchVector() {
    return fBranchVector;
  }

  //_______________________________________________________________________________
  //
  inline TTree* TreeBuffer::GetTree() { return fTree; }
  
}

#endif
