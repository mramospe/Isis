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

#include "Bin.h"


//______________________________________________________________________________

namespace Analysis {

  class AdaptiveBinning {

  public:

    // Constructors and destructor
    AdaptiveBinning();
    virtual ~AdaptiveBinning();
    
    // Method
    virtual TObject* GetStruct( const char *name = "",
				const char *title = "" ) const;

    // Inline methods
    inline const std::vector<Bin*>& GetBinList() const;
    inline size_t                   GetNbins() const;

  protected:
    
    // Attribute
    std::vector<Bin*> fBinList;
        
  };

  //_______________
  // INLINE METHODS

  const std::vector<Bin*>& AdaptiveBinning::GetBinList() const {
    return fBinList;
  }
  // Returns the number of bins
  inline size_t AdaptiveBinning::GetNbins() const {
    return fBinList.size();
  }
}

#endif
