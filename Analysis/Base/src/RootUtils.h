///////////////////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 17/02/2017
//
// --------------------------------------------------------------------------------
//
//  Description:
//
//  Different utils to be used within the Root classes and functions
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#ifndef ROOT_UTILS
#define ROOT_UTILS

#include "TFile.h"
#include "TObject.h"

#include <string>


//_______________________________________________________________________________

namespace Analysis {

  // Get a pointer to an object in a TFile and send an error message if the operation
  // fails
  TObject* GetSafeObject( TFile *ifile, const std::string &path );

}

#endif
