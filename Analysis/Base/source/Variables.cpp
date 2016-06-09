///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  Analysis package                                                             //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  AUTHOR: Miguel Ramos Pernas                                                  //
//  e-mail: miguel.ramos.pernas@cern.ch                                          //
//                                                                               //
//  Last update: 09/06/2016                                                      //
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
