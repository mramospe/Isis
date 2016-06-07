///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  General package                                                              //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  AUTHOR: Miguel Ramos Pernas                                                  //
//  e-mail: miguel.ramos.pernas@cern.ch                                          //
//                                                                               //
//  Last update: 07/06/2016                                                      //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  Description:                                                                 //
//                                                                               //
//  Implements a class to help adding strings separated by one character. This   //
//  is useful, for example, to manage paths or file names, which are separated   //
//  by < / > and < _ > symbols respectively. When two classes of this type are   //
//  added, the separator is taken to that of the right hand in the addition.     //
//                                                                               //
// ----------------------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////////////////


#ifndef SMART_NAME
#define SMART_NAME

#include <string>
#include <vector>


//_______________________________________________________________________________

namespace General {

  class SmartName {

  public:

    // Constructor and destructor
    SmartName( const std::string &str = std::string(), const char &sep = '_' );
    ~SmartName();

    // Method
    bool Find( const std::string &other ) const;

    // Inline methods
    inline std::string        BuildFilePath( const std::string &ext );
    inline const std::string& GetName() const;

    // Operators
    inline SmartName operator +  ( const std::string &str ) const;
    inline SmartName operator +  ( const SmartName &str ) const;
    inline void      operator += ( const std::string &str );
    inline void      operator += ( const SmartName &str );
    inline void      operator =  ( const SmartName &str );
    inline void      operator =  ( const std::string &str );
    inline bool      operator == ( const SmartName &other ) const;
    inline bool      operator == ( const std::string &other ) const;
    inline bool      operator != ( const SmartName &other ) const;
    inline bool      operator != ( const std::string &other ) const;
    
  protected:
    
    // Attributes
    std::string fName;
    char        fSep;
    
  };
  
  // _________________
  // -- INLINE METHODS

  // Builds the complete path to a file, given the type of the file (without the dot)
  inline std::string SmartName::BuildFilePath( const std::string &ext ) { return fName + "." + ext; }
  // Returns the current name of the class
  inline const std::string& SmartName::GetName() const { return fName; }
  
  // _________________
  // -- OPERATORS
  
  inline SmartName SmartName::operator + ( const std::string &str ) const {
    return SmartName( fName + fSep + str, fSep );
  }
  inline SmartName SmartName::operator + ( const SmartName &str ) const {
    return SmartName( fName + fSep + str.fName, str.fSep );
  }
  inline void SmartName::operator += ( const std::string &str ) { fName += fSep + str; }
  inline void SmartName::operator += ( const SmartName &str ) {
    fName += fSep + str.fName;
    fSep   = str.fSep;
  }
  inline void SmartName::operator =  ( const std::string &str ) {
    fName = str;
    fSep  = '_';
  }
  inline void SmartName::operator =  ( const SmartName &str ) {
    fName = str.fName;
    fSep  = str.fSep;
  }
  inline bool SmartName::operator == ( const std::string &other ) const { return fName == other; }
  inline bool SmartName::operator == ( const SmartName &other ) const { return fName == other.fName; }
  inline bool SmartName::operator != ( const std::string &other ) const { return fName != other; }
  inline bool SmartName::operator != ( const SmartName &other ) const { return fName != other.fName; }
  std::ostream& operator << ( std::ostream &os, const SmartName &name );

}

#endif
