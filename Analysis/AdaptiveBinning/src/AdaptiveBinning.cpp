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


#include "AdaptiveBinning.h"


//______________________________________________________________________________


// -- CONSTRUCTOR AND DESTRUCTOR

//______________________________________________________________________________
// Constructor
Analysis::AdaptiveBinning::AdaptiveBinning() { }

//______________________________________________________________________________
// Destructor
Analysis::AdaptiveBinning::~AdaptiveBinning() {

  for ( auto it = fBinList.begin(); it != fBinList.end(); ++it )
    delete (*it);
}

//______________________________________________________________________________


// -- METHOD

//______________________________________________________________________________
// Virtual method to get the a histogram with the adaptive binned structure
TObject* Analysis::AdaptiveBinning::GetStruct( const char *name, const char *title ) const {
  return 0;
}
