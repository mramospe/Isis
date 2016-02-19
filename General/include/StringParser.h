///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  General package                                                              //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  AUTHOR: Miguel Ramos Pernas                                                  //
//  e-mail: miguel.ramos.pernas@cern.ch                                          //
//                                                                               //
//  Last update: 19/02/2016                                                      //
//                                                                               //
// ----------------------------------------------------------------------------- //
//                                                                               //
//  Description:                                                                 //
//                                                                               //
//  Implements the functions to convert expressions given as strings to bool     //
//  and double values. If one wants to evaluate or calculate expressions many    //
//  times, since the < Calculate > and < Evaluate > check the input string       //
//  every time they are called, it is much faster to check it once via           //
//  < CheckCalcExpression > and < CheckEvalExpression >, where a keyword or a    //
//  set of them can be provided. In this case the function will replace that     //
//  word(s) by < 1 >, so the expression can be checked.                          //
//                                                                               //
// ----------------------------------------------------------------------------- //
///////////////////////////////////////////////////////////////////////////////////


#ifndef STRING_PARSER
#define STRING_PARSER

#include <string>
#include <vector>


//_______________________________________________________________________________

namespace General {

  namespace StringParser {

    // ----------------------------------------------------
    // Functions concerning the string-to-double calculator

    // Main functions
    double                          BareCalculate( std::string expr );
    double                          Calculate( std::string expr );
    
    // Auxiliar functions
    inline double                   Decimal( std::string::iterator &it );
    inline double                   Expression( std::string::iterator &it );
    inline double                   Factor( std::string::iterator &it );
    inline std::string::iterator    FindClosePar( std::string::iterator it );
    inline std::string::iterator    FindComma( std::string::iterator it,
					       std::string::iterator &endfunc );
    inline double                   Function( std::string::iterator &it );
    double                       ( *GetOneDoubleFunction( std::string func ) )( double );
    double                       ( *GetTwoDoubleFunction( std::string func ) )( double, double );
    inline double                   Number( std::string::iterator &it );
    inline double                   Term( std::string::iterator &it );

    // -------------------------------------------------
    // Functions concerning the string-to-bool evaluator

    // Main functions
    bool                         BareEvaluate( std::string expr );
    bool                         Evaluate( std::string expr );
    
    // Auxiliar functions
    inline bool                  BoolExpression( std::string::iterator &it, const bool &close = false );
    inline bool                  BoolTerm( std::string::iterator &it );
    inline bool                  Inequation( std::string::iterator &it );
    inline std::string::iterator NextBoolOperator( std::string::iterator it );

    // -------------------------------------------------------
    // Functions to check that the given expression is correct

    // Main functions
    bool CheckCalcExpression( std::string expr );
    bool CheckCalcExpression( std::string expr, std::string key );
    bool CheckCalcExpression( std::string expr, std::vector<std::string> &keys );
    bool CheckEvalExpression( std::string expr );
    bool CheckEvalExpression( std::string expr, std::string key );
    bool CheckEvalExpression( std::string expr, std::vector<std::string> &keys );

    // Auxiliar functions
    inline void CheckCalcChar( std::string::iterator &it );
    inline bool CheckBoolSymbol( std::string::iterator &it );
    inline void CheckEvalChar( std::string::iterator &it );
    inline bool CheckFunction( std::string::iterator &it );
    inline bool CheckLeft( std::string::iterator it );
    inline bool CheckParentheses( std::string &expr );
    inline bool CheckRight( std::string::iterator it );

  }
}

#endif
