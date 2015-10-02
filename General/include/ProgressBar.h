///////////////////////////////////////////////////////////////////
// ------------------------------------------------------------- //
//						                 //
//  General package  			                         //
//        					                 //
// ------------------------------------------------------------- //
//						                 //
//  AUTHOR: Miguel Ramos Pernas		                         //
//  e-mail: miguel.ramos.pernas@cern.ch		                 //
//						                 //
//  Last update: 08/07/2015			                 //
//   						                 //
// ------------------------------------------------------------- //
//						                 //
//  Description:				                 //
//						                 //
//  Includes the ProgressBar class to display the                //
//  progress of a certain loop. Configuration options            //
//  are:                                                         //
//                                                               //
//  Bar        => Generates a progress bar with size 50          //
//  Percentage => Shows the percentage of the loop each time it  //
//                completes the specified percentage             //
//                specified                                      //
//  Time       => Displays the time elapsed in the loop          //
//  Date       => Allows the display of the date. There are two  //
//                options:                                       //
//                 - Start: shows the starting date              //
//                 - End:   shows the ending date                //
//                By default local time is displayed. If option  //
//                UTC is specified the dates will correspond to  //
//                the UTC date and time.                         //
//                                                               //
//  Options have to be separated by the < : > character. All     //
//  modifications have to be done through the < = > operator in  //
//  the way < Bar=50 >. This sets the ProgressBar class to       //
//  generate a bar of size 50 points. For Date the options can   //
//  be separated by any symbol by < = > or < : >.                //
//                                                               //
//  For the colors, they have to be expresed as ANSI color       //
//  codes. With the same options as above but substituing        //
//  < Date > by < StartDate > and < EndDate >. An example could  //
//  be < Bar=32;41;1 >, meaning the display of the bar with      //
//  green bold lines, and with a red background.                 //
//						                 //
// ------------------------------------------------------------- //
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

    // Methods
    void End();
    void Print( size_t &entry );
    void SetColor( const char *opts );
    void Start( size_t nentries );

  protected:

    // Attributes
    unsigned short int           fBarWidth;
    std::map
    < const char*, std::string > fColors;
    size_t                       fNentries;
    unsigned short int           fPercentage;
    std::string                  fStartDate;
    clock_t                      fStart;
    std::map
    < const char*, bool >        fUse;

  private:

    // Methods
    inline std::string GetDate();
    inline std::string GetOption( std::string &str, const char *opt );
    inline std::string GetTime();
  };

}

#endif
