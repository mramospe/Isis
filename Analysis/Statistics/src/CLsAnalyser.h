/////////////////////////////////////////////////////////
//
//  Analysis package
//
// ------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 17/02/2017
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

    // Main constructor
    CLsAnalyser();
    
    // Constructor that allocates the distribution of the two samples in two vectors
    CLsAnalyser( const CLsArray &old_hyp,
		 const CLsArray &new_hyp,
		 const int      &npoints = 10000 );

    // Destructor
    ~CLsAnalyser();

    // Gets the distribution of the old and new hypothesis
    void Evaluate();

    // Gets the new hypothesis histogram
    TH1D* GetNewHypHist( const char *name, const int &nbins );

    // Gets the old hypothesis histogram
    TH1D* GetOldHypHist( const char *name, const int &nbins );

    // Gets the CLs for a given confidence level (introduce 0.8413 and 1 - 0.8413 to
    // get the region inside 1 sigma).
    double GetQCLs( const double &q, const char type = 'O' );

    // Gets the ROC curve
    TGraph* GetROC( const int &npoints );

    // Sets a new hypothesis. The array containing the distribution of the old new
    // hypothesis is cleared.
    void SetNewHypothesis( const CLsArray &new_hyp );

    // Sets a new hypothesis. The array containing the distribution of the old old
    // hypothesis is cleared.
    void SetOldHypothesis( const CLsArray &old_hyp );

    // Gets the p-value of the old hypothesis given the test statistics value
    inline double GetAlpha( const double &t );

    // Gets the p-value of the old hypothesis given the observation
    inline double GetAlpha( const CLsArray &obs );

    // Gets the p-value of the new hypothesis given the test statistics value
    inline double GetBeta( const double &t );

    // Gets the p-value of the new hypothesis given the observation
    inline double GetBeta( const CLsArray &obs );

    // Gets the CLs
    inline double GetCLs( const CLsArray &obs );

    // Gets the new hypothesis test statistics event at position <index>
    inline double GetNewHypEvt( const int &index );

    // Gets the old hypothesis test statistics event at position <index>
    inline double GetOldHypEvt( const int &index );

    // Gets the size of the arrays in the class
    inline int GetSize();

    // Sets a new number of points for the distributions
    inline void SetNpoints( const int &npoints );

    // Gets the test statistics value for a given observation
    inline double TestStat( const CLsArray &obs );

  protected:

    // New hypothesis
    CLsArray fNewHyp;

    // Test statistics distribution for the new hypothesis
    std::vector<double> fNewHypArray;

    // Number of points to be generated in each step
    int fNpoints;

    // Old hypothesis
    CLsArray fOldHyp;

    // Test statistics distribution for the old hypothesis
    std::vector<double> fOldHypArray;

  protected:
    
    // Pointer to the function to build the new hypothesis probability distribution
    double ( CLsArray::*GetNewHypProb )( const CLsArray &values );

    // Pointer to the function to build the old hypothesis probability distribution
    double ( CLsArray::*GetOldHypProb )( const CLsArray &values );

    // Gets the p-value for a given hypothesis
    double GetPValue( const std::vector<double> &list,
		      const double &t0,
		      const char type );
    
  };

  //_______________________________________________________________________________
  //
  inline double CLsAnalyser::GetAlpha( const double &t ) {
    return this->GetPValue( fOldHypArray, t, 'O' );
  }

  //_______________________________________________________________________________
  //
  inline double CLsAnalyser::GetAlpha( const Analysis::CLsArray &obs ) {
    return this->GetAlpha( this->TestStat( obs ) );
  }

  //_______________________________________________________________________________
  //
  inline double CLsAnalyser::GetBeta( const double &t ) {
    return this->GetPValue( fNewHypArray, t, 'N' );
  }

  //_______________________________________________________________________________
  //
  inline double CLsAnalyser::GetBeta( const Analysis::CLsArray &obs ) {
    return this->GetBeta( this->TestStat( obs ) );
  }

  //_______________________________________________________________________________
  //
  inline double CLsAnalyser::GetCLs( const Analysis::CLsArray &obs ) {
    return this->GetBeta( obs )/( 1 - this->GetAlpha( obs ) );
  }

  //_______________________________________________________________________________
  //
  inline double CLsAnalyser::GetNewHypEvt( const int &index ) {
    return fNewHypArray[ index ];
  }

  //_______________________________________________________________________________
  //
  inline double CLsAnalyser::GetOldHypEvt( const int &index ) {
    return fOldHypArray[ index ];
  }

  //_______________________________________________________________________________
  //
  inline int CLsAnalyser::GetSize() { return fNpoints; }

  //_______________________________________________________________________________
  //
  inline void CLsAnalyser::SetNpoints( const int &npoints ) { fNpoints = npoints; }

  //_______________________________________________________________________________
  //
  inline double CLsAnalyser::TestStat( const Analysis::CLsArray &obs ) {
    return -2*std::log( ( fOldHyp.*GetOldHypProb )( obs )/
			( fNewHyp.*GetNewHypProb )( obs ) );
  }

}

#endif
