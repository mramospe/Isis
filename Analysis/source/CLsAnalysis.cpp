/////////////////////////////////////////////////////////
//                                                     //
//  Analysis package                                   //
//                                                     //
// --------------------------------------------------- //
//                                                     //
//  AUTHOR: Miguel Ramos Pernas                        //
//  e-mail: miguel.ramos.pernas@cern.ch                //
//                                                     //
//  Last update: 08/08/2015                            //
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


#include <cstring>
#include <algorithm>
#include <cmath>

#include "CLsAnalysis.h"


//_______________________________________________________________________________
/////////////////////////////
//     CLS ARRAY CLASS     //
/////////////////////////////


// -- CONSTRUCTORS AND DESTRUCTOR

//_______________________________________________________________________________
// Main constructor
AnalysisPackage::CLsArray::CLsArray() : fGenerator( 0 ),
					fType( 0 ),
					fMeans( 0 ),
					fSigmas( 0 ),
					fSize( 0 ) { }

//_______________________________________________________________________________
// Copy constructor
AnalysisPackage::CLsArray::CLsArray( const AnalysisPackage::CLsArray &obj ) :
  fGenerator( 0 ) {

  fType  = obj.fType;
  fSize  = obj.fSize;
  
  fMeans = new double[ fSize ];
  for ( int i = 0; i < fSize; i++ )
    fMeans[ i ] = obj.fMeans[ i ];
    
  if ( obj.fSigmas ) {
      
    fSigmas = new double[ fSize ];
    for ( int i = 0; i < fSize; i++ )
      fSigmas[ i ] = obj.fSigmas[ i ];
  }
  else
    fSigmas = 0;
}

//_______________________________________________________________________________
// Constructor for single-values ( Gaussian )
AnalysisPackage::CLsArray::CLsArray( double value, double sigma ) :
  fGenerator( 0 ),
  fType( "Gaussian" ) {

  fSize      = 1;
  fMeans     = new double[ 1 ];
  fSigmas    = new double[ 1 ];
  fMeans[0]  = value;  
  fSigmas[0] = sigma;
}

//_______________________________________________________________________________
// Constructor for multiple-values ( Gaussian )
AnalysisPackage::CLsArray::CLsArray( double *values,
				     double *sigmas,
				     int     size ) :
  fGenerator( 0 ),
  fType( "Gaussian" ) {

  fSize   = size;
  fMeans  = new double[ fSize ];
  fSigmas = new double[ fSize ];

  for (int i = 0; i < fSize; i++) {
    fMeans[ i ]  = values[ i ];
    fSigmas[ i ] = sigmas[ i ];
  }
}

//_______________________________________________________________________________
// Constructor for single-values ( Poisson )
AnalysisPackage::CLsArray::CLsArray( double value ) :
  fGenerator( 0 ),
  fType( "Poisson" ),
  fSigmas( 0 ) {

  fSize     = 1;
  fMeans    = new double[ 1 ];
  fMeans[0] = value;
}

//_______________________________________________________________________________
// Constructor for multiple-values ( Poisson )
AnalysisPackage::CLsArray::CLsArray( double *values, int size ) :
  fGenerator( 0 ),
  fType( "Poisson" ),
  fSigmas( 0 ) {

  fSize  = size;
  fMeans = new double[ fSize ];

  for (int i = 0; i < fSize; i++)
    fMeans[ i ] = values[ i ];
}

//_______________________________________________________________________________
// Destructor
AnalysisPackage::CLsArray::~CLsArray() {

  delete[] fMeans;
  if ( fSigmas )
    delete[] fSigmas;
}

//_______________________________________________________________________________


// -- PUBLIC METHODS

//_______________________________________________________________________________
// Defines the <=> operator
AnalysisPackage::CLsArray&
AnalysisPackage::CLsArray::operator = ( AnalysisPackage::CLsArray obj ) {
  
  if ( obj.fType )
    fType = obj.fType;

  if ( obj.fMeans ) {

    delete[] fMeans;

    fSize  = obj.fSize;
    fMeans = new double[ fSize ];

    for ( int i = 0; i < fSize; i++ )
      fMeans[ i ] = obj.fMeans[ i ];
  }

  if ( obj.fSigmas ) {
    
    delete[] fSigmas;

    fSigmas = new double[ fSize ];

    for ( int i = 0; i < fSize; i++ )
      fSigmas[ i ] = obj.fSigmas[ i ];
  }

  return *this;
}

//_______________________________________________________________________________
// Defines the <get_item> operator
double AnalysisPackage::CLsArray::operator [] ( int index ) { return fMeans[index]; }

//_______________________________________________________________________________
// Defines the <+> operator with CLsArray objects
AnalysisPackage::CLsArray
AnalysisPackage::CLsArray::operator + ( AnalysisPackage::CLsArray obj ) {

  double means_res[ fSize ];

  if ( !strcmp( fType, "Gaussian" ) and !strcmp( obj.fType, "Gaussian" ) ) {
    
    double sigmas_res[ fSize ];

    for ( int i = 0; i < fSize; i++ ) {
      means_res[ i ]  = fMeans[ i ] + obj[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( fSigmas[ i ]*fSigmas[ i ] +
					     obj.fSigmas[ i ]*obj.fSigmas[ i ] ) );
    }
    
    return AnalysisPackage::CLsArray( means_res, sigmas_res, fSize );
  }
  else if ( !strcmp( fType, "Gaussian" ) and !strcmp( obj.fType, "Poisson" ) ) {

    double sigmas_res[ fSize ];

    for ( int i = 0; i < fSize; i++ ) {
      means_res[ i ]  = fMeans[ i ] + obj[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( fSigmas[ i ]*fSigmas[ i ]
					     + obj[ i ] ) );
    }
    
    return AnalysisPackage::CLsArray( means_res, sigmas_res, fSize );
  }
  else if ( !strcmp( fType, "Poisson" ) and !strcmp( obj.fType, "Gaussian" ) ) {

    double sigmas_res[ fSize ];

    for ( int i = 0; i < fSize; i++ ) {
      means_res[ i ]  = fMeans[ i ] + obj[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( obj[ i ]*obj[ i ]
					     + fMeans[ i ] ) );
    }
    
    return AnalysisPackage::CLsArray( means_res, sigmas_res, fSize );
  }
  else {
   
    for ( int i = 0; i < fSize; i++ )
      means_res[ i ] = fMeans[ i ] + obj[ i ];

    return AnalysisPackage::CLsArray( means_res, fSize );
  }
}

//_______________________________________________________________________________
// Defines the <-> operator
AnalysisPackage::CLsArray
AnalysisPackage::CLsArray::operator - ( AnalysisPackage::CLsArray obj ) {

  double means_res[ fSize ];

  if ( !strcmp( fType, "Gaussian" ) and !strcmp( obj.fType, "Gaussian" ) ) {
  
    double sigmas_res[ fSize ];

    for ( int i = 0; i < fSize; i++ ) {
      means_res[ i ]  = fMeans[ i ] + obj[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( fSigmas[ i ]*fSigmas[ i ] -
					     obj.fSigmas[ i ]*obj.fSigmas[ i ] ) );
    }
    
    return AnalysisPackage::CLsArray( means_res, sigmas_res, fSize );
  }
  else if ( !strcmp( fType, "Gaussian" ) and !strcmp( obj.fType, "Poisson" ) ) {

    double sigmas_res[ fSize ];

    for ( int i = 0; i < fSize; i++ ) {
      means_res[ i ]  = fMeans[ i ] + obj[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( fSigmas[ i ]*fSigmas[ i ]
					     - obj[ i ] ) );
    }
    
    return AnalysisPackage::CLsArray( means_res, sigmas_res, fSize );
  }
  else if ( !strcmp( fType, "Poisson" ) and !strcmp( obj.fType, "Gaussian" ) ) {

    double sigmas_res[ fSize ];

    for ( int i = 0; i < fSize; i++ ) {
      means_res[ i ]  = fMeans[ i ] - obj[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( obj[ i ]*obj[ i ]
					     - fMeans[ i ] ) );
    }
    
    return AnalysisPackage::CLsArray( means_res, sigmas_res, fSize );
  }
  else {
   
    for ( int i = 0; i < fSize; i++ )
      means_res[ i ] = fMeans[ i ] - obj[ i ];

    return AnalysisPackage::CLsArray( means_res, fSize );
  }
}

//_______________________________________________________________________________
// Defines the <*> operator
AnalysisPackage::CLsArray AnalysisPackage::operator * ( double value, CLsArray array ) {

  double means_res[ array.fSize ];

  if ( !strcmp( array.fType, "Gaussian" ) ) {

    double sigmas_res[ array.fSize ];

    for ( int i = 0; i < array.fSize; i++ ) {
      means_res[ i ]  = value*array.fMeans[ i ];
      sigmas_res[ i ] = value*array.fSigmas[ i ];
    }

    return AnalysisPackage::CLsArray( means_res, sigmas_res, array.fSize );
  }
  else {

    for ( int i = 0; i < array.fSize; i++ )
      means_res[ i ]  = value*array.fMeans[ i ];

    return AnalysisPackage::CLsArray( means_res, array.fSize );
  }
}

//_______________________________________________________________________________
// Defines the <*> operator
AnalysisPackage::CLsArray AnalysisPackage::operator * ( CLsArray array, double value ) {

  int    size( array.fSize );
  double means_res[ size ];

  if ( !strcmp( array.fType, "Gaussian" ) ) {

    double sigmas_res[ size ];

    for ( int i = 0; i < size; i++ ) {
      means_res[ i ]  = value*array.fMeans[ i ];
      sigmas_res[ i ] = value*array.fSigmas[ i ];
    }

    return AnalysisPackage::CLsArray( means_res, sigmas_res, size );
  }
  else {

    for ( int i = 0; i < size; i++ )
      means_res[ i ]  = value*array.fMeans[ i ];

    return AnalysisPackage::CLsArray( means_res, size );
  }
}

//_______________________________________________________________________________
// Defines the </> operator
AnalysisPackage::CLsArray AnalysisPackage::CLsArray::operator / ( double value ) {

  return ( *this )*1./value;
}

//_______________________________________________________________________________


// -- PROTECTED METHODS

//_______________________________________________________________________________
// Generates an array filled with random gaussian numbers following the
// distribution given by the means and sigmas owned by this class.
inline AnalysisPackage::CLsArray AnalysisPackage::CLsArray::GenerateGaussian() {

  double res[ fSize ];

  for ( int i = 0; i < fSize; i++ )
    res[ i ] = fGenerator.Gaus( fMeans[ i ], fSigmas[ i ] );

  return AnalysisPackage::CLsArray( res, fSize );;
}

//_______________________________________________________________________________
// Generates an array filled with random poissonian numbers following the
// distribution given by the means and sigmas owned by this class.
inline AnalysisPackage::CLsArray AnalysisPackage::CLsArray::GeneratePoisson() {

  double res[ fSize ];

  for ( int i = 0; i < fSize; i++ )
    res[ i ] = fGenerator.Poisson( fMeans[ i ] );
  
  return AnalysisPackage::CLsArray( res, fSize );
}

//_______________________________________________________________________________
// Gets the gaussian probability of having the given value(s)
inline double AnalysisPackage::CLsArray::GetGaussianProb( AnalysisPackage::CLsArray &values  ) {

  double res( 1 );

  for ( int i = 0; i < fSize; i++ )
    res *= GetGaussian( fMeans[ i ], fSigmas[ i ], values[ i ] );

  return res;
}

//_______________________________________________________________________________
// Gets the poissonian probability of having the given value(s)
inline double AnalysisPackage::CLsArray::GetPoissonProb( AnalysisPackage::CLsArray &values  ) {

  double res( 1 );

  for ( int i = 0; i < fSize; i++ )
    res *= GetPoisson( fMeans[ i ], values[ i ] );

  return res;
}

//_______________________________________________________________________________



//_______________________________________________________________________________
////////////////////////////////
//     CLS ANALYSIS CLASS     //
////////////////////////////////


// -- CONSTRUCTORS AND DESTRUCTOR

//_______________________________________________________________________________
// Main constructor
AnalysisPackage::CLsAnalyzer::CLsAnalyzer() { }

//_______________________________________________________________________________
// Constructor that allocates the distribution of the two samples in two vectors
AnalysisPackage::CLsAnalyzer::CLsAnalyzer( AnalysisPackage::CLsArray old_hyp,
					   AnalysisPackage::CLsArray new_hyp,
					   int    npoints ) {

  fNewHyp  = new_hyp;
  fOldHyp  = old_hyp;
  fNPoints = npoints;

  if ( !strcmp( fNewHyp.fType, "Gaussian" ) )
    GetNewHypProb = &AnalysisPackage::CLsArray::GetGaussianProb;
  else
    GetNewHypProb = &AnalysisPackage::CLsArray::GetPoissonProb;
  if ( !strcmp( fOldHyp.fType, "Gaussian" ) )
    GetOldHypProb = &AnalysisPackage::CLsArray::GetGaussianProb;
  else
    GetOldHypProb = &AnalysisPackage::CLsArray::GetPoissonProb;
}

//_______________________________________________________________________________
// Destructor
AnalysisPackage::CLsAnalyzer::~CLsAnalyzer() { }

//_______________________________________________________________________________


// -- PUBLIC METHODS

//_______________________________________________________________________________
// Gets the distribution of the old and new hypothesis
void AnalysisPackage::CLsAnalyzer::Evaluate() {
  
  fNewHypArray = std::vector<double>( fNPoints );
  fOldHypArray = std::vector<double>( fNPoints );

  if ( !strcmp( fNewHyp.fType, "Gaussian" ) )
    for ( int ievt = 0; ievt < fNPoints; ievt++ )
      fNewHypArray[ ievt ] = this -> TestStat( fNewHyp.GenerateGaussian() );
  else
    for ( int ievt = 0; ievt < fNPoints; ievt++ )
      fNewHypArray[ ievt ] = this -> TestStat( fNewHyp.GeneratePoisson() );
  if ( !strcmp( fOldHyp.fType, "Gaussian" ) )
    for ( int ievt = 0; ievt < fNPoints; ievt++ )
      fOldHypArray[ ievt ] = this -> TestStat( fOldHyp.GenerateGaussian() );
  else
    for ( int ievt = 0; ievt < fNPoints; ievt++ )
      fOldHypArray[ ievt ] = this -> TestStat( fOldHyp.GeneratePoisson() );

  std::sort( fNewHypArray.begin(), fNewHypArray.end() );
  std::sort( fOldHypArray.begin(), fOldHypArray.end() );
}

//_______________________________________________________________________________
// Gets the p-value of the old hypothesis given the test statistics value
double AnalysisPackage::CLsAnalyzer::GetAlpha( double t ) {

  return this -> GetPValue( fOldHypArray, t, "old" );
}

//_______________________________________________________________________________
// Gets the p-value of the old hypothesis given the observation
double AnalysisPackage::CLsAnalyzer::GetAlpha( AnalysisPackage::CLsArray obs ) {

  return this -> GetAlpha( this -> TestStat( obs ) );
}

//_______________________________________________________________________________
// Gets the p-value of the new hypothesis given the test statistics value
double AnalysisPackage::CLsAnalyzer::GetBeta( double t ) {

  return this -> GetPValue( fNewHypArray, t, "new" );
}

//_______________________________________________________________________________
// Gets the p-value of the new hypothesis given the observation
double AnalysisPackage::CLsAnalyzer::GetBeta( AnalysisPackage::CLsArray obs ) {

  return this -> GetBeta( this -> TestStat( obs ) );
}

//_______________________________________________________________________________
// Gets the CLs
double AnalysisPackage::CLsAnalyzer::GetCLs( AnalysisPackage::CLsArray obs ) {

  return this -> GetBeta( obs )/( 1 - this -> GetAlpha( obs ) );
}

//_______________________________________________________________________________
// Gets the new hypothesis test statistics event at position <index>
double AnalysisPackage::CLsAnalyzer::GetNewHypEvt( int index ) {

  return fNewHypArray[ index ];
}

//_______________________________________________________________________________
// Gets the new hypothesis histogram
TH1D* AnalysisPackage::CLsAnalyzer::GetNewHypHist( const char *name, int nbins ) {

  double
    step( ( fNewHypArray[ fNPoints - 1 ] - fOldHypArray[ 0 ] )/2 ),
    xmin( fOldHypArray[ 0 ] - step ),
    xmax( fNewHypArray[ fNPoints - 1 ] + step );

  TH1D *hist = new TH1D( name,
			 name,
			 nbins,
			 xmin,
			 xmax );

  for ( int i = 0; i < fNPoints; i++ )
    hist -> Fill( fNewHypArray[ i ] );

  return hist;
}

//_______________________________________________________________________________
// Gets the old hypothesis test statistics event at position <index>
double AnalysisPackage::CLsAnalyzer::GetOldHypEvt( int index ) {

  return fOldHypArray[ index ];
}

//_______________________________________________________________________________
// Gets the old hypothesis histogram
TH1D* AnalysisPackage::CLsAnalyzer::GetOldHypHist( const char *name, int nbins ) {

  double
    step( ( fNewHypArray[ fNPoints - 1 ] - fOldHypArray[ 0 ] )/2 ),
    xmin( fOldHypArray[ 0 ] - step ),
    xmax( fNewHypArray[ fNPoints - 1 ] + step );

  TH1D *hist = new TH1D( name,
			 name,
			 nbins,
			 xmin,
			 xmax );

  for ( int i = 0; i < fNPoints; i++ )
    hist -> Fill( fOldHypArray[ i ] );

  return hist;
}

//_______________________________________________________________________________
// Gets the p-value for a given hypothesis
double AnalysisPackage::CLsAnalyzer::GetPValue( std::vector<double> &list,
						double              &t0,
						const char            *type ) {

  int evts( 0 );

  std::vector<double>::iterator it;

  if ( !strcmp( type, "new" ) ) {
    for ( it = list.begin(); it != list.end(); it++ )
      if ( *it < t0 )
	evts++;
  }
  else {
    for ( it = list.begin(); it != list.end(); it++ )
      if ( *it > t0 )
	evts++;
  }

  return ( double ) evts/fNPoints;
}

//_______________________________________________________________________________
// Gets the CLs for a given confidence level ( introduce 0.8413 and 1 - 0.8413
// to get the region inside 1 sigma ).
double AnalysisPackage::CLsAnalyzer::GetQCLs( double q, const char *type ) {

  double tq;

  if ( !strcmp( type, "new" ) )
    tq = fNewHypArray[ int( q*fNPoints ) ];
  else
    tq = fOldHypArray[ int( q*fNPoints ) ];

  return this -> GetBeta( tq )/( 1 - this -> GetAlpha( tq ) );    
}

//_______________________________________________________________________________
// Gets the ROC curve
TGraph* AnalysisPackage::CLsAnalyzer::GetROC( int npoints ) {

  double
    it_min( fOldHypArray[ 0 ] ),
    step( ( fNewHypArray[ fNPoints - 1 ] - it_min )/npoints );

  TGraph *ROC = new TGraph();
  
  for ( int i = 0; i < npoints; i++ )
    ROC -> SetPoint( i,
		     this -> GetAlpha( it_min + i*step ),
		     1 - this -> GetBeta( it_min + i*step ) );

  return ROC;
}

//_______________________________________________________________________________
// Gets the size of the arrays in the class
int AnalysisPackage::CLsAnalyzer::GetSize() { return fNPoints; }

//_______________________________________________________________________________
// Sets a new hypothesis. The arrays containing the distributions of the old
// hypothesis are cleared.
void AnalysisPackage::CLsAnalyzer::SetHypothesis( AnalysisPackage::CLsArray old_hyp,
						  AnalysisPackage::CLsArray new_hyp,
						  int    npoints ) {

  this -> SetNewHypothesis( new_hyp );
  this -> SetOldHypothesis( old_hyp );

  fNPoints = npoints;
}

//_______________________________________________________________________________
// Sets a new hypothesis. The array containing the distribution of the old new
// hypothesis is cleared.
void AnalysisPackage::CLsAnalyzer::SetNewHypothesis( AnalysisPackage::CLsArray new_hyp ) {

  fNewHyp = new_hyp;
  fNewHypArray.clear();

  if ( !strcmp( fNewHyp.fType, "Gaussian" ) )
    GetNewHypProb = &AnalysisPackage::CLsArray::GetGaussianProb;
  else
    GetNewHypProb = &AnalysisPackage::CLsArray::GetPoissonProb;
}

//_______________________________________________________________________________
// Sets a new number of points for the distributions
void AnalysisPackage::CLsAnalyzer::SetNPoints( int npoints ) { fNPoints = npoints; }

//_______________________________________________________________________________
// Sets a new hypothesis. The array containing the distribution of the old old
// hypothesis is cleared.
void AnalysisPackage::CLsAnalyzer::SetOldHypothesis( AnalysisPackage::CLsArray old_hyp ) {

  fOldHyp = old_hyp;
  fOldHypArray.clear();

  if ( !strcmp( fOldHyp.fType, "Gaussian" ) )
    GetOldHypProb = &AnalysisPackage::CLsArray::GetGaussianProb;
  else
    GetOldHypProb = &AnalysisPackage::CLsArray::GetPoissonProb;
}

//_______________________________________________________________________________
// Gets the test statistics value for a given observation
double AnalysisPackage::CLsAnalyzer::TestStat( AnalysisPackage::CLsArray obs ) {

  return -2*std::log( ( fOldHyp.*GetOldHypProb )( obs )/
		      ( fNewHyp.*GetNewHypProb )( obs ) );
}

//_______________________________________________________________________________



//_______________________________________________________________________________
/////////////////////////////
//     OTHER FUNCTIONS     //
/////////////////////////////


//_______________________________________________________________________________
// Gets the gaussian probability associated to this value
inline double AnalysisPackage::GetGaussian( double mean,
					    double sigma,
					    double value ) {

  return 1./( std::sqrt( 2*M_PI )*sigma )*
    std::exp( - ( value - mean )*( value - mean )/( 2*sigma*sigma ) );
}

//_______________________________________________________________________________
// Gets the poissonian probability associated to this value
inline double AnalysisPackage::GetPoisson( double mean,
					   double value ) {

  return std::pow( mean, value )*std::exp( - mean )/std::tgamma( value + 1 );
}
