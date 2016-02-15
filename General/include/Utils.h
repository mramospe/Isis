///////////////////////////////////////////////////////////
//                                                       //
//  General package                                      //
//                                                       //
// ----------------------------------------------------- //
//                                                       //
//  AUTHOR: Miguel Ramos Pernas                          //
//  e-mail: miguel.ramos.pernas@cern.ch                  //
//                                                       //
//  Last update: 15/02/2016                              //
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
  // Calculates the length ( number of numbers ) for a given interger
  size_t CalcIntLength( long int integer ) {
    size_t size = 0;
    if ( integer > 0 )
      while( integer >= 1 ) {
	integer /= 10;
	size++;
      }
    else
      while ( integer <= -1 ) {
	integer /= 10;
	size++;
      }
    return size;
  }

  //_______________________________________________________________________________
  // This function allows to center a string in a place with width equal to
  // < size >. In case of working with odd sizes the right hand will always be
  // greater.
  std::string CenterString( const std::string &str, const size_t &size ) {
    unsigned short int
      rst = size - str.size(),
      coc = rst / 2;
    std::string output( coc + rst % 2, ' ' );
    output += str;
    output += std::string( coc, ' ' );
    return output;
  }

  //_______________________________________________________________________________
  // This function replaces all the substrings of the strings storaged in a vector
  // with another
  void ReplaceNames( std::vector<std::string> &vector,
		     std::string               istr,
		     std::string               ostr ) {

    size_t pos;
    for ( std::vector<std::string>::iterator it = vector.begin();
	  it != vector.end();
	  it++ ) {
      pos = it -> find( istr );
      if ( pos != std::string::npos )
	it -> replace( pos, istr.length(), ostr );
    }
  }

  //_______________________________________________________________________________
  // This function allows to merge two vector so the output vector is the result
  // of the concatenation of both
  template<typename type> std::vector<type> MergeVectors( std::vector<type> vector1, std::vector<type> vector2 ) {

    std::vector<type> ovector( vector1.size() + vector2.size() );
    typename std::vector<type>::iterator
      it1 = vector1.begin(),
      it2 = vector2.begin(),
      oit = ovector.begin();
    
    for ( true; it1 != vector1.end(); it1++, oit++ )
      *oit = *it1;
    for ( true; it2 != vector2.end(); it2++, oit++ )
      *oit = *it2;

    return ovector;
  }
  
  //_______________________________________________________________________________
  // Returns a given vector as a string
  template<typename type> std::string VectorToString( std::vector<type> &vector ) {
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
  
  //_______________________________________________________________________________
  // Prints a given vector
  template<typename type> void PrintVector( std::vector<type> &vector ) {
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

}

#endif
