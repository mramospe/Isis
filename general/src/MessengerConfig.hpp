////////////////////////////////////////////////////////////
//
//  General package
//
// ---------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 19/10/2017
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


#ifndef MESSENGER_CONFIG_HPP
#define MESSENGER_CONFIG_HPP

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

    // GLOBAL prefix for the error messages
    extern std::string ErrorPrefix;

    // GLOBAL color for the information messages
    extern int InfoColor;

    // GLOBAL prefix for the error messages
    extern std::string InfoPrefix;

    // GLOBAL prefix for the messages
    extern std::string MsgPrefix;

    // GLOBAL color for the warning messages
    extern int WarningColor;

    // GLOBAL prefix for the error messages
    extern std::string WarningPrefix;
  }

  // Status code of a message
  enum MsgCode { Msg, Info, Warning, Error };
  
  // Function to check whether color must be applied or not
  bool applyColor( const std::ostream &os );

  // Function to replace < endl > for the class isis::Messenger
  std::ostream& endMsgLine( std::ostream &os );
}

#endif
