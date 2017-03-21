///////////////////////////////////////////////////////////////////
//
//  General package
//
// ----------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 21/03/2017
//
// ----------------------------------------------------------------
///////////////////////////////////////////////////////////////////


#include "ProgressBar.h"

#include <iostream>
#include <sstream>
#include <iomanip>


//______________________________________________________________________________

namespace Isis {

  //______________________________________________________________________________
  //
  ProgressBar::ProgressBar( const char *opts ) :
    fBarWidth( 50 ), fPercentage( 1 ) {

    fUse[ "Bar"  ]      = true;
    fUse[ "Time" ]      = false;
    fUse[ "StartDate" ] = false;
    fUse[ "EndDate"   ] = false;

    if ( opts ) {

      std::string str( opts );

      if ( str.find( "Bar" ) != std::string::npos )
	fBarWidth = std::atoi( this->GetOption( str, "Bar" ).c_str() );

      if ( str.find( "Percentage" ) != std::string::npos )
	fPercentage = std::atoi( this->GetOption( str, "Percentage" ).c_str() );

      if ( str.find( "Time" ) != std::string::npos )
	fUse[ "Time" ] = true;
    
      if ( str.find( "Date" ) != std::string::npos ) {

	std::string opt( this->GetOption( str, "Date" ) );

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
  //
  ProgressBar::~ProgressBar() { }

  //______________________________________________________________________________
  //
  void ProgressBar::End() {

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
	"m( Time elapsed: " << this->GetTime() << " )\033[m";

    if ( fUse[ "StartDate" ] )
      std::cout << " \033[" << fColors[ "StartDate" ] <<
	"m( Started: " << fStartDate << " )\033[m";

    if ( fUse[ "EndDate" ] )
      std::cout << " \033[" << fColors[ "EndDate" ] <<
	"m( Finished: " << this->GetDate() << " )\033[m";

    std::cout << std::endl;
  }

  //______________________________________________________________________________
  //
  void ProgressBar::Print( const size_t &entry ) {

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
	  "m( Time elapsed: " << this->GetTime() << " )\033[m";

      if ( fUse[ "StartDate" ] )
	std::cout << " \033[" << fColors[ "StartDate" ] <<
	  "m( Started: " << fStartDate << " )\033[m";

      std::cout << "\r" << std::flush;
    }
  }

  //______________________________________________________________________________
  //
  void ProgressBar::SetColor( const char *opts ) {

    std::string str( opts );

    const char *pars[ 5 ] = { "Bar"       ,
			      "Percentage",
			      "Time"      ,
			      "StartDate" ,
			      "EndDate"   };

    for ( unsigned short int i = 0; i < 5; i++ )
      if ( str.find( pars[ i ] ) != std::string::npos )
	fColors[ pars[ i ] ] = this->GetOption( str, pars[ i ] );
      else
	fColors[ pars[ i ] ] = "";
  }

  //______________________________________________________________________________
  //
  void ProgressBar::Start( size_t nentries ) {
  
    fNentries = nentries;

    if ( fUse[ "Time" ] )
      fStart = clock();
    if ( fUse[ "StartDate" ] )
      fStartDate = this->GetDate();
  }

  //______________________________________________________________________________
  //
  inline std::string ProgressBar::GetDate() {

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
  //
  inline std::string ProgressBar::GetOption( std::string &str, const char *opt ) {

    std::string res;
    std::string::iterator it( str.begin() + str.find( opt ) );

    while ( *it != '=' )
      it++;

    for ( it++; *it != ':' && it != str.end(); it++ )
      res += *it;

    return res;
  }

  //______________________________________________________________________________
  //
  inline std::string ProgressBar::GetTime() {

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

}
