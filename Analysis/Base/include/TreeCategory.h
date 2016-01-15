///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  Analysis package                                                             //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  AUTHOR: Miguel Ramos Pernas                                                  //
//  e-mail: miguel.ramos.pernas@cern.ch                                          //
//                                                                               //
//  Last update: 05/01/2016                                                      //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  Description:                                                                 //
//                                                                               //
//  Implements the class TreeCategory. It allows to manage subsets of data       //
//  located in TTree objects that satisfy a given set of cuts. Also provides     //
//  a method to obtain a subset of the TreeCategory object itself, returning     //
//  the index of the entries.                                                    //
//                                                                               //
// ----------------------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////////////////


#ifndef TREE_CATEGORY
#define TREE_CATEGORY

#include "TObject.h"
#include "TTree.h"

#include <string>
#include <vector>


//_______________________________________________________________________________

namespace Analysis {

  class TreeCategory {

  public:
    
    // Constructor
    TreeCategory( std::string name, TTree *itree, std::string cuts = "" );
    TreeCategory( std::string name, TreeCategory &other, std::string cuts = "" );

    // Destructor
    ~TreeCategory();

    // Methods
    inline std::vector<size_t>::iterator Begin();
    inline std::vector<size_t>::iterator End();
    inline const std::string   GetCuts() const;
    inline const size_t        GetEntries() const;
    inline const std::string   GetName() const;
    inline TTree*              GetTree();
    TH1*                       MakeHistogram( std::string var,
					      size_t      nbins,
					      double      vmin,
					      double      vmax,
					      std::string cuts = "",
					      bool        check = true );
    TH1*                       MakeHistogram( std::string name,
					      std::string expr,
					      size_t nbins,
					      double vmin,
					      double vmax,
					      std::string cuts = "",
					      bool        check = true );
    std::vector<size_t>        MakeSlice( std::string cuts, bool check = true );

  protected:

    // Attributes
    std::string          fCuts;
    std::string          fName;
    TTree               *fTree;
    std::vector<size_t>  fTreeEntries;

  };

  //______________________________
  // INLINE METHODS

  // Returns the beginning of the vector of indices
  inline std::vector<size_t>::iterator TreeCategory::Begin() { return fTreeEntries.begin(); }
  // Returns the end of the vector of indices
  inline std::vector<size_t>::iterator TreeCategory::End() { return fTreeEntries.end(); }
  // Returns the set of cuts which where used to construct the category
  inline const std::string TreeCategory::GetCuts() const { return fCuts; }
  // Returns the number of entries attached to this category
  inline const size_t TreeCategory::GetEntries() const { return fTreeEntries.size(); }
  // Returns the name of the category
  inline const std::string TreeCategory::GetName() const { return fName; }
  // Returns the TTree object associated with this category
  inline TTree* TreeCategory::GetTree() { return fTree; }

}

#endif
