///////////////////////////////////////////////////////////
//                                                       //
//  General package                                      //
//                                                       //
// ----------------------------------------------------- //
//                                                       //
//  AUTHOR: Miguel Ramos Pernas                          //
//  e-mail: miguel.ramos.pernas@cern.ch                  //
//                                                       //
//  Last update: 22/06/2016                              //
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

#include <algorithm>


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
std::string General::CenterString( const std::string &str, const size_t &size, const char &ch ) {
  unsigned short int
    rst = size - str.size(),
    coc = rst / 2;
  std::string output( coc + rst % 2, ch );
  output += str;
  output += std::string( coc, ch );
  return output;
}

//_______________________________________________________________________________
// Checks the current expression to see if it can be pased. If a list of strings
// is given, a check is made to see if all the names in the current expression
// are contained in such list.
void General::CheckParseOpts( const std::string              &str,
			      const std::vector<std::string> &lst ) {

  std::string name, opt;
  size_t pos;

  // Splits the string to get the different options
  std::vector<std::string> splt;  
  General::SplitString( splt, str, ":" );

  // Performs the loop over the splitted options to verify that they are
  // correctly written
  for ( auto it = splt.begin(); it != splt.end(); ++it ) {
    if ( ( pos = it -> find( '=' ) ) != std::string::npos ) {
      name = it -> substr( 0, pos );
      opt  = it -> substr( pos + 1 );
      General::TrimString( name );
      General::TrimString( opt );
    }
    else {
      name = *it;
      opt  = std::string();
      General::TrimString( name );
      while ( name.front() == '!' )
	name.erase( name.begin() );
    }
    
    // Checks if the name of the option is in the given list
    if ( lst.size() && std::find( lst.begin(), lst.end(), name ) == lst.end() )
      std::cerr << "WARNING: Option for < " << name <<
	" > not known; check the input options." << std::endl;
    
    // Checks if the option given has whitespaces or not
    if ( opt.find( ' ' ) != std::string::npos ) {
      std::cerr << "ERROR: Option < " << name << 
	" > has whitespaces on its value" << std::endl;
      return;
    }
  }
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

//_______________________________________________________________________________
// Filters the entries in the given vector following the rules imposed by the
// input variable < expr >. The strings that pass the filter are attached to the
// output vector.
void General::StringVectorFilter( std::vector<std::string>       &output,
				  const std::vector<std::string> &input,
				  const std::string              &expr ) {

  // Splits the input string using the character < * >
  std::vector<std::string> splitVec;
  General::SplitString( splitVec, expr, "*" );

  // Defines two boolean variables that determine if the first or the last word provided
  // in the expression must be checked
  bool
    checkstart = true,
    checkend   = true;
  if ( splitVec.front() == std::string() )
    checkstart = false;
  if ( splitVec.back() == std::string() )
    checkend = false;

  // The splitted vector will have empty values in the first and last entries depending
  // if the < * > symbol is placed in the expression extremes, so they must be removed
  splitVec.assign( splitVec.begin() + !checkstart, splitVec.end() - !checkend );
  
  // Adds each string to the new vector if they satisfy the requirements imposed by < expr >
  for ( auto it = input.begin(); it != input.end(); ++it ) {
    
    size_t
      prevPos = 0,
      nextPos = 0;

    bool cond1 = true, cond2 = true;

    // If the start or the end does not coincide with the expected, the condition variable
    // is set to < false >, so it does not enter into the following loop
    if ( checkstart )
      if ( it -> find( splitVec.front() ) != 0 )
	cond1 = false;
    if ( checkend ) {
      size_t pos = (it -> size() - splitVec.back().size());
      if ( it -> rfind( splitVec.back() ) != pos )
	cond1 = false;
    }
    
    for ( auto itkw = splitVec.begin(); itkw != splitVec.end() && cond1 && cond2; ++itkw ) {

      nextPos = it -> find( *itkw );
      
      cond1 = (nextPos >= prevPos);
      cond2 = (nextPos != std::string::npos);

      prevPos = nextPos;
    }

    // Only if the iterator has reached the end the string is saved
    if ( cond1 && cond2 )
      output.push_back( *it );
  }
}

//_______________________________________________________________________________
// Trims the given string using < trexpr >. All the elements that match this
// expression in the front and back of < str > will be removed.
void General::TrimString( std::string &str, const std::string &trexpr ) {
  size_t pos = str.find_first_not_of( trexpr );
  str.erase( 0, pos );
  pos = str.find_last_not_of( trexpr ) + 1;
  str.erase( pos );
}
