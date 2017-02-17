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
///////////////////////////////////////////////////////////////////////////////////


#include "Messenger.h"
#include "RootUtils.h"

#include "TFile.h"
#include "TObject.h"

#include <string>

//_______________________________________________________________________________
//
TObject* Analysis::GetSafeObject( TFile *ifile, const std::string &path ) {

  TObject *obj = ifile->Get(path.c_str());

  if ( !obj )
    IError <<
      "Unable to get object < " << path << " > from file: " << ifile->GetName()
				<< IEndMsg;

  return obj;
}
