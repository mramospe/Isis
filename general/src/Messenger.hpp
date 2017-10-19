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
//  Definition of the main class to print information into
//  a terminal/file. It can be formatted so one can put
//  colors and change the style of the words being
//  displayed.
//
// ---------------------------------------------------------
////////////////////////////////////////////////////////////


#ifndef MESSENGER_HPP
#define MESSENGER_HPP

#include "MessengerConfig.hpp"

#include <cstdint>
#include <iostream>


//_______________________________________________________________________________

namespace isis {
  
  class Messenger {
  
  public:

    // Constructor given the start message, the output stream and the
    // configuration colors and style
    Messenger( const std::string &start,
	       std::ostream &os,
	       const int &fgcol = ANSIFormat::aNoColor,
	       const int &bgcol = ANSIFormat::aNoColor,
	       const int &sty = ANSIFormat::aNoStyle ) {
      
      this->build(start, os, fgcol, bgcol, sty);
    }

    // Constructor given a status code
    Messenger( const short unsigned int &st, std::ostream *os = 0 );
    
    // Destructor
    ~Messenger() { }

    // Send object to the output stream
    template<class type>
    Messenger& operator << ( const type& obj );
    
  protected:

    // Output stream
    std::ostream *fOStream;

  private:

    // Build the object
    void build( const std::string &start,
		std::ostream &os,
		const int &fgcol,
		const int &bgcol = ANSIFormat::aNoColor,
		const int &sty = ANSIFormat::aNoStyle );
    
  };

  // Stream operator
  template<class type>
  inline Messenger& Messenger::operator << ( const type& obj ) {
    *fOStream << obj;
    return *this;
  }

}

//_______________________________________________________________________________
// Definition of the objects to display the basic types of messages
#define IBegMsg  isis::Messenger(isis::MsgCode::Msg)
#define IError   isis::Messenger(isis::MsgCode::Error)
#define IInfo    isis::Messenger(isis::MsgCode::Info)
#define IWarning isis::Messenger(isis::MsgCode::Warning)
#define IEndMsg  isis::endMsgLine

#endif
