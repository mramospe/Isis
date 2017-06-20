////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// -------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 20/06/2017
//
// -------------------------------------------------------------------------
//
//  Description:
//
//  Define a simple class with a name
//
// -------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////


#ifndef NAMEDOBJECT_H
#define NAMEDOBJECT_H

#include <string>


//__________________________________________________________________________

namespace isis {

  class NamedObject {

  public:

    // Constructor given the name
    NamedObject( const std::string &name = std::string() ) : fName(name) { }

    // Destructor
    ~NamedObject() { }

    // Set the name of this class
    void setName( const std::string &name ) {

      fName = name;
    }

    // Return the name
    std::string getName() const {

      return fName;
    }
    
  protected:

    // Stored name
    std::string fName;

  };

}

#endif
