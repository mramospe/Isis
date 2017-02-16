///////////////////////////////////////////////////////////////////
//
//  General package
//
// ----------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 16/02/2017
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


#ifndef PROGRESS_BAR
#define PROGRESS_BAR

#include <cstdlib>
#include <string>
#include <map>
#include <ctime>


//_______________________________________________________________________________

namespace General {

  class ProgressBar {

  public:

    // Constructor
    ProgressBar( const char *opts = 0 );

    // Destructor
    ~ProgressBar();

    // Ends the display of the progress
    void End();

    // If the required percentage of the loop is achieved, it prints the progress
    void Print( const size_t &entry );

    // Sets the colors of the different display options
    void SetColor( const char *opts );

    // Starts the progress display
    void Start( size_t nentries );

  protected:

    // Width of the progress bar
    unsigned short int fBarWidth;

    // Colors for each 
    std::map<const char*, std::string> fColors;

    // Number of entries to perform the iteration
    size_t fNentries;

    // Percentage needed to draw each mark
    unsigned short int fPercentage;

    // String to store the starting date
    std::string fStartDate;

    // Variable to store the starting time
    clock_t fStart;

    // Map storing which information must be shown
    std::map<const char*, bool> fUse;

  private:

    // Gets the date depending on the configuration option specified
    inline std::string GetDate();

    // Gets the option for one of the possible selections
    inline std::string GetOption( std::string &str, const char *opt );

    // Gets the time elapsed in format ( days, hours, minutes, seconds ). There are
    // only displayed those magnitudes wich have reached a value greater than 0.
    inline std::string GetTime();
  };

}

#endif
