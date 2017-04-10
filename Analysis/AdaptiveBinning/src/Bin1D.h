///////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 10/04/2017
//
// --------------------------------------------------------------------
//
//  Description:
//
//  Implements the one-dimensional bin class to work together with
//  AdaptiveBinning1D to create adaptive binned histograms.
//
// --------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////


#ifndef BIN_1D
#define BIN_1D

#include "Bin.h"


//______________________________________________________________________________

namespace isis {

  class AdaptiveBinning1D;

  class Bin1D: public Bin {

    friend AdaptiveBinning1D;

  public:
    
    // Constructor
    Bin1D( const double &vmin );

    // Destructor
    ~Bin1D();

    // Fill the bin if the point given is inside the considered range
    void fill( const double &pos, const double &weight = 1 );

    // Inline methods
    inline const double getMin() const;

  protected:

    // Minimum value required for an entry to be inside the bin
    double fMin;

    // Set the minimum value of the bin
    inline void setMin( const double &vmin );

  };

  //______________________________________________________________________________
  //
  inline const double Bin1D::getMin() const { return fMin; }
  
  //______________________________________________________________________________
  //
  inline void Bin1D::setMin( const double &vmin ) { fMin = vmin; }

}

#endif
