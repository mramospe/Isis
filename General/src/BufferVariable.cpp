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
    this->construct();
  }

  //_______________________________________________________________________________
  //
  BufferVariable::~BufferVariable() { this->clear(); }

  //_______________________________________________________________________________
  //
  std::string BufferVariable::toString() const {
  
    I_SWITCH_BY_DATA_TYPE(fType, fPath, I_TO_STRING_PTR,
			
			  IError << "The type of the buffer "
			  "variable has not been specified yet" << IEndMsg;
			  return std::string();
			  );
  }

  //_______________________________________________________________________________
  //
  void BufferVariable::construct() {

    if ( fType != '\0' )
      I_SWITCH_BY_DATA_TYPE(fType, fPath, I_NEW_INSTANCE,
			  
			    IError << "Unknown type for buffer variable < "
			    << fType << " >" << IEndMsg;
			    );
  }

  //_______________________________________________________________________________
  //
  void BufferVariable::clear() {

    if ( fType != '\0' )
      I_SWITCH_BY_DATA_TYPE(fType, fPath, I_DELETE_PTR, NOOP);
  }

}
