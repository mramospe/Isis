///////////////////////////////////////////////////////////
//                                                       //
//  General package                                      //
//                                                       //
// ----------------------------------------------------- //
//                                                       //
//  AUTHOR: Miguel Ramos Pernas                          //
//  e-mail: miguel.ramos.pernas@cern.ch                  //
//                                                       //
//  Last update: 13/04/2016                              //
//                                                       //
// ----------------------------------------------------- //
//                                                       //
//  Description:                                         //
//                                                       //
//  Template file that contains some utils to work with  //
//  c++ classes.                                         //
//                                                       //
// ----------------------------------------------------- //
///////////////////////////////////////////////////////////


#ifndef __UTILS__
#define __UTILS__

#include <iostream>
#include <vector>
#include <string>
#include <sstream>


//_______________________________________________________________________________ 

namespace General {

  //_______________________________________________________________________________
  // These are the definitions of different useful functions
  size_t      CalcIntLength( long int integer );
  std::string CenterString( const std::string &str, const size_t &size );
  void        ParseStringMatch( std::vector<std::string>       &output,
				const std::vector<std::string> &vector,
				const std::string              &expr );
  void        ReplaceNames( std::vector<std::string> &vector,
			    std::string               istr,
			    std::string               ostr );
  void        SplitString( std::vector<std::string> &output,
			   const std::string        &str,
			   const std::string        &expr );

  //_______________________________________________________________________________
  // This function sets < value > to the assignement given in the expression
  // < opts > for the name < var >. The format of the string has to be like:
  // expr = "var1=2.3 : var2=4e5 : var3=S : var4 : var5=1"
  // Note:
  //  The whitespaces surrounding the < : > symbols can be removed (recommendable).
  //  Whitespaces can be written surrounding < = > symbols too.
  //  No whitespaces can be present between characters in the expression to assign.
  template<typename type>
  void ParseOpts( std::string opts, const std::string &var, type &value ) {

    size_t endpos, pos = opts.find( var );

    // If the variable is not found an error is displayed
    if ( pos == std::string::npos ) {
      std::cerr << "ERROR: Could not parse option < " << var <<
	" >. Key not found." << std::endl;
      return;
    }
  
    // Looks for the end of the current expression
    endpos = opts.find( ':', pos );
    if ( pos == std::string::npos )
      endpos = opts.size();

    // If no < = > symbol is found, it is considered that the variable is true
    pos = opts.find( '=', pos );
    if ( pos >= endpos ) {
      value = true;
      return;
    }
    ++pos;
  
    // Gets the string to parse
    opts = opts.substr( pos, endpos - pos );

    // Removes the initial and final whitespaces
    while ( opts.front() == ' ' )
      opts.erase( 0 );
    while ( opts.back() == ' ' )
      opts.erase( opts.size() - 1 );

    // If the string to be parsed has whitespaces, an error is sent
    if ( opts.find( ' ' ) != std::string::npos ) {
      std::cerr << "ERROR: The value to assign to < " << var <<
	" > has whitespaces; remove them." << std::endl;
      return;
    }
  
    // The string is converted to a stream and the value for the given variable
    // is set to its result
    std::stringstream ss( opts );
    ss >> value;
  }

  //_______________________________________________________________________________
  // Prints a given vector
  template<typename type> void PrintVector( const std::vector<type> &vector ) {
    std::stringstream ss;
    if ( vector.size() ) {
      ss << "[ " << vector[ 0 ];
      for ( auto it = vector.begin() + 1; it != vector.end(); ++it )
	ss << ", " << *it;
      ss << " ]";
    }
    else
      ss << "[]";
    std::cout << ss.str() << std::endl;
  }

  //_______________________________________________________________________________
  // Returns a given vector as a string
  template<typename type> std::string VectorToString( const std::vector<type> &vector ) {
    std::stringstream ss;
    if ( vector.size() ) {
      ss << "[ " << vector[ 0 ];
      for ( auto it = vector.begin() + 1; it != vector.end(); ++it )
	ss << ", " << *it;
      ss << " ]";
    }
    else
      ss << "[]";
    return ss.str();
  }
  
}

#endif
