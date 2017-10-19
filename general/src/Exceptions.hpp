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


#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include "Messenger.hpp"

#include <exception>
#include <string>
#include <sstream>


//_______________________________________________________________________________

namespace isis {

  //_______________________________________________________________________________
  // Base class to represent an exception
  class BaseException : public std::exception {

  public:

    // Constructor given the string to be printed and the message code
    BaseException( const std::string &info, const MsgCode &st = MsgCode::Error ) {

      std::stringstream ss;
      Messenger msg(st, &ss);
      msg << info << IEndMsg;

      fWhat = ss.str();
    }
    
    // Destructor. The output is not printed if the "fIgnore" flag is true.
    virtual ~BaseException() { if ( !fIgnore ) std::cout << this->what(); }

    // Ignore the output of this exception
    void ignore() { fIgnore = true; }
  
    const char* msg() {
    
      this->ignore();
      return this->what();
    }
  
  protected:

    // Redefinition of the method from "std::exception"
    const char* what() const throw() { return fWhat.c_str(); }

    // Store what is going to be printed
    std::string fWhat;

    // Flag to ignore the output of this exception
    bool fIgnore = false;

  };

  //_______________________________________________________________________________
  // Represent an exception to be thrown when an input argument is invalid
  class InvalidArgument : public BaseException {

  public:

    // Constructor given the invalid argument and the message code
    InvalidArgument( const std::string &arg, const MsgCode &st = MsgCode::Error ) :
      BaseException(std::string{"Invalid argument \""} + arg + std::string{"\""}, st) { }
  };

    //_______________________________________________________________________________
  // Represent an exception to be thrown when an object could not be found
  class NotFound : public BaseException {

  public:

    // Constructor given the object name, the argument and the message code
    NotFound( const std::string &objname, const std::string &arg, const MsgCode &st = MsgCode::Error ) :
      BaseException(std::string{"Unable to find "} + objname +
		    std::string{"\""} + arg + std::string{"\""}, st) { }
    
  };
  
}

#endif
