/////////////////////////////////////////////////////////
//                                                     //
//  Analysis package                                   //
//                                                     //
// --------------------------------------------------- //
//                                                     //
//  AUTHOR: Miguel Ramos Pernas                        //
//  e-mail: miguel.ramos.pernas@cern.ch                //
//                                                     //
//  Last update: 22/02/2016                            //
//                                                     //
// --------------------------------------------------- //
//                                                     //
//  Description:                                       //
//                                                     //
//  Implements the class to perform CLs analysis,      //
//  providing the tools to make a mu-scan, get the     //
//  p-values and the ROC curve, etc.                   //
//                                                     //
// --------------------------------------------------- //
/////////////////////////////////////////////////////////                               


#ifndef CLS_ANALYSIS
#define CLS_ANALYSIS

#include "TH1D.h"
#include "TGraph.h"
#include "TRandom3.h"

#include <cstdlib>
#include <iostream>


//_______________________________________________________________________________

namespace Analysis {

  class CLsArray {

    friend class CLsAnalyser;
  
  public:
  
    // Constructor
    CLsArray();
    CLsArray( const CLsArray &other );
    CLsArray( const double &value, const double &sigma );
    CLsArray( double *values, double *sigmas, const int &size );
    CLsArray( const double &value );
    CLsArray( double *values, const int &size );

    // Destructor
    ~CLsArray();

    // Methods
    CLsArray&        operator =  ( const CLsArray &other );
    double           operator [] ( const int &index );
    CLsArray         operator +  ( const CLsArray &other );
    CLsArray         operator -  ( const CLsArray &other );
    friend CLsArray  operator *  ( const double &value, const CLsArray &other );
    friend CLsArray  operator *  ( const CLsArray &other, const double &value );
    CLsArray         operator /  ( const double &value );

  protected:
    
    // Attributes
    TRandom3    fGenerator;
    const char *fType;
    double     *fMeans;
    double     *fSigmas;
    int         fSize;

    // Methods
    inline CLsArray  GenerateGaussian();
    inline CLsArray  GeneratePoisson();
    inline double    GetGaussianProb( const CLsArray &values );
    inline double    GetPoissonProb( const CLsArray &values );

  };


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
    double  GetQCLs( const double &q, const char *type = "old" );
    TGraph* GetROC( const int &npoints );
    void    SetHypothesis( const CLsArray &old_hyp, const CLsArray &new_hyp, const int &npoints );
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
    inline void   SetNPoints( const int &npoints );
    inline double TestStat( const CLsArray &obs );

  protected:

    // Attributes
    CLsArray            fNewHyp;
    std::vector<double> fNewHypArray;
    int                 fNPoints;
    CLsArray            fOldHyp;
    std::vector<double> fOldHypArray;

    // Methods
    double ( CLsArray::*GetNewHypProb )( const CLsArray &values );
    double ( CLsArray::*GetOldHypProb )( const CLsArray &values );
    double GetPValue( const std::vector<double> &list, const double &t0, const char *type );
    
  };

  //_______________
  // INLINE METHODS

  // Gets the p-value of the old hypothesis given the test statistics value
  inline double Analysis::CLsAnalyser::GetAlpha( const double &t ) {
    return this -> GetPValue( fOldHypArray, t, "old" );
  }
  // Gets the p-value of the old hypothesis given the observation
  inline double Analysis::CLsAnalyser::GetAlpha( const Analysis::CLsArray &obs ) {
    return this -> GetAlpha( this -> TestStat( obs ) );
  }
  // Gets the p-value of the new hypothesis given the test statistics value
  inline double Analysis::CLsAnalyser::GetBeta( const double &t ) {
    return this -> GetPValue( fNewHypArray, t, "new" );
  }
  // Gets the p-value of the new hypothesis given the observation
  inline double Analysis::CLsAnalyser::GetBeta( const Analysis::CLsArray &obs ) {
    return this -> GetBeta( this -> TestStat( obs ) );
  }
  // Gets the CLs
  inline double Analysis::CLsAnalyser::GetCLs( const Analysis::CLsArray &obs ) {
    return this -> GetBeta( obs )/( 1 - this -> GetAlpha( obs ) );
  }
  // Gets the new hypothesis test statistics event at position <index>
  inline double Analysis::CLsAnalyser::GetNewHypEvt( const int &index ) {
    return fNewHypArray[ index ];
  }
  // Gets the old hypothesis test statistics event at position <index>
  inline double Analysis::CLsAnalyser::GetOldHypEvt( const int &index ) {
    return fOldHypArray[ index ];
  }
  // Gets the size of the arrays in the class
  inline int Analysis::CLsAnalyser::GetSize() { return fNPoints; }
  // Sets a new number of points for the distributions
  inline void Analysis::CLsAnalyser::SetNPoints( const int &npoints ) { fNPoints = npoints; }
  // Gets the test statistics value for a given observation
  double Analysis::CLsAnalyser::TestStat( const Analysis::CLsArray &obs ) {
    return -2*std::log( ( fOldHyp.*GetOldHypProb )( obs )/
			( fNewHyp.*GetNewHypProb )( obs ) );
  }


  // Other functions
  inline double GetGaussian( const double &mean, const double &sigma, const double &value );
  inline double GetPoisson( const double &mean, const double &value );

}

#endif
