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

namespace isis {

  class AdaptiveBinning {

  public:

    // Constructor
    AdaptiveBinning();

    // Destructor
    virtual ~AdaptiveBinning();
    
    // Method to get the a histogram with the adaptive binned structure
    virtual TObject* getStruct( const char *name = "",
				const char *title = "" ) const;

    // Return the list of bins
    inline const std::vector<Bin*>& getBinList() const;

    // Return the number of bins
    inline size_t getNbins() const;

  protected:
    
    // List of bins
    std::vector<Bin*> fBinList;
        
  };

  //______________________________________________________________________________
  //
  const std::vector<Bin*>& AdaptiveBinning::getBinList() const {
    
    return fBinList;
  }

  //______________________________________________________________________________
  //
  inline size_t AdaptiveBinning::getNbins() const { return fBinList.size(); }
}

#endif
