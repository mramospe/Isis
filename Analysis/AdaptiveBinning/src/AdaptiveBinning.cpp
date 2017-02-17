///////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 17/02/2017
//
// --------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////


#include "AdaptiveBinning.h"


//______________________________________________________________________________
//
Analysis::AdaptiveBinning::AdaptiveBinning() { }

//______________________________________________________________________________
//
Analysis::AdaptiveBinning::~AdaptiveBinning() {

  for ( auto it = fBinList.begin(); it != fBinList.end(); ++it )
    delete (*it);
}

//______________________________________________________________________________
//
TObject* Analysis::AdaptiveBinning::GetStruct( const char *name, const char *title ) const {
  return 0;
}
