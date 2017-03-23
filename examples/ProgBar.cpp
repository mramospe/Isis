////////////////////////////
//  Progress bar example  //
////////////////////////////

#include "ProgressBar.h"

#include <iostream>

int main() {

  // Initializes the progress bar with all options
  Isis::ProgressBar prbar("Bar=50:Percentage=1:Time:Date=Start-End");
  
  // Sets the color of the different options
  prbar.setColor( "Bar=32;41;1:Percentage=33;45;1:Time=33;3:StartDate=35;42;2:EndDate=31;45;2" );

  std::cout << " Starting..." << std::endl;

  // Number of iterations
  size_t n = 100000000;

  // Initializes the progress bar
  prbar.start( n );
  for ( size_t i = 0; i < n; i++ )
    prbar.display( i );

  // Prints the bar for the last time ( reaching automatically the 100% )
  prbar.end();

  std::cout << " Ending..." << std::endl;

  return 0;
}
