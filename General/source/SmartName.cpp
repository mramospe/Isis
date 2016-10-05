///////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 07/06/2016
//
// --------------------------------------------------------------------------------
//
//  Description:
//
//  Implements a class to help adding strings separated by one character. This
//  is useful, for example, to manage paths or file names, which are separated
//  by < / > and < _ > symbols respectively. When two classes of this type are
//  added, the separator is taken to that of the right hand in the addition.
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#include "SmartName.h"
#include "Utils.h"

#include <algorithm>
#include <vector>


//_______________________________________________________________________________


// -- CONSTRUCTOR AND DESTRUCTOR

//_______________________________________________________________________________
// Constructor
General::SmartName::SmartName( const std::string &str, const char &sep ) :
  fName( str ), fSep( sep ) { }

//_______________________________________________________________________________
// Destructor
General::SmartName::~SmartName() { }

//_______________________________________________________________________________


// -- NON-MEMBER OPERATOR

//_______________________________________________________________________________
// Stream operator
std::ostream& General::operator << ( std::ostream &os, const General::SmartName &name ) {
  os << name.GetName();
  return os;
}

//_______________________________________________________________________________


// -- METHOD

//_______________________________________________________________________________
// Looks in the name for the given pattern, and returns the result of the search
bool General::SmartName::Find( const std::string &other ) const {
  
  std::vector<std::string> vec;
  General::SplitString( vec, fName, std::string( 1, fSep ) );
  
  return std::find( vec.begin(), vec.end(), other ) != vec.end();
}
