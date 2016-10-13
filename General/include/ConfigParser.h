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


#ifndef CONFIG_PARSER
#define CONFIG_PARSER

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>


//_______________________________________________________________________________ 

namespace General {

  class ConfigParser {

    typedef std::pair< std::string, std::vector<std::string> > ConfigOptPair;
    typedef std::map<std::string, ConfigOptPair>               ConfigMap;
    typedef std::vector< std::pair<std::string, const char> >  TypeVec;

  public:

    // Constructors and destructor
    ConfigParser();
    ~ConfigParser();

    // Methods
    void BookConfigOpt( const std::string &name,
			const char &type,
			const std::vector<std::string> &poss = {} );
    void ParseArgs( const int &nargs, const char *argv[] );

    // Template method
    template <typename type>
    void Extract( const std::string &name, type &value );
    
  protected:
    
    // Attributes
    ConfigMap         fArgs;
    bool              fParsed;
    std::stringstream fParser;
    TypeVec           fVariables;
    
  };

  //________________
  // TEMPLATE METHOD
  
  // Extracts the value for the given variable
  template <typename type>
  void ConfigParser::Extract( const std::string &name, type &value ) {
    if ( !fParsed )
      std::cerr << "ERROR: No arguments have been parsed yet; unable to extract value" << std::endl;
    fParser << fArgs[ name ].first;
    fParser >> value;
    fParser.clear();
  }

}

#endif
