////////////////////////////////////////////////////////////
//
//  General package
//
// ---------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 14/02/2016
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


#include "GlobalMessenger.h"

#include <iostream>


using namespace General::ANSIFormat;


//_______________________________________________________________________________
// Default values for the colors in the output messages
int General::ANSIFormat::ErrorColor   = aRed;
int General::ANSIFormat::InfoColor    = aGreen;
int General::ANSIFormat::WarningColor = aYellow;

//_______________________________________________________________________________
// Formats the input message following whe given foreground and background colors
// and style. Only the message and foreground color are mandatory.
std::string General::FormatMsg( const std::string &msg,
				const int &fgcol,
				const int &bgcol,
				const int &sty ) {

  std::string prestr = "\033[";

  if ( fgcol > -1 )
    prestr += ";3" + std::to_string( fgcol );
  if ( bgcol > -1 )
    prestr += ";4" + std::to_string( bgcol );
  if ( sty > -1 )
    prestr += ';' + std::to_string( sty );
  
  return prestr + "m" + msg + "\033[0m";
}

//_______________________________________________________________________________
// Send a message using the given input information
void General::SendMsg( std::ostream &os,
		       const std::string &pre,
		       const std::string &msg,
		       const bool &color_enabled,
		       const int &color ) {
  
  std::string frmsg = pre + msg;
  if ( color_enabled )
    frmsg = FormatMsg( frmsg, color );

  os << frmsg << std::endl;
}

//_______________________________________________________________________________
// Function to send an error message
void General::SendErrorMsg( const std::string &msg ) {

  SendMsg( std::cerr, "ERROR: ", msg, ColoredStdErr, ErrorColor );
}

//_______________________________________________________________________________
// Function to send an information message
void General::SendInfoMsg( const std::string &msg ) {

  SendMsg( std::cout, "INFO: ", msg, ColoredStdOut, InfoColor );
}

//_______________________________________________________________________________
// Function to send a warning message
void General::SendWarningMsg( const std::string &msg ) {

  SendMsg( std::cout, "WARNING: ", msg, ColoredStdOut, WarningColor );
}

