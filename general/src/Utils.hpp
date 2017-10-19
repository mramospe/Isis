///////////////////////////////////////////////////////////
//
//  General package
//
// --------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 18/09/2017
//
// --------------------------------------------------------
//
//  Description:
//
//  Template file that contains some utils to work with
//  c++ classes.
//
// --------------------------------------------------------
///////////////////////////////////////////////////////////


#ifndef UTILS_H
#define UTILS_H

#include "Definitions.hpp"
#include "Exceptions.hpp"
#include "Messenger.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <sstream>


//_______________________________________________________________________________ 

namespace isis {

  // Calculates the length ( number of numbers ) for a given interger
  size_t calcIntLength( long int integer );

  // This function allows to center a string in a place with width equal to
  // < size >. In case of working with odd sizes the right hand will always be
  // greater.
  std::string centerString( const std::string &str,
			    const size_t      &size,
			    const char        &ch = ' ' );

  // Checks the current expression to see if it can be pased. If a list of strings
  // is given, a check is made to see if all the names in the current expression
  // are contained in such list.
  void checkParseOpts( const std::string &str, const Strings &lst = {} );

  // This function replaces all the substrings of the strings storaged in a vector
  // with another
  void replaceNames( Strings &vector, std::string istr, std::string ostr );

  // Splits a given string < str > taking into account the expression < expr > that
  // appears on it. The output is appended to the given vector.
  void splitString( Strings &output,
		    const std::string &str,
		    const std::string &expr );

  // Filters the entries in the given vector following the rules imposed by the
  // input variable < regex >. The strings that pass the filter are attached to the
  // output vector.
  void stringVectorFilter( Strings &output,
			   const Strings &input,
			   const std::string &regex );

  // Trims the given string using < trexpr >. All the elements that match this
  // expression in the front and back of < str > will be removed.
  void trimString( std::string &str, const std::string &trexpr = " " );
  
  //_______________________________________________________________________________
  // This function checks if the given string can be converted to a variable of the
  // type specified
  template <typename type>
  bool checkStringType( const std::string &input ) {
    std::istringstream iss( input );
    type value;
    iss >> std::noskipws >> value;
    return iss.eof() && !iss.fail();
  }

  //_______________________________________________________________________________
  // Integer power function
  template<typename type>
  type ipow( const type &value, size_t pw ) {
    type res = 1;
    while ( pw-- )
      res *= value;
    return res;
  }

  //_______________________________________________________________________________
  // This function sets < value > to the assignement given in the expression
  // < opts > for the name < var >. The format of the string has to be like:
  // expr = "var1=2.3 : var2=4e5 : var3=S : var4 : var5=1"
  // Note:
  //  The whitespaces surrounding the < : > symbols can be removed (recommendable).
  //  Whitespaces can be written surrounding < = > symbols too.
  //  No whitespaces can be present between characters in the expression to assign.
  template<typename type>
  void parseOpt( std::string opts, const std::string &var, type &value ) {

    size_t endpos, pos, varpos = opts.find(var);

    // If the variable is not found an error is displayed
    if ( varpos == std::string::npos ) {
      IWarning << "Could not parse option \"" << var <<
	"\". Key not found." << IEndMsg;
      return;
    }
  
    // Looks for the end of the current expression
    if ( varpos == std::string::npos )
      endpos = opts.size();
    else
      endpos = opts.find(':', varpos);
    
    // If no < = > symbol is found, it is considered that the variable is boolean. The
    // static_cast call is needed to allow taking strings as values at compilation time.
    pos = opts.find( '=', varpos );
    if ( pos >= endpos ) {
      bool res = true;
      if ( varpos )
	while ( opts.at( --varpos ) == '!' ) {
	  opts.erase( varpos );
	  res = !res;
	}
      void *auxptr = &res;
      value = *static_cast<type*>( auxptr );
      return;
    }
    ++pos;
    
    // Gets the string to parse removing the initial and final whitespaces
    opts = opts.substr( pos, endpos - pos );
    trimString( opts );

    // If the string to be parsed has whitespaces, an error is sent
    if ( opts.find( ' ' ) != std::string::npos )
      throw BaseException("The value to assign to \"" + var +
			  "\" has whitespaces; remove them.");
  
    // The string is converted to a stream and the value for the given variable
    // is set to its result
    std::stringstream ss( opts );
    ss >> value;
  }

  //_______________________________________________________________________________
  // Returns a given container as a string
  template<template <class ...> class cont, class type>
  std::string contToString( const cont<type> &container ) {
    
    std::stringstream ss;
    if ( container.size() ) {
      ss << '[' << container[ 0 ];
      for ( auto it = std::begin(container) + 1; it != std::end(container); ++it )
	ss << ", " << *it;
      ss << ']';
    }
    else
      ss << "[]";
    return ss.str();
  }
  
}

#endif
