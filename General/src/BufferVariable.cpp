//////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 21/03/2017
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#include "BufferVariable.h"
#include "Definitions.h"
#include "Messenger.h"
#include "ValueTypeDef.h"


//_______________________________________________________________________________

namespace Isis {

  //_______________________________________________________________________________
  //
  BufferVariable::BufferVariable( const char &type ) : fType( type ) {
    this->Construct();
  }

  //_______________________________________________________________________________
  //
  BufferVariable::~BufferVariable() { this->Delete(); }

  //_______________________________________________________________________________
  //
  std::string BufferVariable::ToString() const {
  
    I_SWITCH_BY_DATA_TYPE(fType, fPath, I_TO_STRING_PTR,
			
			  IError << "The type of the buffer "
			  "variable has not been specified yet" << IEndMsg;
			  return std::string();
			  );
  }

  //_______________________________________________________________________________
  //
  void BufferVariable::Construct() {

    if ( fType != '\0' )
      I_SWITCH_BY_DATA_TYPE(fType, fPath, I_NEW_INSTANCE,
			  
			    IError << "Unknown type for buffer variable < "
			    << fType << " >" << IEndMsg;
			    );
  }

  //_______________________________________________________________________________
  //
  void BufferVariable::Delete() {

    if ( fType != '\0' )
      I_SWITCH_BY_DATA_TYPE(fType, fPath, I_DELETE_PTR, NOOP);
  }

}
