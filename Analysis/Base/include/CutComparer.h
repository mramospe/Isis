///////////////////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 28/03/2016
//
// --------------------------------------------------------------------------------
//
//  Description:
//
//  Implements the class to perform comparisons among different TreeCategory
//  classes. One has to define the different variables that are going to be
//  compared together with those to cut over. This class will work in the
//  current directory so one has to construct a TFile or a TDirectoryFile
//  object in order to use it. All the histograms will be saved in the
//  different folders, together with the cut information and the canvas with
//  all the histograms.
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#ifndef CUT_COMPARER
#define CUT_COMPARER

#include "LoopArray.h"
#include "Variables.h"

#include "TTree.h"

#include <map>
#include <string>
#include <utility>
#include <vector>


//_______________________________________________________________________________

namespace Analysis {

  class CutComparer {

  public:

    // Constructors
    CutComparer();

    // Destructor
    ~CutComparer();

    // Methods
    void AddCutVariable( std::string        name,
			 const std::string &dir,
			 const size_t      &npoints,
			 const double      &vmin,
			 const double      &vmax );
    void AddCutVariable( const std::string &name,
			 std::string        expr,
			 const std::string &dir,
			 const size_t      &npoints,
			 const double      &vmin,
			 const double      &vmax );
    void Compare();

    // Inline methods
    inline void AddCategory( const std::string &name,
			     TTree             *tree,
			     const std::string &cut = "",
			     const std::string &weight = "",
			     const bool        &applycuts = true );
    inline void AddCompVariable( const std::string &name,
				 const size_t      &nbins,
				 const double      &vmin,
				 const double      &vmax );
    inline void AddCompVariable( const std::string &name,
				 const std::string &expr,
				 const size_t      &nbins,
				 const double      &vmin,
				 const double      &vmax );

  protected:

    // Nested struct to manage the categories
    struct CutCompCat {
      CutCompCat( const std::string &name,
		  TTree             *tree,
		  const std::string &cut = "",
		  const std::string &weight = "",
		  const bool        &applycuts = true ) :
	fCut( applycuts ), fName( name ), fTree( tree ), fWgtExpr( cut ) {
	if ( weight.size() ) {
	  if ( fWgtExpr.size() )
	    fWgtExpr = weight + "*(" + fWgtExpr + ")";
	  else
	    fWgtExpr = weight;
	}
      }
      bool         fCut;
      std::string  fName;
      TTree       *fTree;
      std::string  fWgtExpr;
    };

    // Attributes
    std::vector<CutCompCat>         fCategories;
    std::vector<Variable<double> >  fCompVars;
    std::string                     fCutString;
    std::vector<std::pair<
		  Variable<double>,
		  size_t> >         fCutVars;
    General::LoopArray              fLoopArray;

  };

  //______________________________
  // INLINE METHODS

  // Adds a new category to the comparer
  inline void CutComparer::AddCategory( const std::string &name,
					TTree             *tree,
					const std::string &cut,
					const std::string &weight,
					const bool        &applycuts ) {
    fCategories.push_back( CutCompCat( name, tree, cut, weight, applycuts ) );
  }
  // Adds a new variable to compare. The number of bins and the range have to be
  // specified.
  inline void CutComparer::AddCompVariable( const std::string &name,
					    const size_t      &nbins,
					    const double      &vmin,
					    const double      &vmax ) {
    fCompVars.push_back( Variable<double>( name, name, vmin, vmax, nbins ) );
  }
  // Adds a new variable to compare as an expression
  inline void CutComparer::AddCompVariable( const std::string &name,
					    const std::string &expr,
					    const size_t      &nbins,
					    const double      &vmin,
					    const double      &vmax ) {
    fCompVars.push_back( Variable<double>( name, expr, vmin, vmax, nbins ) );
  }
}

#endif
