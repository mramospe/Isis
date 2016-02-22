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


#include "CLsAnalysis.h"

#include <algorithm>
#include <cmath>
#include <cstring>


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
Analysis::CLsArray::CLsArray( const Analysis::CLsArray &other ) :
  fGenerator( 0 ) {

  fType  = other.fType;
  fSize  = other.fSize;
  
  fMeans = new double[ fSize ];
  for ( int i = 0; i < fSize; i++ )
    fMeans[ i ] = other.fMeans[ i ];
    
  if ( other.fSigmas ) {
      
    fSigmas = new double[ fSize ];
    for ( int i = 0; i < fSize; i++ )
      fSigmas[ i ] = other.fSigmas[ i ];
  }
  else
    fSigmas = 0;
}

//_______________________________________________________________________________
// Constructor for single-values ( Gaussian )
Analysis::CLsArray::CLsArray( const double &value, const double &sigma ) :
  fGenerator( 0 ),
  fType( "Gaussian" ) {

  fSize        = 1;
  fMeans       = new double[ 1 ];
  fSigmas      = new double[ 1 ];
  fMeans[ 0 ]  = value;  
  fSigmas[ 0 ] = sigma;
}

//_______________________________________________________________________________
// Constructor for multiple-values ( Gaussian )
Analysis::CLsArray::CLsArray( double    *values,
			      double    *sigmas,
			      const int &size ) :
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
Analysis::CLsArray::CLsArray( const double &value ) :
  fGenerator( 0 ),
  fType( "Poisson" ),
  fSigmas( 0 ) {

  fSize       = 1;
  fMeans      = new double[ 1 ];
  fMeans[ 0 ] = value;
}

//_______________________________________________________________________________
// Constructor for multiple-values ( Poisson )
Analysis::CLsArray::CLsArray( double *values, const int &size ) :
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
Analysis::CLsArray::operator = ( const Analysis::CLsArray &other ) {
  
  if ( other.fType )
    fType = other.fType;

  if ( other.fMeans ) {
    delete[] fMeans;
    fSize  = other.fSize;
    fMeans = new double[ fSize ];
    for ( int i = 0; i < fSize; i++ )
      fMeans[ i ] = other.fMeans[ i ];
  }

  if ( other.fSigmas ) {
    delete[] fSigmas;
    fSigmas = new double[ fSize ];
    for ( int i = 0; i < fSize; i++ )
      fSigmas[ i ] = other.fSigmas[ i ];
  }

  return *this;
}

//_______________________________________________________________________________
// Defines the <get_item> operator
double Analysis::CLsArray::operator [] ( const int &index ) { return fMeans[ index ]; }

//_______________________________________________________________________________
// Defines the <+> operator with CLsArray objects
Analysis::CLsArray
Analysis::CLsArray::operator + ( const Analysis::CLsArray &other ) {

  double means_res[ fSize ];

  if ( !strcmp( fType, "Gaussian" ) and !strcmp( other.fType, "Gaussian" ) ) {
    double sigmas_res[ fSize ];
    for ( int i = 0; i < fSize; i++ ) {
      means_res[ i ]  = fMeans[ i ] + other.fMeans[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( fSigmas[ i ]*fSigmas[ i ] +
					     other.fSigmas[ i ]*other.fSigmas[ i ] ) );
    }
    return Analysis::CLsArray( means_res, sigmas_res, fSize );
  }
  else if ( !strcmp( fType, "Gaussian" ) and !strcmp( other.fType, "Poisson" ) ) {
    double sigmas_res[ fSize ];
    for ( int i = 0; i < fSize; i++ ) {
      means_res[ i ]  = fMeans[ i ] + other.fMeans[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( fSigmas[ i ]*fSigmas[ i ]
					     + other.fMeans[ i ] ) );
    }
    return Analysis::CLsArray( means_res, sigmas_res, fSize );
  }
  else if ( !strcmp( fType, "Poisson" ) and !strcmp( other.fType, "Gaussian" ) ) {
    double sigmas_res[ fSize ];
    for ( int i = 0; i < fSize; i++ ) {
      means_res[ i ]  = fMeans[ i ] + other.fMeans[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( other.fMeans[ i ]*other.fMeans[ i ]
					     + fMeans[ i ] ) );
    }
    return Analysis::CLsArray( means_res, sigmas_res, fSize );
  }
  else {
    for ( int i = 0; i < fSize; i++ )
      means_res[ i ] = fMeans[ i ] + other.fMeans[ i ];
    return Analysis::CLsArray( means_res, fSize );
  }
}

//_______________________________________________________________________________
// Defines the <-> operator
Analysis::CLsArray
Analysis::CLsArray::operator - ( const Analysis::CLsArray &other ) {

  double means_res[ fSize ];

  if ( !strcmp( fType, "Gaussian" ) and !strcmp( other.fType, "Gaussian" ) ) {
    double sigmas_res[ fSize ];
    for ( int i = 0; i < fSize; i++ ) {
      means_res[ i ]  = fMeans[ i ] + other.fMeans[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( fSigmas[ i ]*fSigmas[ i ] -
					     other.fSigmas[ i ]*other.fSigmas[ i ] ) );
    }
    return Analysis::CLsArray( means_res, sigmas_res, fSize );
  }
  else if ( !strcmp( fType, "Gaussian" ) and !strcmp( other.fType, "Poisson" ) ) {
    double sigmas_res[ fSize ];
    for ( int i = 0; i < fSize; i++ ) {
      means_res[ i ]  = fMeans[ i ] + other.fMeans[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( fSigmas[ i ]*fSigmas[ i ]
					     - other.fMeans[ i ] ) );
    }
    return Analysis::CLsArray( means_res, sigmas_res, fSize );
  }
  else if ( !strcmp( fType, "Poisson" ) and !strcmp( other.fType, "Gaussian" ) ) {
    double sigmas_res[ fSize ];
    for ( int i = 0; i < fSize; i++ ) {
      means_res[ i ]  = fMeans[ i ] - other.fMeans[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( other.fMeans[ i ]*other.fMeans[ i ]
					     - fMeans[ i ] ) );
    }
    return Analysis::CLsArray( means_res, sigmas_res, fSize );
  }
  else {
    for ( int i = 0; i < fSize; i++ )
      means_res[ i ] = fMeans[ i ] - other.fMeans[ i ];
    return Analysis::CLsArray( means_res, fSize );
  }
}

//_______________________________________________________________________________
// Defines the <*> operator
Analysis::CLsArray Analysis::operator * ( const double &value, const CLsArray &other ) {

  double means_res[ other.fSize ];

  if ( !strcmp( other.fType, "Gaussian" ) ) {
    double sigmas_res[ other.fSize ];
    for ( int i = 0; i < other.fSize; i++ ) {
      means_res[ i ]  = value*other.fMeans[ i ];
      sigmas_res[ i ] = value*other.fSigmas[ i ];
    }
    return Analysis::CLsArray( means_res, sigmas_res, other.fSize );
  }
  else {
    for ( int i = 0; i < other.fSize; i++ )
      means_res[ i ]  = value*other.fMeans[ i ];
    return Analysis::CLsArray( means_res, other.fSize );
  }
}

//_______________________________________________________________________________
// Defines the <*> operator
Analysis::CLsArray Analysis::operator * ( const CLsArray &other, const double &value ) {

  int    size( other.fSize );
  double means_res[ size ];

  if ( !strcmp( other.fType, "Gaussian" ) ) {
    double sigmas_res[ size ];
    for ( int i = 0; i < size; i++ ) {
      means_res[ i ]  = value*other.fMeans[ i ];
      sigmas_res[ i ] = value*other.fSigmas[ i ];
    }
    return Analysis::CLsArray( means_res, sigmas_res, size );
  }
  else {
    for ( int i = 0; i < size; i++ )
      means_res[ i ]  = value*other.fMeans[ i ];
    return Analysis::CLsArray( means_res, size );
  }
}

//_______________________________________________________________________________
// Defines the </> operator
Analysis::CLsArray Analysis::CLsArray::operator / ( const double &value ) {
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
inline double Analysis::CLsArray::GetGaussianProb( const Analysis::CLsArray &values  ) {
  double res( 1 );
  for ( int i = 0; i < fSize; i++ )
    res *= GetGaussian( fMeans[ i ], fSigmas[ i ], values.fMeans[ i ] );

  return res;
}

//_______________________________________________________________________________
// Gets the poissonian probability of having the given value(s)
inline double Analysis::CLsArray::GetPoissonProb( const Analysis::CLsArray &values  ) {

  double res( 1 );

  for ( int i = 0; i < fSize; i++ )
    res *= GetPoisson( fMeans[ i ], values.fMeans[ i ] );

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
Analysis::CLsAnalyser::CLsAnalyser( const Analysis::CLsArray &old_hyp,
				    const Analysis::CLsArray &new_hyp,
				    const int                &npoints ) {

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
// Gets the new hypothesis histogram
TH1D* Analysis::CLsAnalyser::GetNewHypHist( const char *name, const int &nbins ) {
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
// Gets the old hypothesis histogram
TH1D* Analysis::CLsAnalyser::GetOldHypHist( const char *name, const int &nbins ) {
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
double Analysis::CLsAnalyser::GetPValue( const std::vector<double> &list,
					 const double              &t0,
					 const char                *type ) {
  int evts( 0 );
  std::vector<double>::iterator it;
  if ( !strcmp( type, "new" ) ) {
    for ( auto it = list.begin(); it != list.end(); it++ )
      if ( *it < t0 )
	evts++;
  }
  else {
    for ( auto it = list.begin(); it != list.end(); it++ )
      if ( *it > t0 )
	evts++;
  }
  return ( double ) evts/fNPoints;
}

//_______________________________________________________________________________
// Gets the CLs for a given confidence level ( introduce 0.8413 and 1 - 0.8413
// to get the region inside 1 sigma ).
double Analysis::CLsAnalyser::GetQCLs( const double &q, const char *type ) {
  double tq;
  if ( !strcmp( type, "new" ) )
    tq = fNewHypArray[ int( q*fNPoints ) ];
  else
    tq = fOldHypArray[ int( q*fNPoints ) ];
  return this -> GetBeta( tq )/( 1 - this -> GetAlpha( tq ) );    
}

//_______________________________________________________________________________
// Gets the ROC curve
TGraph* Analysis::CLsAnalyser::GetROC( const int &npoints ) {
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
// Sets a new hypothesis. The arrays containing the distributions of the old
// hypothesis are cleared.
void Analysis::CLsAnalyser::SetHypothesis( const Analysis::CLsArray &old_hyp,
					   const Analysis::CLsArray &new_hyp,
					   const int                &npoints ) {
  this -> SetNewHypothesis( new_hyp );
  this -> SetOldHypothesis( old_hyp );
  fNPoints = npoints;
}

//_______________________________________________________________________________
// Sets a new hypothesis. The array containing the distribution of the old new
// hypothesis is cleared.
void Analysis::CLsAnalyser::SetNewHypothesis( const Analysis::CLsArray &new_hyp ) {
  fNewHyp = new_hyp;
  fNewHypArray.clear();
  if ( !strcmp( fNewHyp.fType, "Gaussian" ) )
    GetNewHypProb = &Analysis::CLsArray::GetGaussianProb;
  else
    GetNewHypProb = &Analysis::CLsArray::GetPoissonProb;
}

//_______________________________________________________________________________
// Sets a new hypothesis. The array containing the distribution of the old old
// hypothesis is cleared.
void Analysis::CLsAnalyser::SetOldHypothesis( const Analysis::CLsArray &old_hyp ) {

  fOldHyp = old_hyp;
  fOldHypArray.clear();

  if ( !strcmp( fOldHyp.fType, "Gaussian" ) )
    GetOldHypProb = &Analysis::CLsArray::GetGaussianProb;
  else
    GetOldHypProb = &Analysis::CLsArray::GetPoissonProb;
}

//_______________________________________________________________________________



//_______________________________________________________________________________
/////////////////////////////
//     OTHER FUNCTIONS     //
/////////////////////////////


//_______________________________________________________________________________
// Gets the gaussian probability associated to this value
inline double Analysis::GetGaussian( const double &mean,
				     const double &sigma,
				     const double &value ) {
  return 1./( std::sqrt( 2*M_PI )*sigma )*
    std::exp( - ( value - mean )*( value - mean )/( 2*sigma*sigma ) );
}

//_______________________________________________________________________________
// Gets the poissonian probability associated to this value
inline double Analysis::GetPoisson( const double &mean,
				    const double &value ) {

  return std::pow( mean, value )*std::exp( - mean )/std::tgamma( value + 1 );
}
