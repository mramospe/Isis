///////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 13/06/2016
//
// --------------------------------------------------------------------
//
//  Description:
//
//  Implements the abstract class to generate adaptive binned
//  histograms. The structure of the output histogram is obtained
//  calling the < GetStruct > method.
//
// --------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////


#ifndef ADAPTIVE_BINNING
#define ADAPTIVE_BINNING

#include <iostream>

#include "TObject.h"


//______________________________________________________________________________

namespace Analysis {

  class AdaptiveBinning {

  public:

    // Constructors and destructor
    AdaptiveBinning();
    virtual ~AdaptiveBinning();

    // Methods
    virtual TObject* GetStruct( const char *name, const char *title );

    // Inline method
    inline bool IsWeighted();

  protected:
    
    // Attribute
    bool fWeighted;

    // Methods
    virtual void Construct( const size_t &occ );
    void         DisplayInfo( const size_t &entries,
			      const double &sw,
			      const size_t &nbins,
			      const double &occ );
  };

  //______________
  // INLINE METHOD
  inline bool AdaptiveBinning::IsWeighted() { return fWeighted; }

}

#endif
