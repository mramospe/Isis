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


#include "Utils.h"


//_______________________________________________________________________________
// Calculates the length ( number of numbers ) for a given interger
size_t General::CalcIntLength( long int integer ) {
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
std::string General::CenterString( const std::string &str, const size_t &size ) {
  unsigned short int
    rst = size - str.size(),
    coc = rst / 2;
  std::string output( coc + rst % 2, ' ' );
  output += str;
  output += std::string( coc, ' ' );
  return output;
}

//_______________________________________________________________________________
// Appends to a vector the names that satisfy a given expression, being these
// located inside another vector.
void General::ParseStringMatch( std::vector<std::string>       &output,
				const std::vector<std::string> &vector,
				const std::string              &expr ) {

  std::vector<std::string> spltvec;
  General::SplitString( spltvec, expr, "*" );
  std::vector<std::string>::iterator itn;
  size_t pos;
    
  bool checkfirst = true, checklast = true, add;
  if ( spltvec.front() == "" ) {
    spltvec.erase( spltvec.begin() );
    checkfirst = false;
  }
  if ( spltvec.back() == "" ) {
    spltvec.erase( spltvec.end() - 1 );
    checklast = false;
  }
  
  for ( auto it = vector.begin(); it != vector.end(); it++ ) {
    add = true;
    pos = 0;

    itn = spltvec.begin();
    while ( itn != spltvec.end() && ( pos = it -> find( *itn, pos ) ) != std::string::npos )
      pos += itn++ -> size();
    
    if ( itn == spltvec.end() ) {
      
      if ( checkfirst && checklast )
	add = ( it -> find( spltvec.front() ) == 0 &&
		it -> rfind( spltvec.back() ) + spltvec.back().size() == it -> size() );
      else if ( checkfirst )
	add = ( it -> find( spltvec.front() ) == 0 );
      else if ( checklast )
	add = ( it -> rfind( spltvec.back() ) + spltvec.back().size() == it -> size() );
      if ( add )
	output.push_back( *it );
    }
  }
}

//_______________________________________________________________________________
// This function replaces all the substrings of the strings storaged in a vector
// with another
void General::ReplaceNames( std::vector<std::string> &vector, std::string istr, std::string ostr ) {
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
// Splits a given string < str > taking into account the expression < expr > that
// appears on it. The output is appended to the given vector.
void General::SplitString( std::vector<std::string> &output,
			   const std::string        &str,
			   const std::string        &expr ) {
  size_t strpos, pos = 0;
  while ( ( strpos = str.find( expr, pos ) ) != std::string::npos ) {
    output.push_back( str.substr( pos, strpos - pos ) );
    pos = strpos + 1;
  }
  output.push_back( str.substr( pos, str.size() - pos ) );
}

