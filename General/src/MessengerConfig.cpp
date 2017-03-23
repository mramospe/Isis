////////////////////////////////////////////////////////////
//
//  General package
//
// ---------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 23/03/2017
//
// ---------------------------------------------------------
////////////////////////////////////////////////////////////


#include "Definitions.h"
#include "Messenger.h"
#include "MessengerConfig.h"

#include <iostream>


//_______________________________________________________________________________

namespace Isis {

  //_______________________________________________________________________________
  //
  int ANSIFormat::ErrorColor   = aRed;
  int ANSIFormat::InfoColor    = aGreen;
  int ANSIFormat::WarningColor = aYellow;

  //_______________________________________________________________________________
  //
  bool applyColor( const std::ostream &os ) {

    if ( &os == &std::cout )
      return ANSIFormat::ColoredStdOut;
    else if ( &os == &std::cerr )
      return ANSIFormat::ColoredStdErr;
    else
      return false;
  }

  //_______________________________________________________________________________
  //
  std::ostream& endMsgLine( std::ostream &os ) {

    switch( applyColor(os) ) {
    case true:
      os << "\033[0m";
    default:
      NOOP;
    }
    
    os << std::endl;
  
    return os;
  }

}
