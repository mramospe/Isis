/////////////////////////////////////////////////////////////////////////////
//  Example to check the behaviour of the string calculator and evaluator  //
/////////////////////////////////////////////////////////////////////////////

#include "StringParser.h"

#include <iostream>
#include <vector>
#include <ctime>

int main() {

  clock_t t = clock();
  std::vector<std::string> dvec, bvec, dcvec, bcvec;

  // Checks different double-returning expressions
  std::cout << "\n________________ DOUBLE __________________\n" << std::endl;
  std::cout << "------ Calculations ------" << std::endl;
  dvec.push_back( "1 + 2" );
  dvec.push_back( "2. + 7." );
  dvec.push_back( "sqrt( abs( -(2 * 1 + ( 4 * 3 - 5 )*( ( 1 + 4 ) -3) )/2) + 1.5 )" );
  dvec.push_back( "max( 2, 1 ) + sqrt( abs( -( 2 * 1 + ( 4 * 3 - 5 )*( ( 1 + 4 ) -3) )/2 ) + 1.5 )" );
  dvec.push_back( "min( 1, 2 ) * 4 + ( 9 - 7 )" );
  dvec.push_back( "sqrt( 1.234 ) + 4.32*pow( 2.4, 3.1 ) + 5" );
  dvec.push_back( "1 + pow( 2, 4 )/6.2 + 4" );
  for ( auto it = dvec.begin(); it != dvec.end(); it++ )
    std::cout << *it << " => " << General::StringParser::BareCalculate( *it ) << std::endl;

  std::cout << "------ Checks ------" << std::endl;
  dcvec.push_back( "1 + 2" );
  dcvec.push_back( "4 + 8 - 9)" );
  dcvec.push_back( "( 4 * 5 )+ 1 / ( -4 +2 )" );
  dcvec.push_back( "( 4 * 5 )+ 1 / ( -4 +/ 2 )" );
  dcvec.push_back( "3/2 + 4*6 + /( 3 - 1 )" );
  dcvec.push_back( "pow( , 2 ) + 2 * ( 4 - 5 )" );
  dcvec.push_back( "*1 + 2 * ( 4 - 5 )" );
  for ( auto it = dcvec.begin(); it != dcvec.end(); it++ )
    if ( General::StringParser::CheckCalcExpression( *it ) )
      std::cout << "FINE => " << *it << std::endl;

  // Checks different bool-returning expressions
  std::cout << "\n________________ BOOLEAN __________________\n" << std::endl;
  std::cout << "------ Calculations ------" << std::endl;
  bvec.push_back( "2. < 7" );
  bvec.push_back( "2 > 7." );
  bvec.push_back( "2 == 7." );
  bvec.push_back( "2 == 2." );
  bvec.push_back( "2 == ( 2. + 1 )" );
  bvec.push_back( "1 || 0" );
  bvec.push_back( "1 && (1 + 2) > 5" );
  bvec.push_back( "0 || ( 1 && 0 )" );
  bvec.push_back( "(1 || 0 || 0) && (0)" );
  for ( auto it = bvec.begin(); it != bvec.end(); it++ )
    std::cout << *it << " => " << General::StringParser::BareEvaluate( *it ) << std::endl;

  std::cout << "------ Checks ------" << std::endl;
  bcvec.push_back( "1 + 2 > 3" );
  bcvec.push_back( "4 + 8 - 9) < 4" );
  bcvec.push_back( "( 4 * 5 )+ 1 >= 4 & 1" );
  bcvec.push_back( "( 4 * 5 )+ 1 - ( 1 && 0 )|| 4 > 3" );
  bcvec.push_back( "3 |= 1 + 2" );
  bcvec.push_back( "3 > 4*3 && 5 == min( 5, 1 )" );
  bcvec.push_back( "4 > pow( 2, 1.5 )*9.5 && ( 1 || 3 > 2 ) || 1" );
  for ( auto it = bcvec.begin(); it != bcvec.end(); it++ )
    if ( General::StringParser::CheckEvalExpression( *it ) )
      std::cout << "FINE => " << *it << std::endl;

  // Displays the elapsed time
  std::cout << "\nTime elapsed: " << ( clock() - t )*1./CLOCKS_PER_SEC << " s\n" << std::endl;
  
  return 0;
}
