///////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 20/06/2017
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#include "Definitions.h"
#include "SmartName.h"
#include "Utils.h"

#include <algorithm>
#include <vector>


//_______________________________________________________________________________

namespace isis {

  //_______________________________________________________________________________
  //
  SmartName::SmartName( const std::string &str, const char &sep ) :
    NamedObject(str),
    fSep( sep ) { }

  //_______________________________________________________________________________
  //
  SmartName::~SmartName() { }

  //_______________________________________________________________________________
  //
  bool SmartName::find( const std::string &other ) const {
  
    Strings vec;
    splitString( vec, fName, std::string( 1, fSep ) );
  
    return std::find( vec.begin(), vec.end(), other ) != vec.end();
  }

}
