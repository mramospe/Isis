///////////////////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 19/10/2017
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#include "Exceptions.hpp"
#include "RootUtils.hpp"

#include "TFile.h"
#include "TObject.h"

#include <string>

//_______________________________________________________________________________

namespace isis {

  //_______________________________________________________________________________
  //
  TObject* getSafeObject( TFile *ifile, const std::string &path ) {

    TObject *obj = ifile->Get(path.c_str());

    if ( !obj )
      throw NotFound("object", path);
    
    return obj;
  }

}
