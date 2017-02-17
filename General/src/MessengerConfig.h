////////////////////////////////////////////////////////////
//
//  General package
//
// ---------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 17/02/2016
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

    // Colors to format the output
    enum Colors { aBlack, aRed, aGreen, aYellow, aBlue, aMagenta, aCyan, aWhite, aNoColor };

    // Styles to format the output
    enum Styles { aNormal, aBold, aFaint, aItalic, aUnderline, aNoStyle };

    // GLOBAL variable storing whether the error output can be printed in color
    const bool ColoredStdErr = isatty( STDERR_FILENO );

    // GLOBAL variable storing whether the usual output can be printed in color
    const bool ColoredStdOut = isatty( STDOUT_FILENO );

    // GLOBAL color for the error messages
    extern int ErrorColor;

    // GLOBAL color for the information messages
    extern int InfoColor;

    // GLOBAL color for the warning messages
    extern int WarningColor;
  }

  // Function to check whether color must be applied or not
  bool ApplyColor( const std::ostream &os );

  // Function to replace < endl > for the class General::Messenger
  std::ostream& EndMsgLine( std::ostream &os );
}

//_______________________________________________________________________________
// Definition of the objects to display the basic types of messages
#define IBegMsg  General::Messenger("--- "     , std::cout, General::ANSIFormat::aNoColor)
#define IError   General::Messenger("ERROR: "  , std::cerr, General::ANSIFormat::ErrorColor)
#define IInfo    General::Messenger("INFO: "   , std::cout, General::ANSIFormat::InfoColor)
#define IWarning General::Messenger("WARNING: ", std::cout, General::ANSIFormat::WarningColor)
#define IEndMsg  General::EndMsgLine

#endif
