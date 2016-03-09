///////////////////////////////////////////////////////////
//                                                       //
//  General package                                      //
//                                                       //
// ----------------------------------------------------- //
//                                                       //
//  AUTHOR: Miguel Ramos Pernas                          //
//  e-mail: miguel.ramos.pernas@cern.ch                  //
//                                                       //
//  Last update: 09/03/2016                              //
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
  void        ReplaceNames( std::vector<std::string> &vector, std::string istr, std::string ostr );
  void        SplitString( std::vector<std::string> &output,
			   const std::string        &str,
			   const std::string        &expr );
  
  //_______________________________________________________________________________
  // Prints a given vector
  template<typename type> void PrintVector( const std::vector<type> &vector ) {
    std::stringstream ss;
    if ( vector.size() ) {
      ss << "[ " << vector[ 0 ];
      for ( typename std::vector<type>::iterator it = vector.begin() + 1;
	    it != vector.end();
	    it++ )
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
      for ( typename std::vector<type>::iterator it = vector.begin() + 1;
	    it != vector.end();
	    it++ )
	ss << ", " << *it;
      ss << " ]";
    }
    else
      ss << "[]";
    return ss.str();
  }
  
}

#endif
