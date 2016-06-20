//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
//  General package                                                             //
//                                                                              //
// ---------------------------------------------------------------------------- //
//                                                                              //
//  AUTHOR: Miguel Ramos Pernas                                                 //
//  e-mail: miguel.ramos.pernas@cern.ch                                         //
//                                                                              //
//  Last update: 20/06/2016                                                     //
//                                                                              //
// ---------------------------------------------------------------------------- //
//                                                                              //
//  Description:                                                                //
//                                                                              //
//  Main class to store any kind of primitive data types. Different objects of  //
//  this type can be stored in the same container, while refering to different  //
//  data types. It results very useful when dealing with buffers were each      //
//  variable has a different type. Since it uses a < union >, the size of this  //
//  objects is defined as that of the larger type for the computer.             //
//                                                                              //
// ---------------------------------------------------------------------------- //
//////////////////////////////////////////////////////////////////////////////////


#include "BufferVariable.h"


//_______________________________________________________________________________


// -- CONSTRUCTOR AND DESTRUCTOR

//_______________________________________________________________________________
// Main constructor
General::BufferVariable::BufferVariable() { }

//_______________________________________________________________________________
// Constructor given the type as a character
General::BufferVariable::BufferVariable( const char &type ) : fType( type ) {
  switch( fType ) {
  case 'B': break; // char
  case 'b': break; // unsigned char
  case 'S': break; // short int
  case 's': break; // unsigned short int
  case 'I': break; // int
  case 'i': break; // unsigned int
  case 'F': break; // float
  case 'D': break; // double
  case 'L': break; // long long int
  case 'l': break; // unsigned long long int
  case 'O': break; // bool
  default:
    std::cerr << "ERROR: Unknown type for buffer variable < " << type << " >" << std::endl;
    break;
  }
}

//_______________________________________________________________________________
// Destructor
General::BufferVariable::~BufferVariable() { }

//_______________________________________________________________________________


// -- PUBLIC METHODS

//_______________________________________________________________________________
// Returns a void pointer to the value that is being used
void* General::BufferVariable::PathToValue() {
  switch( fType ) {
  case 'B': return &fCharValue;
  case 'b': return &fUCharValue;
  case 'S': return &fSIntValue;
  case 's': return &fUSIntValue;
  case 'I': return &fIntValue;
  case 'i': return &fUIntValue;
  case 'F': return &fFloatValue;
  case 'D': return &fDoubleValue;
  case 'L': return &fLLIntValue;
  case 'l': return &fULLIntValue;
  case 'O': return &fBoolValue;
  default:  return 0;
  }
}

//_______________________________________________________________________________
// Converts this class to a string
std::string General::BufferVariable::ToString() const {
  switch( fType ) {
  case 'B': return std::string( 1, fCharValue );
  case 'b': return std::string( 1, fUCharValue );
  case 'S': return std::to_string( fSIntValue );
  case 's': return std::to_string( fUSIntValue );
  case 'I': return std::to_string( fIntValue );
  case 'i': return std::to_string( fUIntValue );
  case 'F': return std::to_string( fFloatValue );
  case 'D': return std::to_string( fDoubleValue );
  case 'L': return std::to_string( fLLIntValue );
  case 'l': return std::to_string( fULLIntValue );
  case 'O': return std::to_string( fBoolValue );
  default:  return std::string();
  }
}

//_______________________________________________________________________________


// -- OPERATOR

//_______________________________________________________________________________
// Definition of the stream operator
std::ostream& General::operator << ( std::ostream &os, const General::BufferVariable &var ) {
  os << var.ToString();
  return os;
}
