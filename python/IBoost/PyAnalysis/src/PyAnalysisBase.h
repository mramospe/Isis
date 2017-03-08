//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 08/03/2017
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

namespace py = boost::python;
namespace an = Analysis;


//_______________________________________________________________________________

// Wrappers from RootUtils
namespace RootUtils {

  //_______________________________________________________________________________
  //
  PyObject* GetSafeObject( PyObject *ifile_obj, const std::string &path ) {

    TFile *ifile = static_cast<TFile*>(TPython::ObjectProxy_AsVoidPtr( ifile_obj ));
    
    return TPython::ObjectProxy_FromVoidPtr( an::GetSafeObject(ifile, path),
					     "TObject",
					     false );
  }
}
