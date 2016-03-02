///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  Analysis package                                                             //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  AUTHOR: Miguel Ramos Pernas                                                  //
//  e-mail: miguel.ramos.pernas@cern.ch                                          //
//                                                                               //
//  Last update: 29/02/2016                                                      //
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
    CutComparer( const std::map<std::string, std::pair<TTree*, std::string> > &catvec );

    // Destructor
    ~CutComparer();

    // Methods
    void AddCutVariable( const std::string &name,
			 const std::string &dir,
			 const size_t      &npoints,
			 const double      &vmin,
			 const double      &vmax );
    void AddCutVariable( const std::string &name,
			 const std::string &expr,
			 const std::string &dir,
			 const size_t      &npoints,
			 const double      &vmin,
			 const double      &vmax );
    void Compare();

    // Inline methods
    inline void AddCategory( const std::string &name,
			     TTree             *tree,
			     const std::string &cut = "",
			     const std::string &weight = "" );
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

    // Nested struct to manage the variables
    struct CutCompVar {
      CutCompVar( const std::string &expr,
		  const size_t      &npoints,
		  const double      &vmin,
		  const double      &vmax ) :
	fExpr( expr ), fMax( vmax ), fMin( vmin ), fN( npoints ) { }
      std::string fExpr;
      double      fMax;
      double      fMin;
      size_t      fN;
    };

    // Attributes
    std::map<std::string,
	     std::pair<TTree*, std::string> > fCategories;
    std::vector<
      std::pair<std::string, CutCompVar> >    fCompVars;
    std::string                               fCutString;
    std::vector<
      std::pair<std::string, CutCompVar> >    fCutVars;
    General::LoopArray                        fLoopArray;

  };

  //______________________________
  // INLINE METHODS

  // Adds a new category to the comparer
  inline void CutComparer::AddCategory( const std::string &name,
					TTree             *tree,
					const std::string &cut,
					const std::string &weight ) {
    if ( weight.size() )
      fCategories[ name ] = std::make_pair( tree, weight + "*(" + cut + ")" );
    else
      fCategories[ name ] = std::make_pair( tree, weight );
  }
  // Adds a new variable to compare. The number of bins and the range have to be
  // specified.
  inline void CutComparer::AddCompVariable( const std::string &name,
					    const size_t      &nbins,
					    const double      &vmin,
					    const double      &vmax ) {
    fCompVars.push_back( std::make_pair( name, CutCompVar( name, nbins, vmin, vmax ) ) );
  }
  // Adds a new variable to compare as an expression
  inline void CutComparer::AddCompVariable( const std::string &name,
					    const std::string &expr,
					    const size_t      &nbins,
					    const double      &vmin,
					    const double      &vmax ) {
    fCompVars.push_back( std::make_pair( name, CutCompVar( expr, nbins, vmin, vmax ) ) );
  }
}

#endif
