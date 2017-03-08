////////////////////////////////////////////////////////////
//
//  General package
//
// ---------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 07/03/2017
//
// ---------------------------------------------------------
////////////////////////////////////////////////////////////


#include "Definitions.h"
#include "Messenger.h"
#include "MessengerConfig.h"

#include <iostream>


using namespace General::ANSIFormat;


//_______________________________________________________________________________
//
int General::ANSIFormat::ErrorColor   = aRed;
int General::ANSIFormat::InfoColor    = aGreen;
int General::ANSIFormat::WarningColor = aYellow;

//_______________________________________________________________________________
//
bool General::ApplyColor( const std::ostream &os ) {

  if ( &os == &std::cout )
    return General::ANSIFormat::ColoredStdOut;
  else if ( &os == &std::cerr )
    return General::ANSIFormat::ColoredStdErr;
  else
    return false;
}

//_______________________________________________________________________________
//
std::ostream& General::EndMsgLine( std::ostream &os ) {

  switch( ApplyColor(os) ) {
  case true:
    os << "\033[0m";
  default:
    NOOP;
  }
    
  os << std::endl;
  
  return os;
}
