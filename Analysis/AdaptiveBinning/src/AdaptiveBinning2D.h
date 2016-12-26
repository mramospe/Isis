///////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 26/12/2016
//
// --------------------------------------------------------------------
//
//  Description:
//
//  Implements the class to make two-dimensional adaptive binning
//  histograms. The construction can be made given a set of vectors
//  or a TTree object and the name of the leaves.
//
// --------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////


#ifndef ADAPTIVE_BINNING_2D
#define ADAPTIVE_BINNING_2D

#include "AdaptiveBinning.h"
#include "Bin2D.h"

#include <iostream>
#include <vector>

#include "TTree.h"
#include "TH2Poly.h"


//______________________________________________________________________________

namespace Analysis {

  class AdaptiveBinning2D: public AdaptiveBinning {
    
  public:
    
    // Constructors and destructor
    AdaptiveBinning2D();
    AdaptiveBinning2D( size_t min_occ,
		       double xmin,
		       double xmax,
		       double ymin,
		       double ymax,
		       const std::vector<double> &xvalues,
		       const std::vector<double> &yvalues,
		       const std::vector<double> &weights = std::vector<double>() );
    ~AdaptiveBinning2D();

    // Methods
    TH2Poly* GetAdjStruct( const char *name = "", const char *title = "" ) const;
    TH2Poly* GetStruct( const char *name = "", const char *title = "" ) const;

    // Inline methods
    inline const std::vector<Bin2D*>& GetAdjBinList() const;
    inline double GetXmax() const;
    inline double GetXmin() const;
    inline double GetYmax() const;
    inline double GetYmin() const;

  protected:
    
    // Attributes
    std::vector<Bin2D*> fAdjBinList;
    double              fXmax;
    double              fXmin;
    double              fYmax;
    double              fYmin;

  };

  //_______________
  // INLINE METHODS

  // Returns the list of adjusted adaptive bins
  inline const std::vector<Bin2D*>& AdaptiveBinning2D::GetAdjBinList() const {
    return fAdjBinList;
  }
  // Return the limits of the histogram after the construction
  inline double AdaptiveBinning2D::GetXmax() const { return fXmax; }
  inline double AdaptiveBinning2D::GetXmin() const { return fXmin; }
  inline double AdaptiveBinning2D::GetYmax() const { return fYmax; }
  inline double AdaptiveBinning2D::GetYmin() const { return fYmin; }
  
}

#endif
