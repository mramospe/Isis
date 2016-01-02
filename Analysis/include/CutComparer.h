///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  Analysis package                                                             //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  AUTHOR: Miguel Ramos Pernas                                                  //
//  e-mail: miguel.ramos.pernas@cern.ch                                          //
//                                                                               //
//  Last update: 02/01/2016                                                      //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  Description:                                                                 //
//                                                                               //
//  Implements the class to perform comparisons among different TreeCategory     //
//  classes. One has to define the different variables that are going to be      //
//  compared together with those to cut over. This class will work in the        //
//  current directory so one has to construct a TFile or a TDirectoryFile        //
//  object in order to use it. All the histograms will be saved in the           //
//  different folders, together with the cut information and the canvas with     //
//  all the histograms.                                                          //
//                                                                               //
// ----------------------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////////////////


#ifndef CUT_COMPARER
#define CUT_COMPARER

#include "LoopArray.h"
#include "TreeCategory.h"

#include <utility>
#include <string>
#include <vector>


//_______________________________________________________________________________

namespace Analysis {

  class CutComparer {

    // Nested struct variable to manage the categories
    struct CatCompVar {
      double      fMax;
      double      fMin;
      size_t      fN;
    };

  public:

    // Constructors
    CutComparer();
    CutComparer( std::vector<TreeCategory*> &catvec );

    // Destructor
    ~CutComparer();

    // Methods
    inline void AddCategory( TreeCategory &cat );
    inline void AddCategories( std::vector<TreeCategory*> &catvec );
    void        AddCompVariable( std::string var, size_t nbins, double vmin, double vmax );
    void        AddCutVariable( std::string var, std::string dir, size_t npoints, double vmin, double vmax );
    void        Compare();

  protected:

    // Attributes
    std::vector<TreeCategory*>             fCategories;
    std::vector<
      std::pair<std::string, CatCompVar> > fCompVars;
    std::string                            fCutString;
    std::vector<
      std::pair<std::string, CatCompVar> > fCutVars;
    General::LoopArray                     fLoopArray;
  };

  //______________________________
  // INLINE METHODS

  // Adds a new category to the comparer
  inline void CutComparer::AddCategory( TreeCategory &cat ) {
    fCategories.push_back( &cat );
  }
  // Adds a set of categories stored in a vector
  inline void CutComparer::AddCategories( std::vector<TreeCategory*> &catvec ) {
    fCategories.insert( fCategories.end(), catvec.begin(), catvec.end() );
  }

}

#endif
