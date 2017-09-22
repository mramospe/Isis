///////////////////////////////////////////////////////////////////////
//
//  General package
//
// --------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 10/04/2017
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

#include "Definitions.hpp"
#include "Messenger.hpp"

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>


//_______________________________________________________________________________ 

namespace isis {

  class ConfigParser {

    // Definition of types used by this class
    typedef std::pair<std::string, Strings> ConfigOptPair;
    typedef std::map<std::string, ConfigOptPair> ConfigMap;
    typedef std::vector< std::pair<std::string, const char> >  TypeVec;

  public:

    // Main constructor
    ConfigParser();

    // Destructor
    ~ConfigParser();

    // Books a new variable, storing also its type
    void bookConfigOpt( const std::string &name,
			const char &type,
			const Strings &poss = {} );

    // Parses the options given to the main function. Both input variables must be
    // those passed to the executable.
    void parseArgs( const int &nargs, const char *argv[] );

    // Extracts the value for the given variable
    template <typename type>
    void extract( const std::string &name, type &value );
    
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
  void ConfigParser::extract( const std::string &name, type &value ) {
    if ( !fParsed )
      IError << "No arguments have been parsed yet; unable to extract value" << IEndMsg;
    fParser << fArgs[ name ].first;
    fParser >> value;
    fParser.clear();
  }

}

#endif
