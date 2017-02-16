///////////////////////////////////////////////////////////////////////
//
//  General package
//
// --------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 16/02/2016
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


#ifndef CONFIG_PARSER
#define CONFIG_PARSER

#include "Messenger.h"

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>


//_______________________________________________________________________________ 

namespace General {

  class ConfigParser {

    // Definition of types used by this class
    typedef std::pair< std::string, std::vector<std::string> > ConfigOptPair;
    typedef std::map<std::string, ConfigOptPair>               ConfigMap;
    typedef std::vector< std::pair<std::string, const char> >  TypeVec;

  public:

    // Main constructor
    ConfigParser();

    // Destructor
    ~ConfigParser();

    // Books a new variable, storing also its type
    void BookConfigOpt( const std::string &name,
			const char &type,
			const std::vector<std::string> &poss = {} );

    // Parses the options given to the main function. Both input variables must be
    // those passed to the executable.
    void ParseArgs( const int &nargs, const char *argv[] );

    // Extracts the value for the given variable
    template <typename type>
    void Extract( const std::string &name, type &value );
    
  protected:
    
    // Map containing the configuration options
    ConfigMap fArgs;

    // Boolean to store whether the configuration has been parsed or not
    bool fParsed;

    // Object to parse the configuration
    std::stringstream fParser;

    // Vector to store the variables containing the information about the configuration
    TypeVec fVariables;
    
  };

  //_______________________________________________________________________________
  //
  template <typename type>
  void ConfigParser::Extract( const std::string &name, type &value ) {
    if ( !fParsed )
      Error << "No arguments have been parsed yet; unable to extract value" << EndMsg;
    fParser << fArgs[ name ].first;
    fParser >> value;
    fParser.clear();
  }

}

#endif
