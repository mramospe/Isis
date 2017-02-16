///////////////////////////////////////////////////////////////////////
//
//  General package
//
// --------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 16/02/2017
//
// --------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////


#include "ConfigParser.h"
#include "Messenger.h"
#include "Utils.h"

#include <algorithm>


//_______________________________________________________________________________
//
General::ConfigParser::ConfigParser() : fParsed( false ) { }

//_______________________________________________________________________________
//
General::ConfigParser::~ConfigParser() { }

//_______________________________________________________________________________
//
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
    Error <<
      "Unknown type for variable < " << name << " > (" << type << ')'
					    << EndMsg;
    return;
  }

  fArgs[ name ] = std::make_pair( std::string(), poss );
  fVariables.push_back( { name, type } );
}

//_______________________________________________________________________________
//
void General::ConfigParser::ParseArgs( const int &nargs, const char *argv[] ) {

  // Allows the class to extract values
  fParsed = true;

  // The first argument is the name of the script
  const size_t truenargs = nargs - 1;

  // An error is displayed if the number of parameters in the main function is not
  // equal to that in the class
  const size_t &gsize = fArgs.size();
  if ( gsize != truenargs ) {
    Error <<
      "Incorrect number of input parameters (" << truenargs << '/' << gsize << ')'
						      << EndMsg;
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
      Error << "Input for < " << name <<
	" > does not match any of the possibilities: " << General::VectorToString( def ) << EndMsg;
      return;
    }
    else
      fArgs[ name ].first = arg;

    // An error is displayed when an argument can not be parsed
    if ( !status )
      Error << "Unable to parse argument " << pos << ": < "
		<< name << " > (" << type << ')' << EndMsg;
  }
}
