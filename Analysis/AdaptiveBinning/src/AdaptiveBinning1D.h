///////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 21/03/2017
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

namespace Isis {

  class AdaptiveBinning1D: public AdaptiveBinning {

  public:

    // Main constructor
    AdaptiveBinning1D();

    // Constructor given vectors of values
    AdaptiveBinning1D( size_t  occ,
		       double  vmin,
		       double  vmax,
		       const std::vector<double> &values,
		       const std::vector<double> &weights = std::vector<double>() );

    // Destructor
    ~AdaptiveBinning1D();

    // Return an empty histogram with the adaptive binning structure
    TH1D* GetStruct( const char *name = "", const char *title = "" ) const;

    // Returns the maximum value for the histogram range
    inline const double GetMax() const;

    // Returns the minimum value for the histogram range
    inline const double GetMin() const;

  protected:

    // Maximum of the histogram
    double fMax;

    // Minimum of the histogram
    double fMin;

  };

  //______________________________________________________________________________
  //  
  inline const double AdaptiveBinning1D::GetMax() const { return fMax; }
  
  //______________________________________________________________________________
  //
  inline const double AdaptiveBinning1D::GetMin() const { return fMin; }
}

#endif
