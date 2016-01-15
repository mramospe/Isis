///////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------- //
//						                     //
//  Analysis package	  			                     //
//        					                     //
// ----------------------------------------------------------------- //
//						                     //
//  AUTHOR: Miguel Ramos Pernas		                             //
//  e-mail: miguel.ramos.pernas@cern.ch		                     //
//						                     //
//  Last update: 12/01/2016			                     //
//   						                     //
// ----------------------------------------------------------------- //
//						                     //
//  Description:				                     //
//						                     //
//  Implements the one-dimensional bin class to work together with   //
//  AdaptiveBinning1D to create adaptive binned histograms.          //
//						                     //
// ----------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////


#ifndef BIN_1D
#define BIN_1D

#include "AdaptiveBinning/Bin.h"


//______________________________________________________________________________

namespace Analysis {

  class AdaptiveBinning1D;

  class Bin1D: public Bin {

    friend AdaptiveBinning1D;

  public:
    
    // Constructor and destructor
    Bin1D();
    ~Bin1D();

    // Method
    void Fill( const double &pos, const double &weight = 1 );

    // Inline methods
    inline const double GetMin() const;
    inline const double GetMax() const;

  protected:

    // Attributes
    double fMax;
    double fMin;

    // Method
    inline void SetBin( const double &vmin, const double &vmax );

  };

  //_______________
  // INLINE METHODS

  // Methods to return the limit of the bin
  inline const double Bin1D::GetMin() const { return fMin; }
  inline const double Bin1D::GetMax() const { return fMax; }
  // Sets the limits of the bin to the input values
  inline void Bin1D::SetBin( const double &vmin, const double &vmax ) { fMin = vmin; fMax = vmax; }

}

#endif
