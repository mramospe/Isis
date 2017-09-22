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

#include "Bin.hpp"


//______________________________________________________________________________

namespace isis {

  class AdaptiveBinning {

  public:

    // Constructor
    AdaptiveBinning() { };

    // Destructor
    virtual ~AdaptiveBinning() {

      for ( auto it = fBinList.begin(); it != fBinList.end(); ++it )
	delete (*it);
    }
    
    // Method to get the a histogram with the adaptive binned structure
    virtual TObject* getStruct( const char *name = "",
				const char *title = "" ) const = 0;

    // Return the list of bins
    inline const std::vector<Bin*>& getBinList() const {

      return fBinList;
    }

    // Return the number of bins
    inline size_t getNbins() const {

      return fBinList.size();
    }

  protected:
    
    // List of bins
    std::vector<Bin*> fBinList;
        
  };

}

#endif
