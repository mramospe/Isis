///////////////////////////////////////////////////////////
//
//  General package
//
// --------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 20/11/2016
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
  std::string CenterString( const std::string &str, const size_t &size, const char &ch = ' ' );
  void        CheckParseOpts( const std::string              &str,
			      const std::vector<std::string> &lst = {} );
  void        ReplaceNames( std::vector<std::string> &vector,
			    std::string               istr,
			    std::string               ostr );
  void        SplitString( std::vector<std::string> &output,
			   const std::string        &str,
			   const std::string        &expr );
  void        StringVectorFilter( std::vector<std::string>       &output,
				  const std::vector<std::string> &input,
				  const std::string              &expr );
  void        TrimString( std::string &str, const std::string &trexpr = " " );
  
  //_______________________________________________________________________________
  // This function checks if the given string can be converted to a variable of the
  // type specified
  template <typename type>
  bool CheckStringType( const std::string &input ) {
    std::istringstream iss( input );
    type value;
    iss >> std::noskipws >> value;
    return iss.eof() && !iss.fail();
  }

  //_______________________________________________________________________________
  // Integer power function
  template<typename type>
  type IPow( const type &value, size_t pw ) {
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
  void ParseOpt( std::string opts, const std::string &var, type &value ) {

    size_t endpos, pos, varpos = opts.find( var );

    // If the variable is not found an error is displayed
    if ( varpos == std::string::npos ) {
      std::cerr << "ERROR: Could not parse option < " << var <<
	" >. Key not found." << std::endl;
      return;
    }
  
    // Looks for the end of the current expression
    if ( varpos == std::string::npos )
      endpos = opts.size();
    else
      endpos = opts.find( ':', varpos );
    
    // If no < = > symbol is found, it is considered that the variable is boolean. The static_cast
    // call is needed to allow taking strings as values at compilation time.
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
    TrimString( opts );

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
