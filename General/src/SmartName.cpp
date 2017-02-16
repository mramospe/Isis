///////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 16/02/2017
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#include "SmartName.h"
#include "Utils.h"

#include <algorithm>
#include <vector>


//_______________________________________________________________________________
//
General::SmartName::SmartName( const std::string &str, const char &sep ) :
  fName( str ), fSep( sep ) { }

//_______________________________________________________________________________
//
General::SmartName::~SmartName() { }

//_______________________________________________________________________________
//
bool General::SmartName::Find( const std::string &other ) const {
  
  std::vector<std::string> vec;
  General::SplitString( vec, fName, std::string( 1, fSep ) );
  
  return std::find( vec.begin(), vec.end(), other ) != vec.end();
}
