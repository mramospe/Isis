////////////////////////////////////////////////////////////
//
//  General package
//
// ---------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 21/03/2017
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
  bool ApplyColor( const std::ostream &os ) {

    if ( &os == &std::cout )
      return ANSIFormat::ColoredStdOut;
    else if ( &os == &std::cerr )
      return ANSIFormat::ColoredStdErr;
    else
      return false;
  }

  //_______________________________________________________________________________
  //
  std::ostream& EndMsgLine( std::ostream &os ) {

    switch( ApplyColor(os) ) {
    case true:
      os << "\033[0m";
    default:
      NOOP;
    }
    
    os << std::endl;
  
    return os;
  }

}
