///////////////////////////////////////////////////////////////////////
//
//  General package
//
// --------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 13/10/2016
//
// --------------------------------------------------------------------
//
//  Description:
//
//  This class facilitates the specification of the input variables
//  for a c++ executable. The types of the variables are defined
//  via the < BookConfigOpt > method. They can be doubles (D),
//  integers (I), or strings (S). Once they are all set, the method
//  < ParseArgs > must be called, introducing the same variables as
//  those in the < main > function.
//
// --------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////


#include "ConfigParser.h"
#include "Utils.h"

#include <algorithm>


//_______________________________________________________________________________


// -- CONSTRUCTOR AND DESTRUCTOR

//_______________________________________________________________________________
// Main constructor
General::ConfigParser::ConfigParser() : fParsed( false ) { }

//_______________________________________________________________________________
// Destructor
General::ConfigParser::~ConfigParser() { }

//_______________________________________________________________________________


// -- PUBLIC METHODS

//_______________________________________________________________________________
// Books a new variable, storing also its type
void General::ConfigParser::BookConfigOpt( const std::string &name,
					   const char &type,
					   const std::vector<std::string> &poss ) {
    
  switch ( type ) {
  case 'D':
    break;
  case 'I':
    break;
  case 'S':
    break;
  default:
    std::cerr <<
      "ERROR: Unknown type for variable < " << name << " > (" << type << ')'
					    << std::endl;
    return;
  }

  fArgs[ name ] = std::make_pair( std::string(), poss );
  fVariables.push_back( { name, type } );
}

//_______________________________________________________________________________
// Parses the options given to the main function. Both input variables must be
// those passed to the executable.
void General::ConfigParser::ParseArgs( const int &nargs, const char *argv[] ) {

  // Allows the class to extract values
  fParsed = true;

  // The first argument is the name of the script
  const size_t truenargs = nargs - 1;

  // An error is displayed if the number of parameters in the main function is not
  // equal to that in the class
  const size_t &gsize = fArgs.size();
  if ( gsize != truenargs ) {
    std::cerr <<
      "ERROR: Incorrect number of input parameters (" << truenargs << '/' << gsize << ')'
						      << std::endl;
    return;
  }

  bool status = true;
  for ( auto it = fVariables.begin(); status && it != fVariables.end(); ++it ) {

    const std::string &name = it -> first;
    const char &type = it -> second;
    const size_t pos = it - fVariables.begin() + 1;
    const std::string &arg = argv[ pos ];
    
    switch ( type ) {
    case 'D':
      status *= General::CheckStringType<float>( arg );
      break;
    case 'I':
      status *= General::CheckStringType<int>( arg );
      break;
    case 'S':
      break;
    }
    
    auto def = fArgs[ name ].second;
    if ( def.size() && std::find( def.begin(), def.end(), arg ) == def.end() ) {
      std::cerr << "ERROR: Input for < " << name <<
	" > does not match any of the possibilities: " << General::VectorToString( def ) << std::endl;
      return;
    }
    else
      fArgs[ name ].first = arg;

    // An error is displayed when an argument can not be parsed
    if ( !status )
      std::cerr << "ERROR: Unable to parse argument " << pos << ": < "
		<< name << " > (" << type << ')' << std::endl;
  }
}
