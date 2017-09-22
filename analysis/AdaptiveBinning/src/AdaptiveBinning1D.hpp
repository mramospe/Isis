///////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 03/07/2017
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

#include "AdaptiveBinning.hpp"
#include "Definitions.hpp"
#include "Bin1D.hpp"

#include "TH1D.h"
#include "TTree.h"

#include <iostream>


//______________________________________________________________________________

namespace isis {

  class AdaptiveBinning1D: public AdaptiveBinning {

  public:

    // Main constructor
    AdaptiveBinning1D() : AdaptiveBinning() { };

    // Constructor given vectors of values
    AdaptiveBinning1D( size_t  occ,
		       double  vmin,
		       double  vmax,
		       const Doubles &values,
		       const Doubles &weights = Doubles() );

    // Destructor
    ~AdaptiveBinning1D() { };

    // Return an empty histogram with the adaptive binning structure
    TH1D* getStruct( const char *name = "", const char *title = "" ) const;

    // Returns the maximum value for the histogram range
    inline const double getMax() const {

      return fMax;
    }

    // Returns the minimum value for the histogram range
    inline const double getMin() const {

      return fMin;
    }

  protected:

    // Maximum of the histogram
    double fMax;

    // Minimum of the histogram
    double fMin;

  };

}

#endif
