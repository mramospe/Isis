////////////////////////////////////////////////////////////
//
//  General package
//
// ---------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 05/10/2016
//
// ---------------------------------------------------------
//
//  Description:
//
//  Provides variables and functions to format the output
//  of any program. The colors and styles are based on
//  the ANSI escape code format.
//
// ---------------------------------------------------------
////////////////////////////////////////////////////////////


#ifndef GLOBAL_MESSENGER
#define GLOBAL_MESSENGER

#include <cstdio>
#include <string>
#include <unistd.h>


//_______________________________________________________________________________

namespace General {
  
  namespace ANSIFormat {
    
    enum Colors { aBlack, aRed, aGreen, aYellow, aBlue, aMagenta, aCyan, aWhite };
    enum Styles { aNormal, aBold, aFaint, aItalic, aUnderline };

    const bool ColoredStdErr = isatty( STDERR_FILENO );
    const bool ColoredStdOut = isatty( STDOUT_FILENO );

    extern int ErrorColor;
    extern int InfoColor;
    extern int WarningColor;
  }
  
  std::string FormatMsg( const std::string &msg,
			 const int &fgcol,
			 const int &bgcol = -1,
			 const int &sty   = -1 );
  void SendErrorMsg( const std::string &msg );
  void SendInfoMsg( const std::string &msg );
  void SendWarningMsg( const std::string &msg );
  
}

#endif
