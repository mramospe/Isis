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
//  Definition of the main class to print information into
//  a terminal/file. It can be formatted so one can put
//  colors and change the style of the words being
//  displayed.
//
// ---------------------------------------------------------
////////////////////////////////////////////////////////////


#ifndef __MESSENGER__
#define __MESSENGER__

#include "Definitions.h"
#include "MessengerConfig.h"

#include <cstdint>
#include <iostream>


namespace General {
  
  class Messenger {
  
  public:

    // Constructor given the start message, the output stream and the
    // configuration colors and style
    Messenger(const std::string &start,
	      std::ostream &os,
	      const int &fgcol = ANSIFormat::aNoColor,
	      const int &bgcol = ANSIFormat::aNoColor,
	      const int &sty = ANSIFormat::aNoStyle);
    
    // Destructor
    ~Messenger();

    // Send object to the output stream
    template<class type>
    Messenger& operator << ( const type& obj );
    
  protected:

    // Output stream
    std::ostream *fOStream;

  };

  // Stream operator
  template<class type>
  inline Messenger& Messenger::operator << ( const type& obj ) {
    *fOStream << obj;
    return *this;
  }

}

#endif
