////////////////////////////////////////////////////////////
//
//  General package
//
// ---------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 16/02/2016
//
// ---------------------------------------------------------
//
//  Description:
//
//  Provides variables and functions to format the output
//  of any program. This configuration is used by the class
//  Messenger. The colors and styles are based on the ANSI
//  escape code format.
//
// ---------------------------------------------------------
////////////////////////////////////////////////////////////


#ifndef MESSENGER_CONFIG
#define MESSENGER_CONFIG

#include <cstdio>
#include <string>
#include <unistd.h>


//_______________________________________________________________________________

namespace General {
  
  namespace ANSIFormat {
    
    enum Colors { aBlack, aRed, aGreen, aYellow, aBlue, aMagenta, aCyan, aWhite, aNoColor };
    enum Styles { aNormal, aBold, aFaint, aItalic, aUnderline, aNoStyle };

    const bool ColoredStdErr = isatty( STDERR_FILENO );
    const bool ColoredStdOut = isatty( STDOUT_FILENO );

    // Default GLOBAL values for the colors in the output messages
    extern int ErrorColor;
    extern int InfoColor;
    extern int WarningColor;
  }

  // Function to check whether color must be applied or not
  bool ApplyColor( const std::ostream &os );

  // Function to replace < endl > for the class General::Messenger
  std::ostream& EndMsgLine( std::ostream &os );
}

//_______________________________________________________________________________
// Definition of the objects to display the basic types of messages
#define BegMsg  General::Messenger("--- "     , std::cout, General::ANSIFormat::aNoColor)
#define Error   General::Messenger("ERROR: "  , std::cerr, General::ANSIFormat::ErrorColor)
#define Info    General::Messenger("INFO: "   , std::cout, General::ANSIFormat::InfoColor)
#define Warning General::Messenger("WARNING: ", std::cout, General::ANSIFormat::WarningColor)
#define EndMsg  General::EndMsgLine

#endif
