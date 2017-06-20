///////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 20/06/2017
//
// --------------------------------------------------------------------------------
//
//  Description:
//
//  Implements a class to help adding strings separated by one character. This
//  is useful, for example, to manage paths or file names, which are separated
//  by < / > and < _ > symbols respectively. When two classes of this type are
//  added, the separator is taken to that of the right hand in the addition.
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#ifndef SMARTNAME_H
#define SMARTNAME_H

#include "NamedObject.h"

#include <string>
#include <vector>


//_______________________________________________________________________________

namespace isis {

  class SmartName : public NamedObject {

  public:

    // Constructor
    SmartName( const std::string &str = std::string(), const char &sep = '_' );

    // Destructor
    ~SmartName();

    // Looks in the name for the given pattern, and returns the result of the search
    bool find( const std::string &other ) const;

    // Builds the complete path to a file, given the type of the file (without the dot)
    inline std::string buildFilePath( const std::string &ext );
    
    // Add a string
    inline SmartName operator + ( const std::string &str ) const;

    // Add another SmartName
    inline SmartName operator + ( const SmartName &str ) const;

    // Add string to this class
    inline void operator += ( const std::string &str );

    // Add another SmartName to this class
    inline void operator += ( const SmartName &str );

    // Set this class from a SmartName
    inline void operator = ( const SmartName &str );

    // Set this class from a string
    inline void operator = ( const std::string &str );

    // Compare this name to another SmartName
    inline bool operator == ( const SmartName &other ) const;

    // Compare this name to a string
    inline bool operator == ( const std::string &other ) const;

    // Negative comparison respect to another SmartName
    inline bool operator != ( const SmartName &other ) const;

    // Negative comparison respect to a string
    inline bool operator != ( const std::string &other ) const;

    // Send the name to a stream
    inline std::ostream& operator << ( std::ostream &os ) const;
    
  protected:
    
    // Character separating each part of the name
    char fSep;
    
  };
  
  //_______________________________________________________________________________
  //
  inline std::string SmartName::buildFilePath( const std::string &ext ) {
    return fName + "." + ext;
  }

  //_______________________________________________________________________________
  //
  inline SmartName SmartName::operator + ( const std::string &str ) const {

    return SmartName( fName + fSep + str, fSep );
  }
  
  //_______________________________________________________________________________
  //
  inline SmartName SmartName::operator + ( const SmartName &str ) const {
    
    return SmartName( fName + fSep + str.fName, str.fSep );
  }

  //_______________________________________________________________________________
  //
  inline void SmartName::operator += ( const std::string &str ) { fName += fSep + str; }

  //_______________________________________________________________________________
  //
  inline void SmartName::operator += ( const SmartName &str ) {
    
    fName += fSep + str.fName;
    fSep   = str.fSep;
  }

  //_______________________________________________________________________________
  //
  inline void SmartName::operator =  ( const std::string &str ) {
    
    fName = str;
    fSep  = '_';
  }

  //_______________________________________________________________________________
  //
  inline void SmartName::operator =  ( const SmartName &str ) {
    
    fName = str.fName;
    fSep  = str.fSep;
  }

  //_______________________________________________________________________________
  //
  inline bool SmartName::operator == ( const std::string &other ) const {
    
    return fName == other;
  }

  //_______________________________________________________________________________
  //
  inline bool SmartName::operator == ( const SmartName &other ) const {
    
    return fName == other.fName;
  }

  //_______________________________________________________________________________
  //
  inline bool SmartName::operator != ( const std::string &other ) const {
    
    return fName != other;
  }

  //_______________________________________________________________________________
  //
  inline bool SmartName::operator != ( const SmartName &other ) const {
    
    return fName != other.fName;
  }

  //_______________________________________________________________________________
  //
  inline std::ostream& SmartName::operator << ( std::ostream &os ) const {
    
    os << fName;
    return os;
  }
}

#endif
