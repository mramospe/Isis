///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  Analysis package                                                             //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  AUTHOR: Miguel Ramos Pernas                                                  //
//  e-mail: miguel.ramos.pernas@cern.ch                                          //
//                                                                               //
//  Last update: 20/06/2016                                                      //
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
