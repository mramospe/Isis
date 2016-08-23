///////////////////////////////////////////////////////////////////////
//                                                                   //
//  General package                                                  //
//                                                                   //
// ----------------------------------------------------------------- //
//                                                                   //
//  AUTHOR: Miguel Ramos Pernas                                      //
//  e-mail: miguel.ramos.pernas@cern.ch                              //
//                                                                   //
//  Last update: 23/08/2016                                          //
//                                                                   //
// ----------------------------------------------------------------- //
//                                                                   //
//  Description:                                                     //
//                                                                   //
//  This class facilitates the specification of the input variables  //
//  for a c++ executable. The types of the variables are specified   //
//  via the < BookVariable > method. Once they are all set, the      //
//  method < ParseArgs > must be called, introducing the same        //
//  variables as those in the < main > function.                     //
//                                                                   //
// ----------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////


#ifndef ARG_PARSER
#define ARG_PARSER

#include <map>
#include <sstream>
#include <string>
#include <vector>


//_______________________________________________________________________________ 

namespace General {

  class ConfigParser {

    typedef std::map<std::string, std::string>                StrMap;
    typedef std::vector< std::pair<std::string, const char> > TypeVec;

  public:

    // Constructors and destructor
    ConfigParser();
    ~ConfigParser();

    // Methods
    void BookVariable( const std::string &name, const char &type );
    void ParseArgs( const int &nargs, const char *argv[] );

    // Template method
    template <typename type>
    void Extract( const std::string &name, type &value );
    
  protected:
    
    // Attributes
    StrMap            fArgs;
    std::stringstream fParser;
    TypeVec           fVariables;
    
  };

  //________________
  // TEMPLATE METHOD
  
  // Extracts the value for the given variable
  template <typename type>
  void ConfigParser::Extract( const std::string &name, type &value ) {
    fParser << fArgs[ name ];
    fParser >> value;
    fParser.clear();
  }

}

#endif
