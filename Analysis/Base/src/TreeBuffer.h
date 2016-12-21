///////////////////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 15/11/2016
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

namespace Analysis {
  
  class TreeBuffer: public General::BufferArray {
    
  public:
    
    // Definition of internal variables
    typedef std::vector<TBranch*> BranchVector;
    
    // Constructors and destructor
    TreeBuffer();
    TreeBuffer( TTree *tree,
		SortFunc func = [] ( const std::string &strA,
				     const std::string &strB ) { return strA < strB; } );
    ~TreeBuffer();
    
    // Methods
    void                     AttachTree( TTree *tree );
    General::BufferVariable* CreateVariable( const std::string &name, const char &type );
    void                     Load( const std::string &expr );
    General::BufferVariable* LoadVariable( const std::string &name );
    void                     SetBranchStatus( const bool &dec );

    // Inline methods
    inline void          Fill();
    inline void          FillBranches();
    inline BranchVector& GetBranchVector();
    inline TTree*        GetTree();
    
  protected:
    
    // Attributes
    TTree        *fTree;
    BranchVector  fBranchVector;
    
  };

  //__________________
  // -- INLINE METHODS

  // Fills the attached tree
  inline void TreeBuffer::Fill() { fTree -> Fill(); }
  // Fills the branches stored in the class
  inline void TreeBuffer::FillBranches() {
    for ( auto it = fBranchVector.begin(); it != fBranchVector.end(); ++it )
      (*it) -> Fill();
  }
  // Returns the vector with the branches attached to this class
  inline TreeBuffer::BranchVector& TreeBuffer::GetBranchVector() { return fBranchVector; }
  // Returns the attached tree
  inline TTree* TreeBuffer::GetTree() { return fTree; }
  
}

#endif
