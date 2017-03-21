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
//  Implements the class to make two-dimensional adaptive binning
//  histograms. The construction can be made given a set of vectors
//  or a TTree object and the name of the leaves.
//
// --------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////


#ifndef ADAPTIVE_BINNING_2D
#define ADAPTIVE_BINNING_2D

#include "AdaptiveBinning.h"
#include "Definitions.h"
#include "Bin2D.h"

#include <iostream>
#include <vector>

#include "TTree.h"
#include "TH2Poly.h"


//______________________________________________________________________________

namespace Isis {

  class AdaptiveBinning2D: public AdaptiveBinning {
    
  public:
    
    // Main constructor
    AdaptiveBinning2D();

    // Constructor given vectors of values
    AdaptiveBinning2D( size_t min_occ,
		       double xmin,
		       double xmax,
		       double ymin,
		       double ymax,
		       const Doubles &xvalues,
		       const Doubles &yvalues,
		       const Doubles &weights = Doubles() );

    // Destructor
    ~AdaptiveBinning2D();

    // Makes an adjusted adaptive binned histogram
    TH2Poly* GetAdjStruct( const char *name = "", const char *title = "" ) const;

    // Makes an adaptive binned histogram
    TH2Poly* GetStruct( const char *name = "", const char *title = "" ) const;

    // Return a list with the adjusted bins
    inline const std::vector<Bin2D*>& GetAdjBinList() const;

    // Return the maximum value in the X direction
    inline double GetXmax() const;

    // Return the minimum value in the X direction
    inline double GetXmin() const;

    // Return the maximum value in the Y direction
    inline double GetYmax() const;

    // Return the minimum value in the Y direction
    inline double GetYmin() const;

  protected:
    
    // List of adjusted bins
    std::vector<Bin2D*> fAdjBinList;

    // Maximum range in the X direction
    double fXmax;

    // Minimum range in the X direction
    double fXmin;

    // Maximum range in the Y direction
    double fYmax;

    // Minimum range in the Y direction
    double fYmin;

  };

  //______________________________________________________________________________
  //
  inline const std::vector<Bin2D*>& AdaptiveBinning2D::GetAdjBinList() const {
    return fAdjBinList;
  }

  //______________________________________________________________________________
  //
  inline double AdaptiveBinning2D::GetXmax() const { return fXmax; }

  //______________________________________________________________________________
  //
  inline double AdaptiveBinning2D::GetXmin() const { return fXmin; }

  //______________________________________________________________________________
  //
  inline double AdaptiveBinning2D::GetYmax() const { return fYmax; }

  //______________________________________________________________________________
  //
  inline double AdaptiveBinning2D::GetYmin() const { return fYmin; }
  
}

#endif
