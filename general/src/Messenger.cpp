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
////////////////////////////////////////////////////////////


#include "Definitions.hpp"
#include "Exceptions.hpp"
#include "Messenger.hpp"
#include "MessengerConfig.hpp"


//_______________________________________________________________________________

namespace isis {
  
  //_______________________________________________________________________________
  //
  Messenger::Messenger( const short unsigned int &st, std::ostream *os ) {

    switch ( st ) {
      
    case ( MsgCode::Msg ):
      this->build(ANSIFormat::MsgPrefix, os ? *os : std::cout, ANSIFormat::aNoColor);
      break;
    case ( MsgCode::Info ):
      this->build(ANSIFormat::InfoPrefix, os ? *os : std::cout, ANSIFormat::InfoColor);
      break;
    case ( MsgCode::Warning ):
      this->build(ANSIFormat::WarningPrefix, os ? *os : std::cout, ANSIFormat::WarningColor);
      break;
    case ( MsgCode::Error ):
      this->build(ANSIFormat::ErrorPrefix, os ? *os : std::cerr, ANSIFormat::ErrorColor);
      break;
    default:
      throw BaseException("Unknown message code \"" + std::to_string(st) + "\"");
    }
  }

  //_______________________________________________________________________________
  //
  void Messenger::build( const std::string &start,
			 std::ostream &os,
			 const int &fgcol,
			 const int &bgcol,
			 const int &sty ) {

    fOStream = &os;
    
    // Check if color must be applied
    if ( applyColor(os) ) {
    
      *fOStream << "\033[";

      // Configure foreground color
      switch ( fgcol ) {
      
      case ANSIFormat::aNoColor:
	NOOP;
      
      default:
	*fOStream << ";3" << std::to_string( fgcol );
      }

      // Configure background color
      switch ( bgcol ) {
      case ANSIFormat::aNoColor:
	NOOP;
      default:
	*fOStream << ";4" << std::to_string( bgcol );
      }

      // Configure style
      switch ( sty ) {
      case ANSIFormat::aNoStyle:
	NOOP;
      default:
	*fOStream << ";" << std::to_string( sty );
      }
      
      *fOStream << "m";
    }

    *fOStream << start;
  }

}
