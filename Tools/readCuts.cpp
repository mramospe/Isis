/////////////////////////////////////////////////////////////
// ------------------------------------------------------- //
//                                                         //
//  Tool that displays the specified cuts in the console.  //
//                                                         //
// ------------------------------------------------------- //
//                                                         //
//  AUTHOR : Miguel Ramos Pernas                           //
//  e-mail: miguel.ramos.pernas@cern.ch                    //
//                                                         //
//  Last Update: 06/04/2016                                //
//                                                         //
// ------------------------------------------------------- //
//                                                         //
// Usage:                                                  //
//                                                         //
// >> ./readCuts FileName cut1 cut2 ...                    //
//                                                         //
// ------------------------------------------------------- //
/////////////////////////////////////////////////////////////


#include "CutManager.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <vector>

int main( int argc, const char *argv[] ) {

  if ( argc < 2 )
    std::cerr << "The input parameters are: < file name > < cut1 > < cut2 >..." << std::endl;
  else if ( argc == 2 )
    std::cerr << "At least one cut name must be provided" << std::endl;
  else {
    std::vector<std::string> vec( argc - 2 );
    for ( size_t i = 0; i < vec.size(); i++ )
      vec[ i ] = argv[ i + 2 ];
    size_t maxsize = std::max_element( vec.begin(), vec.end(),
				       [] (std::string n1, std::string n2) {
					 return n1.size() > n2.size(); } ) -> size();

    std::cout << "Reading cuts from: " << argv[ 1 ] << std::endl;

    General::CutManager mgr( argv[ 1 ] );
    for ( auto it = vec.begin(); it != vec.end(); it++ )
      std::cout << " - " << std::setw( maxsize ) << *it << " => " << mgr.GetCut( *it ) << std::endl;
  }

  return 0;
}
