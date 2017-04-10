//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 10/04/2017
//
// -------------------------------------------------------------------------------
//
//  Description:
//
//  Wrappers from the Base analysis library
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#include "RootUtils.h"

#include "TPython.h"


//_______________________________________________________________________________

// Wrappers from RootUtils
namespace RootUtils {

  //_______________________________________________________________________________
  //
  inline PyObject* getSafeObject( PyObject *ifile_obj, const std::string &path ) {

    TFile *ifile = static_cast<TFile*>(TPython::ObjectProxy_AsVoidPtr( ifile_obj ));
    
    return TPython::ObjectProxy_FromVoidPtr( Isis::getSafeObject(ifile, path),
					     "TObject",
					     false );
  }
}
