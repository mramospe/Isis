/////////////////////////////////////////////////////////
//
//  Analysis package
//
// ------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 02/12/2016
//
// ------------------------------------------------------
//
//  Description:
//
//  Implements the class to perform CLs analysis,
//  providing the tools to make a mu-scan, get the
//  p-values and the ROC curve, etc.
//
// ------------------------------------------------------
/////////////////////////////////////////////////////////                               


#ifndef CLS_ANALYSER
#define CLS_ANALYSER

#include "CLsArray.h"

#include "TH1D.h"
#include "TGraph.h"

#include <cstdlib>
#include <iostream>


//_______________________________________________________________________________

namespace Analysis {

  class CLsAnalyser {

  public:

    // Constructors
    CLsAnalyser();
    CLsAnalyser( const CLsArray &old_hyp,
		 const CLsArray &new_hyp,
		 const int      &npoints = 10000 );

    // Destructor
    ~CLsAnalyser();

    // Methods
    void    Evaluate();
    TH1D*   GetNewHypHist( const char *name, const int &nbins );
    TH1D*   GetOldHypHist( const char *name, const int &nbins );
    double  GetQCLs( const double &q, const char type = 'O' );
    TGraph* GetROC( const int &npoints );
    void    SetNewHypothesis( const CLsArray &new_hyp );
    void    SetOldHypothesis( const CLsArray &old_hyp );

    // Inline methods
    inline double GetAlpha( const double &t );
    inline double GetAlpha( const CLsArray &obs );
    inline double GetBeta( const double &t );
    inline double GetBeta( const CLsArray &obs );
    inline double GetCLs( const CLsArray &obs );
    inline double GetNewHypEvt( const int &index );
    inline double GetOldHypEvt( const int &index );
    inline int    GetSize();
    inline void   SetNpoints( const int &npoints );
    inline double TestStat( const CLsArray &obs );

  protected:

    // Attributes
    CLsArray            fNewHyp;
    std::vector<double> fNewHypArray;
    int                 fNpoints;
    CLsArray            fOldHyp;
    std::vector<double> fOldHypArray;

    // Methods
    double ( CLsArray::*GetNewHypProb )( const CLsArray &values );
    double ( CLsArray::*GetOldHypProb )( const CLsArray &values );
    double GetPValue( const std::vector<double> &list, const double &t0, const char type );
    
  };

  //_______________
  // INLINE METHODS

  // Gets the p-value of the old hypothesis given the test statistics value
  inline double CLsAnalyser::GetAlpha( const double &t ) {
    return this -> GetPValue( fOldHypArray, t, 'O' );
  }
  // Gets the p-value of the old hypothesis given the observation
  inline double CLsAnalyser::GetAlpha( const Analysis::CLsArray &obs ) {
    return this -> GetAlpha( this -> TestStat( obs ) );
  }
  // Gets the p-value of the new hypothesis given the test statistics value
  inline double CLsAnalyser::GetBeta( const double &t ) {
    return this -> GetPValue( fNewHypArray, t, 'N' );
  }
  // Gets the p-value of the new hypothesis given the observation
  inline double CLsAnalyser::GetBeta( const Analysis::CLsArray &obs ) {
    return this -> GetBeta( this -> TestStat( obs ) );
  }
  // Gets the CLs
  inline double CLsAnalyser::GetCLs( const Analysis::CLsArray &obs ) {
    return this -> GetBeta( obs )/( 1 - this -> GetAlpha( obs ) );
  }
  // Gets the new hypothesis test statistics event at position <index>
  inline double CLsAnalyser::GetNewHypEvt( const int &index ) {
    return fNewHypArray[ index ];
  }
  // Gets the old hypothesis test statistics event at position <index>
  inline double CLsAnalyser::GetOldHypEvt( const int &index ) {
    return fOldHypArray[ index ];
  }
  // Gets the size of the arrays in the class
  inline int CLsAnalyser::GetSize() { return fNpoints; }
  // Sets a new number of points for the distributions
  inline void CLsAnalyser::SetNpoints( const int &npoints ) { fNpoints = npoints; }
  // Gets the test statistics value for a given observation
  double CLsAnalyser::TestStat( const Analysis::CLsArray &obs ) {
    return -2*std::log( ( fOldHyp.*GetOldHypProb )( obs )/( fNewHyp.*GetNewHypProb )( obs ) );
  }

}

#endif
