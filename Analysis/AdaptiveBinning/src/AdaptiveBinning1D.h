///////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 15/12/2016
//
// --------------------------------------------------------------------
//
//  Description:
//
//  Implements the class to make one-dimensional adaptive binning
//  histograms. The construction can be made given a set of vectors
//  or a TTree object and the name of the leaves.
//
// --------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////


#ifndef ADAPTIVE_BINNING_1D
#define ADAPTIVE_BINNING_1D

#include "AdaptiveBinning.h"
#include "Bin1D.h"

#include "TH1D.h"
#include "TTree.h"

#include <iostream>


//______________________________________________________________________________

namespace Analysis {

  class AdaptiveBinning1D: public AdaptiveBinning {

  public:

    // Constructors and destructor
    AdaptiveBinning1D();
    AdaptiveBinning1D( size_t  occ,
		       double  vmin,
		       double  vmax,
		       const std::vector<double> &values,
		       const std::vector<double> &weights = std::vector<double>() );
    ~AdaptiveBinning1D();

    // Method
    TH1D* GetStruct( const char *name = "", const char *title = "" ) const;

    // Inline Methods
    inline const double              GetMax() const;
    inline const double              GetMin() const;

  protected:

    // Attributes
    double fMax;
    double fMin;

  };

  //_______________
  // INLINE METHODS

  // Returns the maximum value for the variable of this class
  inline const double AdaptiveBinning1D::GetMax() const { return fMax; }
  // Returns the minimum value for the variable of this class
  inline const double AdaptiveBinning1D::GetMin() const { return fMin; }
}

#endif
