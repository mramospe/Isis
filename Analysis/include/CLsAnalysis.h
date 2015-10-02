/////////////////////////////////////////////////////////
//                                                     //
//  Analysis package                                   //
//                                                     //
// --------------------------------------------------- //
//                                                     //
//  AUTHOR: Miguel Ramos Pernas                        //
//  e-mail: miguel.ramos.pernas@cern.ch                //
//                                                     //
//  Last update: 12/06/2015                            //
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

#include <iostream>
#include <cstdlib>

#include "TH1D.h"
#include "TGraph.h"
#include "TRandom3.h"


//_______________________________________________________________________________

namespace AnalysisPackage {

  class CLsArray {

    friend class CLsAnalyzer;
  
  public:
  
    // Constructor
    CLsArray();
    CLsArray( const CLsArray& );
    CLsArray( double, double );
    CLsArray( double*, double*, int );
    CLsArray( double );
    CLsArray( double*, int );

    // Destructor
    ~CLsArray();

    // Methods
    CLsArray&        operator =  ( CLsArray );
    double           operator [] ( int );
    CLsArray         operator +  ( CLsArray );
    CLsArray         operator -  ( CLsArray );
    friend CLsArray  operator *  ( double, CLsArray );
    friend CLsArray  operator *  ( CLsArray, double );
    CLsArray         operator /  ( double );

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
    inline double    GetGaussianProb( CLsArray& );
    inline double    GetPoissonProb( CLsArray& );

  };


  class CLsAnalyzer {

  public:

    // Constructors
    CLsAnalyzer();
    CLsAnalyzer( CLsArray,
		 CLsArray,
		 int npoints = 10000 );

    // Destructor
    ~CLsAnalyzer();

    // Methods
    void    Evaluate();
    double  GetAlpha( double );
    double  GetAlpha( CLsArray );
    double  GetBeta( double );
    double  GetBeta( CLsArray );
    double  GetCLs( CLsArray );
    double  GetNewHypEvt( int );
    TH1D*   GetNewHypHist( const char*, int );
    double  GetOldHypEvt( int );
    TH1D*   GetOldHypHist( const char*, int );
    double  GetQCLs( double, const char *type = "old" );
    TGraph* GetROC( int );
    int     GetSize();
    void    SetHypothesis( CLsArray, CLsArray, int );
    void    SetNewHypothesis( CLsArray );
    void    SetNPoints( int );
    void    SetOldHypothesis( CLsArray );
    double  TestStat( CLsArray );

  protected:

    // Attributes
    CLsArray            fNewHyp;
    std::vector<double> fNewHypArray;
    int                 fNPoints;
    CLsArray            fOldHyp;
    std::vector<double> fOldHypArray;

    // Methods
    double ( CLsArray::*GetNewHypProb )( CLsArray& );
    double ( CLsArray::*GetOldHypProb )( CLsArray& );
    double GetPValue( std::vector<double>&, double&, const char* );
    
  };


  // Other functions
  inline double GetGaussian( double, double, double );
  inline double GetPoisson( double, double );

}

#endif
