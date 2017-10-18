////////////////////////////////////////////////////////////
//
//  General package
//
// ---------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 18/10/2017
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

namespace isis {
  
  namespace ANSIFormat {

    // Colors to format the output
    enum Colors { aBlack, aRed, aGreen, aYellow, aBlue, aMagenta, aCyan, aWhite, aNoColor };

    // Styles to format the output
    enum Styles { aNormal, aBold, aFaint, aItalic, aUnderline, aNoStyle };

    // GLOBAL variable storing whether the error output can be printed in color
    extern const bool ColoredStdErr;
    
    // GLOBAL variable storing whether the usual output can be printed in color
    extern const bool ColoredStdOut;

    // GLOBAL color for the error messages
    extern int ErrorColor;

    // GLOBAL color for the information messages
    extern int InfoColor;

    // GLOBAL color for the warning messages
    extern int WarningColor;
  }

  // Function to check whether color must be applied or not
  bool applyColor( const std::ostream &os );

  // Function to replace < endl > for the class isis::Messenger
  std::ostream& endMsgLine( std::ostream &os );
}

#endif
