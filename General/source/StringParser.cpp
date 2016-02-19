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


#include <iostream>
#include <cmath>
#include <stdexcept>

#include "StringParser.h"


// ----------------------------------------------------
// Functions concerning the string-to-double calculator
// ----------------------------------------------------

//_______________________________________________________________________________
// Calculates the given expression, returning the associated double value. Be
// careful with the input string since no check will be made to determine if it
// has been writen correctly. In order to check it first use the < Calculate >
// function instead.
double General::StringParser::BareCalculate( std::string expr ) {
  for ( auto it = expr.begin(); it != expr.end(); it++ )
    if ( *it == ' ' )
      expr.erase( it-- );
  auto it = expr.begin();
  return Expression( it );
}

//_______________________________________________________________________________
// Calculates the given expression, returning the associated boolean value. A
// check will be made first in order to see if the input expression can be
// computed.
double General::StringParser::Calculate( std::string expr ) {
  if ( CheckCalcExpression( expr ) )
    return BareCalculate( expr );
  else
    throw std::invalid_argument( "ERROR: Input expression no valid" );
}

//_______________________________________________________________________________
// Gets a Decimal expression
inline double General::StringParser::Decimal( std::string::iterator &it ) {
  if ( *it >= '0' && *it <= '9' ) {
    double result = ( *it++ - '0' )/10.;
    unsigned int pos = 2;
    while ( *it >= '0' && *it <= '9' )
      result = result + ( *it++ - '0' )/std::pow( 10, pos++ );
    return result;
  }
  else
    return 0;
}

//_______________________________________________________________________________
// Analyses the given expression, wich is the main object in the evaluating
// process. An error is sent if the parenthesis does not match, also with a comma
// ( because of two-double functions )
inline double General::StringParser::Expression( std::string::iterator &it ) {
  double result = Term( it );
  while ( *it == '+' || *it == '-' )
    *it == '+' ? result += Term( ++it ) : result -= Term( ++it );
  if ( *it == ')' )
    it++;
  return result;
}

//_______________________________________________________________________________
// Returns the value conrresponding to a factor. This is the minimal object in
// the evaluating process, where different conditions lead to the different
// possibilities of calculation.
inline double General::StringParser::Factor( std::string::iterator &it ) {
  if ( ( *it >= '0' && *it <= '9' ) || *it == '.' )
    return Number( it );
  else if ( *it == '(' )
    return Expression( ++it );
  else if ( std::isalpha( *it ) )
    return Function( it );
  else if ( *it == '-' )
    return -Factor( ++it );
  else if ( *it == '+' )
    return +Factor( ++it );
  return 0;
}

//_______________________________________________________________________________
// Finds the position of the parenthesis that closes the given one
inline std::string::iterator General::StringParser::FindClosePar( std::string::iterator it ) {
  unsigned int npar = 1;
  do {
    if ( *it++ == '(' )
      npar++;
    else if ( *it == ')')
      npar--;
  } while( npar && *it != '\0' );
  return it++;
}

//_______________________________________________________________________________
// Checks if the string interval defined by the two given iterators corresponds
// to a function that takes two double values ( separated with a comma ).
inline std::string::iterator General::StringParser::FindComma( std::string::iterator it,
							       std::string::iterator &endfunc ) {
  unsigned int npar = 0;
  while ( ++it != endfunc ) {
    if ( *it == '(' )
      npar++;
    else if ( *it == ')' )
      npar--;
    else if ( *it == ',' && !npar )
      return it;
  }
  return it;
}

//_______________________________________________________________________________
// Returns the evaluation of the function present in the current string. Depending
// on the class of this object, it will call the one-double or two-double functions.
inline double General::StringParser::Function( std::string::iterator &it ) {
  std::string::iterator fbegin = it++;
  while ( *it++ != '(' );
  std::string::iterator
    fend = FindClosePar( it ),
    fcom = FindComma( it, fend );
  if ( fcom++ == fend )
    return GetOneDoubleFunction( std::string( fbegin, it - 1 ) )( Expression( it ) );
  else {
    std::swap( it, fcom );
    return GetTwoDoubleFunction( std::string( fbegin, fcom - 1 ) )( Expression( fcom ),
								    Expression( it ) );
  }
}

//_______________________________________________________________________________
// Given the name, returns a pointer to a function that takes and returns a double
// value. If no function is found a NULL pointer is returned.
double ( *General::StringParser::GetOneDoubleFunction( std::string func ) )( double ) {

  // Trigonometric
  if      ( func == "cos"  ) return std::cos;
  else if ( func == "sin"  ) return std::sin;
  else if ( func == "tan"  ) return std::tan;
  else if ( func == "acos" ) return std::acos;
  else if ( func == "asin" ) return std::asin;
  else if ( func == "atan" ) return std::atan;

  // Hyperbolic functions
  else if ( func == "cosh"  ) return std::cosh;
  else if ( func == "sinh"  ) return std::sinh;
  else if ( func == "tanh"  ) return std::tanh;
  else if ( func == "acosh" ) return std::acosh;
  else if ( func == "asinh" ) return std::asinh;
  else if ( func == "atanh" ) return std::atanh;

  // Exponential and logarithmic functions
  else if ( func == "exp"   ) return std::exp;
  else if ( func == "log"   ) return std::log;
  else if ( func == "log10" ) return std::log10;

  // Power functions
  //  else if ( func == "pow"  ) return std::pow;
  else if ( func == "sqrt" ) return std::sqrt;
  else if ( func == "cbrt" ) return std::cbrt;

  // Error function
  else if ( func == "erf"    ) return std::erf;
  else if ( func == "erfc"   ) return std::erfc;
  else if ( func == "tgamma" ) return std::tgamma;
  else if ( func == "lgamma" ) return std::lgamma;

  // Rounding and remainder functions
  else if ( func == "ceil"  ) return std::ceil;
  else if ( func == "floor" ) return std::floor;
  else if ( func == "round" ) return std::round;

  // Others
  else if ( func == "abs" ) return std::abs;

  // Error exception
  else return 0;
}

//_______________________________________________________________________________
// Given the name, returns a function that takes two double values and returns one.
// If no function is found a NULL pointer is returned.
double ( *General::StringParser::GetTwoDoubleFunction( std::string func ) )( double, double ) {

  if ( func == "atan2" ) return std::atan2;
  else if ( func == "min" || func == "fmin" ) return std::fmin;
  else if ( func == "max" || func == "fmax" ) return std::fmax;
  else if ( func == "pow" ) return std::pow;
  else return 0;
}

//_______________________________________________________________________________
// This function allows to construct a number from the string
inline double General::StringParser::Number( std::string::iterator &it ) {
  double integer = 0;
  while( *it >= '0' && *it <= '9' )
    integer = 10*integer + *it++ - '0';
  return *it == '.' ? integer + Decimal( ++it ) : integer;
}

//_______________________________________________________________________________
// Function that takes into account the multiplications and divisions present in
// the string
inline double General::StringParser::Term( std::string::iterator &it ) {
  double result = Factor( it );
  while ( *it == '*' || *it == '/' )
    *it == '*' ? result *= Factor( ++it ) : result /= Factor( ++it );
  return result;
}


// -------------------------------------------------
// Functions concerning the string-to-bool evaluator
// -------------------------------------------------

//_______________________________________________________________________________
// Evaluates the given expression, returning the associated boolean value. Be
// careful with the input string since no check will be made to determine if it
// has been writen correctly. In order to check it first use the < Evaluate >
// function instead.
bool General::StringParser::BareEvaluate( std::string expr ) {
  for ( auto it = expr.begin(); it != expr.end(); it++ )
    if ( *it == ' ' )
      expr.erase( it-- );
  auto it = expr.begin();
  return BoolExpression( it );
}

//_______________________________________________________________________________
// Main function to solve a boolean expression in a string. The boolean input
// is necessary to properly take into account the nesting level.
inline bool General::StringParser::BoolExpression( std::string::iterator &it, const bool &close ) {
  bool result = BoolTerm( it );
  while ( *it == '&' || *it == '|' )
    if ( *it == '&' )
      result &= BoolExpression( it += 2 );
    else if ( *it == '|' )
      result |= BoolExpression( it += 2 );
  if ( close && *it == ')' )
    it++;
  return result;
}

//_______________________________________________________________________________
// This function manages the parts of a boolean expression separated by < && >
// and < || > operators
inline bool General::StringParser::BoolTerm( std::string::iterator &it ) {
  if ( *it == '(' )
    return BoolExpression( ++it, true );
  else if ( *it == '!' )
    return !BoolTerm( ++it );
  else
    if ( *( it + 1 ) == '&' || *( it + 1 ) == '|' || *( it + 1 ) == ')' )
      if ( *it++ == '0' )
	return false;
      else
	return true;
    else
      return Inequation( it );
}

//_______________________________________________________________________________
// Evaluates the given expression, returning the associated boolean value. A
// check will be made first in order to see if the input expression can be
// computed.
bool General::StringParser::Evaluate( std::string expr ) {
  if ( CheckEvalExpression( expr ) )
    return BareEvaluate( expr );
  else
    throw std::invalid_argument( "ERROR: Input expression no valid" );
}

//_______________________________________________________________________________
// Solves the given inequation. If many inequations are concatenated, it will
// perform the calculation as if they were followed by < && > operators, i.e:
// "2 > 3 > 1 => 2 > 3 && 3 > 1".
inline bool General::StringParser::Inequation( std::string::iterator &it ) {
  auto backit = it;
  it = NextBoolOperator( it );
  if ( *it == '>' ) {
    if ( *( ++it ) != '=' )
      return BareCalculate( std::string( backit, it - 1 ) ) >
	BareCalculate( std::string( it, NextBoolOperator( it ) ) ) && Inequation( it );
    else
      return BareCalculate( std::string( backit, it - 1 ) ) >=
	BareCalculate( std::string( ++it, NextBoolOperator( it + 1 ) ) ) && Inequation( it );
  }
  else if ( *it == '<' ) {
    if ( *( ++it ) != '=' )
      return BareCalculate( std::string( backit, it - 1 ) ) <
	BareCalculate( std::string( it, NextBoolOperator( it ) ) ) && Inequation( it );
    else
      return BareCalculate( std::string( backit, it - 1 ) ) <=
	BareCalculate( std::string( ++it, NextBoolOperator( it + 1 ) ) ) && Inequation( it );
  }
  else if ( *it == '=' ) {
    if ( *( ++it ) == '=' )
      return BareCalculate( std::string( backit, it - 1 ) ) ==
	BareCalculate( std::string( ++it, NextBoolOperator( it + 1 ) ) ) && Inequation( it );
    else
      throw std::invalid_argument( "ERROR: Incomplete boolean expression" );
  }
  else if ( *it == '!' ) {
    if ( *( ++it ) == '=' )
      return BareCalculate( std::string( backit, it - 1 ) ) !=
	BareCalculate( std::string( ++it, NextBoolOperator( it + 1 ) ) ) && Inequation( it );
    else
      throw std::invalid_argument( "ERROR: Incomplete boolean expression" );
  }
  else
    if ( *backit == '0' ) {
      do { backit++; } while ( *backit == '0' );
      if ( backit == it )
	return false;
      else
	return true;
    }
    else
      return true;
}

//_______________________________________________________________________________
// Returns the position of the next boolean operator in the given string
inline std::string::iterator General::StringParser::NextBoolOperator( std::string::iterator it ) {
  while ( *it != '>' &&
	  *it != '<' &&
	  *it != '=' &&
	  *it != '!' &&
	  *it != '&' &&
	  *it != '|' &&
	  *it != '\0' ) it++;
  return it;
}


// --------------------------------------------------------------
// Functions to check that a given expression is correctly writen
// --------------------------------------------------------------

//_______________________________________________________________________________
// Checks if a given character concerning "calculate" expressions is well writen
inline void General::StringParser::CheckCalcChar( std::string::iterator &it ) {
  if ( std::isalpha( *it ) ) {
    if ( !CheckFunction( it ) )
      throw std::invalid_argument( "ERROR: Unknown character or incomplete function at position: " );
  }
  else if ( *it == '+' || *it == '-' ) {
    if ( !CheckRight( it ) )
      throw std::invalid_argument( "ERROR: Unknown expression to the right of position: " );
  }
  else if ( *it == '*' || *it == '/' ) {
    if ( !CheckLeft( it ) )
      throw std::invalid_argument( "ERROR: Unknown expression to the left of position: " );
    else if ( !CheckRight( it ) )
      throw std::invalid_argument( "ERROR: Unknown expression to the right of position: " );
  }
  else if ( ( *it < '0' || *it > '9' ) && *it != '.' && *it != '(' && *it != ')' ) {
    throw std::invalid_argument( "ERROR: Unknown symbol at position: " );
  }
}

//_______________________________________________________________________________
// Checks if the given expression is correctly writen to be calculated
bool General::StringParser::CheckCalcExpression( std::string expr ) {
  for ( auto it = expr.begin(); it != expr.end(); it++ )
    if ( *it == ' ' )
      expr.erase( it-- );
  if ( !CheckParentheses( expr ) ) {
    std::cerr << "ERROR: Mismatched parentheses => Input: " << expr << std::endl;
    return false;
  }
  for ( auto it = expr.begin(); it != expr.end(); it++ )
    try { CheckCalcChar( it ); }
    catch ( std::invalid_argument &error ) {
      std::cerr << error.what() << it - expr.begin() + 1 << " => Input: " << expr << std::endl;
      return false;
    }
  return true;
}

//_______________________________________________________________________________
// Checks if the given expression is correctly writen to be calculated. The
// < key > argument has to contain the keyword to be replaced in < expr > to
// make the check.
bool General::StringParser::CheckCalcExpression( std::string expr, std::string key ) {
  size_t pos = 0;
  while ( ( pos = expr.find( key ) ) != std::string::npos )
    expr.replace( pos, key.size(), "1" );
  return CheckCalcExpression( expr );
}

//_______________________________________________________________________________
// Checks if the given expression is correctly writen to be calculated. The
// < keys > argument has to contain the keywords to be replaced in < expr > to
// make the check.
bool General::StringParser::CheckCalcExpression( std::string expr, std::vector<std::string> &keys ) {
  size_t pos = 0;
  for ( auto it = keys.begin(); it != keys.end(); it++ )
    while ( ( pos = expr.find( *it ) ) != std::string::npos )
      expr.replace( pos, it -> size(), "1" );
  return CheckCalcExpression( expr );
}

//_______________________________________________________________________________
// Checks that all the < && >, < || >, < == > and < != > are correctly writen
inline bool General::StringParser::CheckBoolSymbol( std::string::iterator &it ) {
  auto frontit = it + 1;
  if ( *it == '&' && *frontit != '&' )
    return false;
  if ( *it == '|' && *frontit != '|' )
    return false;
  if ( *it == '=' && *frontit != '=' )
    return false;
  if ( *it == '!' && *frontit != '=' )
    return false;
  return true;
}

//_______________________________________________________________________________
// Checks if a given character concerning "evaluate" expressions is well writen
inline void General::StringParser::CheckEvalChar( std::string::iterator &it ) {
  if ( *it == '&' || *it == '|' || *it == '=' || *it == '!' ) {
    if ( !CheckBoolSymbol( it ) )
      throw std::invalid_argument( "ERROR: Wrong boolean operator at position: " );
    else if ( !CheckLeft( it ) )
      throw std::invalid_argument( "ERROR: Wrong expression to the left of position " );
    else if ( !CheckRight( it + 1 ) )
      throw std::invalid_argument( "ERROR: Wrong expression to the right of position " );
    else
      it += 2;
  }
  else if ( *it == '>' || *it == '<' ) {
    std::string::iterator auxit;
    *( it + 1 ) != '=' ? auxit = it : auxit = it + 1;
    if ( !CheckLeft( it ) )
      throw std::invalid_argument( "ERROR: Wrong expression to the left of position " );
    else if ( !CheckRight( auxit ) )
      throw std::invalid_argument( "ERROR: Wrong expression to the right of position " );
    else
      it = auxit + 1;
  }
}

//_______________________________________________________________________________
// Checks if the given expression is correctly writen to be evaluated
bool General::StringParser::CheckEvalExpression( std::string expr ) {
  for ( auto it = expr.begin(); it != expr.end(); it++ )
    if ( *it == ' ' )
      expr.erase( it-- );
  if ( !CheckParentheses( expr ) ) {
    std::cerr << "ERROR: Mismatched parentheses => Input: " << expr << std::endl;
    return false;
  }
  for ( auto it = expr.begin(); it != expr.end(); it++ ) {
    try { CheckEvalChar( it ); }
    catch ( std::invalid_argument &eval_error ) {
      std::cerr << eval_error.what() << it - expr.begin() + 1 << " => Input: " << expr << std::endl;
      return false;
    }
    try { CheckCalcChar( it ); }
    catch ( std::invalid_argument &calc_error ) {
      std::cerr << calc_error.what() << it - expr.begin() + 1 << " => Input: " << expr << std::endl;
      return false;
    }
  }
  return true;
}

//_______________________________________________________________________________
// Checks if the given expression is correctly writen to be evaluated. The
// < key > argument has to contain the keyword to be replaced in < expr > to
// make the check.
bool General::StringParser::CheckEvalExpression( std::string expr, std::string key ) {
  size_t pos = 0;
  while ( ( pos = expr.find( key ) ) != std::string::npos )
    expr.replace( pos, key.size(), "1" );
  return CheckEvalExpression( expr );
}

//_______________________________________________________________________________
// Checks if the given expression is correctly writen to be evaluated. The
// < keys > argument has to contain the keywords to be replaced in < expr > to
// make the check.
bool General::StringParser::CheckEvalExpression( std::string expr, std::vector<std::string> &keys ) {
  size_t pos = 0;
  for ( auto it = keys.begin(); it != keys.end(); it++ )
    while ( ( pos = expr.find( *it ) ) != std::string::npos )
      expr.replace( pos, it -> size(), "1" );
  return CheckEvalExpression( expr );
}

//_______________________________________________________________________________
// Checks if a given string corresponds to a function. It is considered that
// the arguments of the function are not boolean expressions.
inline bool General::StringParser::CheckFunction( std::string::iterator &it ) {
  std::string::iterator fbegin = it++;
  while ( *it++ != '(' && *it != '\0' );
  if ( *it == '\0' )
    return false;
  std::string::iterator
    fend = FindClosePar( it ),
    fcom = FindComma( it, fend );
  if ( fcom++ == fend ) {
    if ( !GetOneDoubleFunction( std::string( fbegin, it - 1 ) ) )
      return false;
    else
      return true;
  }
  else
    if ( !GetTwoDoubleFunction( std::string( fbegin, it - 1 ) ) ) 
      return false;
    else {
      std::swap( fcom, it );
      return CheckCalcExpression( std::string( fcom + 1, it++ - 1 ) );
    }
}

//_______________________________________________________________________________
// Checks if the expression to the left of the iterator is a number or term
inline bool General::StringParser::CheckLeft( std::string::iterator it ) {
  if ( !( *--it == ')' || ( *it >= '0' && *it <= '9' ) || *it == '.' ) )
    return false;
  return true;
}

//_______________________________________________________________________________
// Checks if the opener parenthesis has an attached closer one
inline bool General::StringParser::CheckParentheses( std::string &expr ) {
  int npar = 0;
  for ( auto it = expr.begin(); it != expr.end(); it++ )
    if ( *it == '(' )
      npar++;
    else if ( *it == ')' )
      npar--;
  if ( npar )
    return false;
  else
    return true;
}

//_______________________________________________________________________________
// Checks if the expression to the right of the iterator is a number, term or
// function
inline bool General::StringParser::CheckRight( std::string::iterator it ) {
  if ( ( *++it == '(' || ( *it >= '0' && *it <= '9' ) || *it == '+' || *it == '-' || *it == '.' || std::isalpha( *it ) ) )
    return true;
  return false;
}
