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
//  Last update: 22/04/2016			                     //
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

#include "Bin.h"


//______________________________________________________________________________

namespace Analysis {

  class AdaptiveBinning1D;

  class Bin1D: public Bin {

    friend AdaptiveBinning1D;

  public:
    
    // Constructor and destructor
    Bin1D( const double &vmin );
    ~Bin1D();

    // Method
    void Fill( const double &pos, const double &weight = 1 );

    // Inline methods
    inline const double GetMin() const;

  protected:

    // Attributes
    double fMin;

    // Method
    inline void SetMin( const double &vmin );

  };

  //_______________
  // INLINE METHODS

  // Method to return the limit of the bin
  inline const double Bin1D::GetMin() const { return fMin; }
  // Sets the limits of the bin to the input values
  inline void Bin1D::SetMin( const double &vmin ) { fMin = vmin; }

}

#endif
