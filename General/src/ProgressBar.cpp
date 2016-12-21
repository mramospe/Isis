///////////////////////////////////////////////////////////////////
//
//  General package
//
// ----------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 08/07/2015
//
// ----------------------------------------------------------------
//
//  Description:
//
//  Includes the ProgressBar class to display the
//  progress of a certain loop. Configuration options
//  are:
//
//  Bar        => Generates a progress bar with size 50
//  Percentage => Shows the percentage of the loop each time it
//                completes the specified percentage
//  Time       => Displays the time elapsed in the loop
//  Date       => Allows the display of the date. There are two
//                options:
//                 - Start: shows the starting date
//                 - End:   shows the ending date
//                By default local time is displayed. If option
//                UTC is specified the dates will correspond to
//                the UTC date and time.
//
//  Options have to be separated by the < : > character. All
//  modifications have to be done through the < = > operator in
//  the way < Bar=50 >. This sets the ProgressBar class to
//  generate a bar of size 50 points. For Date the options can
//  be separated by any symbol by < = > or < : >.
//
//  For the colors, they have to be expresed as ANSI color
//  codes. With the same options as above but substituing
//  < Date > by < StartDate > and < EndDate >. An example could
//  be < Bar=32;41;1 >, meaning the display of the bar with
//  green bold lines, and with a red background.
//
// ----------------------------------------------------------------
///////////////////////////////////////////////////////////////////


#include "ProgressBar.h"

#include <iostream>
#include <sstream>
#include <iomanip>

//______________________________________________________________________________


// -- CONSTRUCTORS AND DESTRUCTOR

//______________________________________________________________________________
// Constructor
General::ProgressBar::ProgressBar( const char *opts ) :
  fBarWidth( 50 ), fPercentage( 1 ) {

  fUse[ "Bar"  ]      = true;
  fUse[ "Time" ]      = false;
  fUse[ "StartDate" ] = false;
  fUse[ "EndDate"   ] = false;

  if ( opts ) {

    std::string str( opts );

    if ( str.find( "Bar" ) != std::string::npos )
      fBarWidth = std::atoi( this -> GetOption( str, "Bar" ).c_str() );

    if ( str.find( "Percentage" ) != std::string::npos )
      fPercentage = std::atoi( this -> GetOption( str, "Percentage" ).c_str() );

    if ( str.find( "Time" ) != std::string::npos )
      fUse[ "Time" ] = true;
    
    if ( str.find( "Date" ) != std::string::npos ) {

      std::string opt( this -> GetOption( str, "Date" ) );

      if ( opt.find( "Start" ) != std::string::npos )
	fUse[ "StartDate" ] = true;

      if ( opt.find( "End" ) != std::string::npos )
	fUse[ "EndDate" ] = true;

      if ( opt.find( "UTC" ) != std::string::npos )
	fUse[ "UTC" ] = true;
    }
  }
}

//______________________________________________________________________________
// Destructor
General::ProgressBar::~ProgressBar() { }

//______________________________________________________________________________


// --- PUBLIC METHODS

//______________________________________________________________________________
// Ends the display of the progress
void General::ProgressBar::End() {

  if ( fBarWidth ) {

    std::cout << "\033[" << fColors[ "Bar" ] << "m[";
  
    for ( size_t i = 0; i < fBarWidth; i++ )
	std::cout << "|";

    std::cout << "]\033[m";
  }

  if ( fPercentage )
    std::cout << " \033[" << fColors[ "Percentage" ] << "m100\%\033[m";

  if ( fUse[ "Time" ] )
      std::cout << " \033[" << fColors[ "Time" ] <<
	"m( Time elapsed: " << this -> GetTime() << " )\033[m";

  if ( fUse[ "StartDate" ] )
      std::cout << " \033[" << fColors[ "StartDate" ] <<
	"m( Started: " << fStartDate << " )\033[m";

  if ( fUse[ "EndDate" ] )
    std::cout << " \033[" << fColors[ "EndDate" ] <<
      "m( Finished: " << this -> GetDate() << " )\033[m";

  std::cout << std::endl;
}

//______________________________________________________________________________
// If the required percentage of the loop is achieved, it prints the progress
void General::ProgressBar::Print( const size_t &entry ) {

  if ( entry % ( fNentries*fPercentage/100 ) == 0 ) {
    
    if ( fBarWidth ) {

      std::cout << "\033[" << fColors[ "Bar" ] << "m[";
  
      for ( size_t i = 0; i < fBarWidth; i++ ){
	
	if ( i < fBarWidth*( entry + 1 ) / fNentries )
	  std::cout << "|";
	else
	  std::cout << ".";
      }
      std::cout << "]\033[m";
    }

    std::cout << " \033[" << fColors[ "Percentage" ] << "m" <<
      100*( entry + 1 ) / fNentries << " \%\033[m";

    if ( fUse[ "Time" ] )
      std::cout << " \033[" << fColors[ "Time" ] <<
	"m( Time elapsed: " << this -> GetTime() << " )\033[m";

    if ( fUse[ "StartDate" ] )
      std::cout << " \033[" << fColors[ "StartDate" ] <<
	"m( Started: " << fStartDate << " )\033[m";

    std::cout << "\r" << std::flush;
  }
}

//______________________________________________________________________________
// Sets the colors of the different display options
void General::ProgressBar::SetColor( const char *opts ) {

  std::string str( opts );

  const char *pars[ 5 ] = { "Bar"       ,
			    "Percentage",
			    "Time"      ,
			    "StartDate" ,
			    "EndDate"   };

  for ( unsigned short int i = 0; i < 5; i++ )
    if ( str.find( pars[ i ] ) != std::string::npos )
	fColors[ pars[ i ] ] = this -> GetOption( str, pars[ i ] );
    else
      fColors[ pars[ i ] ] = "";
}

//______________________________________________________________________________
// Starts the progress display
void General::ProgressBar::Start( size_t nentries ) {
  
  fNentries = nentries;

  if ( fUse[ "Time" ] )
    fStart = clock();
  if ( fUse[ "StartDate" ] )
    fStartDate = this -> GetDate();
}

//______________________________________________________________________________


// --- PRIVATE METHODS

//______________________________________________________________________________
// Gets the date depending on the configuration option specified
inline std::string General::ProgressBar::GetDate() {

  time_t  actual_time = time( 0 );
  tm     *time_struct;

  if ( fUse[ "UTC" ] )
    time_struct = gmtime( &actual_time );
  else
    time_struct = localtime( &actual_time );

  std::string res = asctime( time_struct );

  res.erase( res.end() - 1, res.end() );

  return res;
}

//______________________________________________________________________________
// Gets the option for one of the possible selections
inline std::string General::ProgressBar::GetOption( std::string &str, const char *opt ) {

  std::string res;
  std::string::iterator it( str.begin() + str.find( opt ) );

  while ( *it != '=' )
    it++;

  for ( it++; *it != ':' && it != str.end(); it++ )
    res += *it;

  return res;
}

//______________________________________________________________________________
// Gets the time elapsed in format ( days, hours, minutes, seconds ). There are
// only displayed those magnitudes wich have reached a value greater than 0.
inline std::string General::ProgressBar::GetTime() {

  float total_sec( ( ( float ) clock() - fStart ) / CLOCKS_PER_SEC );
  
  int
    ndays  ( total_sec / 86400 ),
    nhours ( total_sec / 3600 - ndays*24 ),
    nmins  ( total_sec / 60 - nhours*60 - ndays*1440 );

  float nsecs ( total_sec - nmins*60 - nhours*3600 - ndays*86400 );

  std::stringstream ss;

  ss << std::fixed << std::setprecision( 4 );

  if ( ndays )
    ss << ndays  << "d "
       << nhours << "h "
       << nmins << "min "
       << nsecs << "sec";
  else
    if ( nhours )
      ss << nhours << "h "
	 << nmins << "min "
	 << nsecs << "sec";
    else
      if ( nmins )
	ss << nmins << "min "
	   << nsecs << "sec";
      else
	ss << nsecs << "sec";

  return ss.str();
}

//______________________________________________________________________________
