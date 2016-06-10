///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  Analysis package                                                             //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  AUTHOR: Miguel Ramos Pernas                                                  //
//  e-mail: miguel.ramos.pernas@cern.ch                                          //
//                                                                               //
//  Last update: 10/06/2016                                                      //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  Description:                                                                 //
//                                                                               //
//  A set of different classes and functions to manage variables are defined     //
//  here. All these objects are used by the other classes in the Analysis        //
//  package, to generate hisograms, graphs, perform analysis in ranges, etc.     //
//  The functions < VariableType > and < ExtractValue > are linked together, in  //
//  such a way that the behaviour for the different types have to be the same.   //
//                                                                               //
// ----------------------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////////////////


#include "Variables.h"


//_______________________________________________________________________________
// -- BUFFER VARIABLE

//_______________________________________________________________________________
// Main constructor
Analysis::BufferVariable::BufferVariable() { }

//_______________________________________________________________________________
// Constructor given the type as a character
Analysis::BufferVariable::BufferVariable( const char &type ) : fType( type ) {
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
Analysis::BufferVariable::~BufferVariable() { }

//_______________________________________________________________________________
// Returns a void pointer to the value that is being used
void* Analysis::BufferVariable::PathToValue() {
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
std::string Analysis::BufferVariable::ToString() const {
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
// Definition of the stream operator
std::ostream& Analysis::operator << ( std::ostream &os, const Analysis::BufferVariable &var ) {
  os << var.ToString();
  return os;
}

//_______________________________________________________________________________


//_______________________________________________________________________________
// -- NAMED VARIABLE

//_______________________________________________________________________________
// Main constructor
Analysis::NamedVariable::NamedVariable() { }

//_______________________________________________________________________________
// Constructor given the name and the expression
Analysis::NamedVariable::NamedVariable( const std::string &name,
					const std::string &expr,
					const char        &type ) :
  fExpr( expr ), fName( name ), fType( type ) { if ( fExpr.size() == 0 ) fExpr = fName; }

//_______________________________________________________________________________
// Destructor
Analysis::NamedVariable::~NamedVariable() { }
