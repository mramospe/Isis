/////////////////////////////////////////////////////////
//                                                     //
//  Analysis package                                   //
//                                                     //
// --------------------------------------------------- //
//                                                     //
//  AUTHOR: Miguel Ramos Pernas                        //
//  e-mail: miguel.ramos.pernas@cern.ch                //
//                                                     //
//  Last update: 22/10/2015                            //
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
Analysis::CLsArray::CLsArray() : fGenerator( 0 ),
				 fType( 0 ),
				 fMeans( 0 ),
				 fSigmas( 0 ),
				 fSize( 0 ) { }

//_______________________________________________________________________________
// Copy constructor
Analysis::CLsArray::CLsArray( const Analysis::CLsArray &obj ) :
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
Analysis::CLsArray::CLsArray( double value, double sigma ) :
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
Analysis::CLsArray::CLsArray( double *values,
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
Analysis::CLsArray::CLsArray( double value ) :
  fGenerator( 0 ),
  fType( "Poisson" ),
  fSigmas( 0 ) {

  fSize     = 1;
  fMeans    = new double[ 1 ];
  fMeans[0] = value;
}

//_______________________________________________________________________________
// Constructor for multiple-values ( Poisson )
Analysis::CLsArray::CLsArray( double *values, int size ) :
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
Analysis::CLsArray::~CLsArray() {

  delete[] fMeans;
  if ( fSigmas )
    delete[] fSigmas;
}

//_______________________________________________________________________________


// -- PUBLIC METHODS

//_______________________________________________________________________________
// Defines the <=> operator
Analysis::CLsArray&
Analysis::CLsArray::operator = ( Analysis::CLsArray obj ) {
  
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
double Analysis::CLsArray::operator [] ( int index ) { return fMeans[index]; }

//_______________________________________________________________________________
// Defines the <+> operator with CLsArray objects
Analysis::CLsArray
Analysis::CLsArray::operator + ( Analysis::CLsArray obj ) {

  double means_res[ fSize ];

  if ( !strcmp( fType, "Gaussian" ) and !strcmp( obj.fType, "Gaussian" ) ) {
    
    double sigmas_res[ fSize ];

    for ( int i = 0; i < fSize; i++ ) {
      means_res[ i ]  = fMeans[ i ] + obj[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( fSigmas[ i ]*fSigmas[ i ] +
					     obj.fSigmas[ i ]*obj.fSigmas[ i ] ) );
    }
    
    return Analysis::CLsArray( means_res, sigmas_res, fSize );
  }
  else if ( !strcmp( fType, "Gaussian" ) and !strcmp( obj.fType, "Poisson" ) ) {

    double sigmas_res[ fSize ];

    for ( int i = 0; i < fSize; i++ ) {
      means_res[ i ]  = fMeans[ i ] + obj[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( fSigmas[ i ]*fSigmas[ i ]
					     + obj[ i ] ) );
    }
    
    return Analysis::CLsArray( means_res, sigmas_res, fSize );
  }
  else if ( !strcmp( fType, "Poisson" ) and !strcmp( obj.fType, "Gaussian" ) ) {

    double sigmas_res[ fSize ];

    for ( int i = 0; i < fSize; i++ ) {
      means_res[ i ]  = fMeans[ i ] + obj[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( obj[ i ]*obj[ i ]
					     + fMeans[ i ] ) );
    }
    
    return Analysis::CLsArray( means_res, sigmas_res, fSize );
  }
  else {
   
    for ( int i = 0; i < fSize; i++ )
      means_res[ i ] = fMeans[ i ] + obj[ i ];

    return Analysis::CLsArray( means_res, fSize );
  }
}

//_______________________________________________________________________________
// Defines the <-> operator
Analysis::CLsArray
Analysis::CLsArray::operator - ( Analysis::CLsArray obj ) {

  double means_res[ fSize ];

  if ( !strcmp( fType, "Gaussian" ) and !strcmp( obj.fType, "Gaussian" ) ) {
  
    double sigmas_res[ fSize ];

    for ( int i = 0; i < fSize; i++ ) {
      means_res[ i ]  = fMeans[ i ] + obj[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( fSigmas[ i ]*fSigmas[ i ] -
					     obj.fSigmas[ i ]*obj.fSigmas[ i ] ) );
    }
    
    return Analysis::CLsArray( means_res, sigmas_res, fSize );
  }
  else if ( !strcmp( fType, "Gaussian" ) and !strcmp( obj.fType, "Poisson" ) ) {

    double sigmas_res[ fSize ];

    for ( int i = 0; i < fSize; i++ ) {
      means_res[ i ]  = fMeans[ i ] + obj[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( fSigmas[ i ]*fSigmas[ i ]
					     - obj[ i ] ) );
    }
    
    return Analysis::CLsArray( means_res, sigmas_res, fSize );
  }
  else if ( !strcmp( fType, "Poisson" ) and !strcmp( obj.fType, "Gaussian" ) ) {

    double sigmas_res[ fSize ];

    for ( int i = 0; i < fSize; i++ ) {
      means_res[ i ]  = fMeans[ i ] - obj[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( obj[ i ]*obj[ i ]
					     - fMeans[ i ] ) );
    }
    
    return Analysis::CLsArray( means_res, sigmas_res, fSize );
  }
  else {
   
    for ( int i = 0; i < fSize; i++ )
      means_res[ i ] = fMeans[ i ] - obj[ i ];

    return Analysis::CLsArray( means_res, fSize );
  }
}

//_______________________________________________________________________________
// Defines the <*> operator
Analysis::CLsArray Analysis::operator * ( double value, CLsArray array ) {

  double means_res[ array.fSize ];

  if ( !strcmp( array.fType, "Gaussian" ) ) {

    double sigmas_res[ array.fSize ];

    for ( int i = 0; i < array.fSize; i++ ) {
      means_res[ i ]  = value*array.fMeans[ i ];
      sigmas_res[ i ] = value*array.fSigmas[ i ];
    }

    return Analysis::CLsArray( means_res, sigmas_res, array.fSize );
  }
  else {

    for ( int i = 0; i < array.fSize; i++ )
      means_res[ i ]  = value*array.fMeans[ i ];

    return Analysis::CLsArray( means_res, array.fSize );
  }
}

//_______________________________________________________________________________
// Defines the <*> operator
Analysis::CLsArray Analysis::operator * ( CLsArray array, double value ) {

  int    size( array.fSize );
  double means_res[ size ];

  if ( !strcmp( array.fType, "Gaussian" ) ) {

    double sigmas_res[ size ];

    for ( int i = 0; i < size; i++ ) {
      means_res[ i ]  = value*array.fMeans[ i ];
      sigmas_res[ i ] = value*array.fSigmas[ i ];
    }

    return Analysis::CLsArray( means_res, sigmas_res, size );
  }
  else {

    for ( int i = 0; i < size; i++ )
      means_res[ i ]  = value*array.fMeans[ i ];

    return Analysis::CLsArray( means_res, size );
  }
}

//_______________________________________________________________________________
// Defines the </> operator
Analysis::CLsArray Analysis::CLsArray::operator / ( double value ) {

  return ( *this )*1./value;
}

//_______________________________________________________________________________


// -- PROTECTED METHODS

//_______________________________________________________________________________
// Generates an array filled with random gaussian numbers following the
// distribution given by the means and sigmas owned by this class.
inline Analysis::CLsArray Analysis::CLsArray::GenerateGaussian() {

  double res[ fSize ];

  for ( int i = 0; i < fSize; i++ )
    res[ i ] = fGenerator.Gaus( fMeans[ i ], fSigmas[ i ] );

  return Analysis::CLsArray( res, fSize );;
}

//_______________________________________________________________________________
// Generates an array filled with random poissonian numbers following the
// distribution given by the means and sigmas owned by this class.
inline Analysis::CLsArray Analysis::CLsArray::GeneratePoisson() {

  double res[ fSize ];

  for ( int i = 0; i < fSize; i++ )
    res[ i ] = fGenerator.Poisson( fMeans[ i ] );
  
  return Analysis::CLsArray( res, fSize );
}

//_______________________________________________________________________________
// Gets the gaussian probability of having the given value(s)
inline double Analysis::CLsArray::GetGaussianProb( Analysis::CLsArray &values  ) {

  double res( 1 );

  for ( int i = 0; i < fSize; i++ )
    res *= GetGaussian( fMeans[ i ], fSigmas[ i ], values[ i ] );

  return res;
}

//_______________________________________________________________________________
// Gets the poissonian probability of having the given value(s)
inline double Analysis::CLsArray::GetPoissonProb( Analysis::CLsArray &values  ) {

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
Analysis::CLsAnalyser::CLsAnalyser() { }

//_______________________________________________________________________________
// Constructor that allocates the distribution of the two samples in two vectors
Analysis::CLsAnalyser::CLsAnalyser( Analysis::CLsArray old_hyp,
				    Analysis::CLsArray new_hyp,
				    int    npoints ) {

  fNewHyp  = new_hyp;
  fOldHyp  = old_hyp;
  fNPoints = npoints;

  if ( !strcmp( fNewHyp.fType, "Gaussian" ) )
    GetNewHypProb = &Analysis::CLsArray::GetGaussianProb;
  else
    GetNewHypProb = &Analysis::CLsArray::GetPoissonProb;
  if ( !strcmp( fOldHyp.fType, "Gaussian" ) )
    GetOldHypProb = &Analysis::CLsArray::GetGaussianProb;
  else
    GetOldHypProb = &Analysis::CLsArray::GetPoissonProb;
}

//_______________________________________________________________________________
// Destructor
Analysis::CLsAnalyser::~CLsAnalyser() { }

//_______________________________________________________________________________


// -- PUBLIC METHODS

//_______________________________________________________________________________
// Gets the distribution of the old and new hypothesis
void Analysis::CLsAnalyser::Evaluate() {
  
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
double Analysis::CLsAnalyser::GetAlpha( double t ) {

  return this -> GetPValue( fOldHypArray, t, "old" );
}

//_______________________________________________________________________________
// Gets the p-value of the old hypothesis given the observation
double Analysis::CLsAnalyser::GetAlpha( Analysis::CLsArray obs ) {

  return this -> GetAlpha( this -> TestStat( obs ) );
}

//_______________________________________________________________________________
// Gets the p-value of the new hypothesis given the test statistics value
double Analysis::CLsAnalyser::GetBeta( double t ) {

  return this -> GetPValue( fNewHypArray, t, "new" );
}

//_______________________________________________________________________________
// Gets the p-value of the new hypothesis given the observation
double Analysis::CLsAnalyser::GetBeta( Analysis::CLsArray obs ) {

  return this -> GetBeta( this -> TestStat( obs ) );
}

//_______________________________________________________________________________
// Gets the CLs
double Analysis::CLsAnalyser::GetCLs( Analysis::CLsArray obs ) {

  return this -> GetBeta( obs )/( 1 - this -> GetAlpha( obs ) );
}

//_______________________________________________________________________________
// Gets the new hypothesis test statistics event at position <index>
double Analysis::CLsAnalyser::GetNewHypEvt( int index ) {

  return fNewHypArray[ index ];
}

//_______________________________________________________________________________
// Gets the new hypothesis histogram
TH1D* Analysis::CLsAnalyser::GetNewHypHist( const char *name, int nbins ) {

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
double Analysis::CLsAnalyser::GetOldHypEvt( int index ) {

  return fOldHypArray[ index ];
}

//_______________________________________________________________________________
// Gets the old hypothesis histogram
TH1D* Analysis::CLsAnalyser::GetOldHypHist( const char *name, int nbins ) {

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
double Analysis::CLsAnalyser::GetPValue( std::vector<double> &list,
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
double Analysis::CLsAnalyser::GetQCLs( double q, const char *type ) {

  double tq;

  if ( !strcmp( type, "new" ) )
    tq = fNewHypArray[ int( q*fNPoints ) ];
  else
    tq = fOldHypArray[ int( q*fNPoints ) ];

  return this -> GetBeta( tq )/( 1 - this -> GetAlpha( tq ) );    
}

//_______________________________________________________________________________
// Gets the ROC curve
TGraph* Analysis::CLsAnalyser::GetROC( int npoints ) {

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
int Analysis::CLsAnalyser::GetSize() { return fNPoints; }

//_______________________________________________________________________________
// Sets a new hypothesis. The arrays containing the distributions of the old
// hypothesis are cleared.
void Analysis::CLsAnalyser::SetHypothesis( Analysis::CLsArray old_hyp,
					   Analysis::CLsArray new_hyp,
					   int    npoints ) {

  this -> SetNewHypothesis( new_hyp );
  this -> SetOldHypothesis( old_hyp );

  fNPoints = npoints;
}

//_______________________________________________________________________________
// Sets a new hypothesis. The array containing the distribution of the old new
// hypothesis is cleared.
void Analysis::CLsAnalyser::SetNewHypothesis( Analysis::CLsArray new_hyp ) {

  fNewHyp = new_hyp;
  fNewHypArray.clear();

  if ( !strcmp( fNewHyp.fType, "Gaussian" ) )
    GetNewHypProb = &Analysis::CLsArray::GetGaussianProb;
  else
    GetNewHypProb = &Analysis::CLsArray::GetPoissonProb;
}

//_______________________________________________________________________________
// Sets a new number of points for the distributions
void Analysis::CLsAnalyser::SetNPoints( int npoints ) { fNPoints = npoints; }

//_______________________________________________________________________________
// Sets a new hypothesis. The array containing the distribution of the old old
// hypothesis is cleared.
void Analysis::CLsAnalyser::SetOldHypothesis( Analysis::CLsArray old_hyp ) {

  fOldHyp = old_hyp;
  fOldHypArray.clear();

  if ( !strcmp( fOldHyp.fType, "Gaussian" ) )
    GetOldHypProb = &Analysis::CLsArray::GetGaussianProb;
  else
    GetOldHypProb = &Analysis::CLsArray::GetPoissonProb;
}

//_______________________________________________________________________________
// Gets the test statistics value for a given observation
double Analysis::CLsAnalyser::TestStat( Analysis::CLsArray obs ) {

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
inline double Analysis::GetGaussian( double mean,
				     double sigma,
				     double value ) {

  return 1./( std::sqrt( 2*M_PI )*sigma )*
    std::exp( - ( value - mean )*( value - mean )/( 2*sigma*sigma ) );
}

//_______________________________________________________________________________
// Gets the poissonian probability associated to this value
inline double Analysis::GetPoisson( double mean,
				    double value ) {

  return std::pow( mean, value )*std::exp( - mean )/std::tgamma( value + 1 );
}
