//////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 14/02/2017
//
// -------------------------------------------------------------------------------
//
//  Description:
//
//  Main class to store any kind of primitive data types. Different objects of
//  this type can be stored in the same container, while refering to different
//  data types. It results very useful when dealing with buffers were each
//  variable has a different type.
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#include "Definitions.h"
#include "BufferVariable.h"
#include "ValueTypeDef.h"


//_______________________________________________________________________________


// -- CONSTRUCTOR AND DESTRUCTOR

//_______________________________________________________________________________
// Constructor given the type as a character
General::BufferVariable::BufferVariable( const char &type ) : fType( type ) {
  this -> Construct();
}

//_______________________________________________________________________________
// Destructor
General::BufferVariable::~BufferVariable() { this -> Delete(); }

//_______________________________________________________________________________


// -- PUBLIC METHODS

//_______________________________________________________________________________
// Converts this class to a string
std::string General::BufferVariable::ToString() const {
  
  I_SWITCH_BY_DATA_TYPE(fType, fPath, I_TO_STRING_PTR,
			
			std::cerr << "ERROR: The type of the buffer "
			"variable has not been specified yet" << std::endl;
			return std::string();
			);
}

//_______________________________________________________________________________


// -- PRIVATE METHODS

//_______________________________________________________________________________
// Method to construct this class, given the type
void General::BufferVariable::Construct() {

  if ( fType != '\0' )
    I_SWITCH_BY_DATA_TYPE(fType, fPath, I_NEW_INSTANCE,
			  
			  std::cerr << "ERROR: Unknown type for buffer variable < "
			  << fType << " >" << std::endl;
			  );
}

//_______________________________________________________________________________
// Method to delete the value stored
void General::BufferVariable::Delete() {

  if ( fType != '\0' )
    I_SWITCH_BY_DATA_TYPE(fType, fPath, I_DELETE_PTR, NOOP);
}


//_______________________________________________________________________________


// -- OPERATOR

//_______________________________________________________________________________
// Definition of the stream operator
std::ostream& General::operator << ( std::ostream &os,
				     const General::BufferVariable &var ) {
  os << var.ToString();
  return os;
}
