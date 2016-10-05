//////////////////////////////////////////////////////////////
//
//  Analysis package
//
// -----------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 06/06/2016
//
// -----------------------------------------------------------
//
//  Description:
//
//  Definition of the Array class to perform CLs analysis.
//
// -----------------------------------------------------------
//////////////////////////////////////////////////////////////


#include "CLsArray.h"

#include <cmath>
#include <iostream>


//_______________________________________________________________________________


// -- CONSTRUCTORS AND DESTRUCTOR

//_______________________________________________________________________________
// Main constructor
Analysis::CLsArray::CLsArray() : fGenerator( 0 ),
				 fMeans( 0 ),
				 fSigmas( 0 ),
				 fSize( 0 ),
				 fType( 'P' ) { }

//_______________________________________________________________________________
// Copy constructor
Analysis::CLsArray::CLsArray( const Analysis::CLsArray &other ) :
  fGenerator( 0 ) {

  fType  = other.fType;
  fSize  = other.fSize;
  
  fMeans = new double[ fSize ];
  for ( int i = 0; i < fSize; ++i )
    fMeans[ i ] = other.fMeans[ i ];
    
  if ( other.fSigmas ) {
      
    fSigmas = new double[ fSize ];
    for ( int i = 0; i < fSize; ++i )
      fSigmas[ i ] = other.fSigmas[ i ];
  }
  else
    fSigmas = 0;
}

//_______________________________________________________________________________
// Constructor for single-values ( Poisson )
Analysis::CLsArray::CLsArray( const double &value ) :
  fGenerator( 0 ),
  fSigmas( 0 ),
  fType( 'P' ) {

  fSize       = 1;
  fMeans      = new double[ 1 ];
  fMeans[ 0 ] = value;
}

//_______________________________________________________________________________
// Constructor for single-values ( Gaussian )
Analysis::CLsArray::CLsArray( const double &value, const double &sigma ) :
  fGenerator( 0 ),
  fType( 'G' ) {

  fSize        = 1;
  fMeans       = new double[ 1 ];
  fSigmas      = new double[ 1 ];
  fMeans[ 0 ]  = value;  
  fSigmas[ 0 ] = sigma;
}

//_______________________________________________________________________________
// Constructor for multiple-values. If an array with the standard deviations is
// provided, the values are considered to be gaussian.
Analysis::CLsArray::CLsArray( const int &size, double *values, double *sigmas ) :
  fGenerator( 0 ),
  fSigmas( 0 ),
  fType( 'P' ) {

  fSize   = size;
  fMeans  = new double[ fSize ];
  
  if ( sigmas ) {
    fSigmas = new double[ fSize ];
    fType   = 'G';
  }

  for ( int i = 0; i < fSize; ++i ) {
    fMeans[ i ]  = values[ i ];
    if ( sigmas )
      fSigmas[ i ] = sigmas[ i ];
  }
}

//_______________________________________________________________________________
// Constructor given two lists
Analysis::CLsArray::CLsArray( const std::initializer_list<double> values,
			      const std::initializer_list<double> sigmas ) :
  fGenerator( 0 ),
  fSigmas( 0 ),
  fType( 'P' ) {

  fSize  = values.size();
  fMeans = new double[ fSize ];
  
  if ( sigmas.size() ) {
    if ( values.size() != sigmas.size() )
      std::cerr <<
	"ERROR: Vectors of values and standard deviations given to the array have different lengths"
		<< std::endl;
    fSigmas = new double[ fSize ];
    fType   = 'G';
  }

  auto itv = values.begin(), its = sigmas.begin();
  for ( int i = 0; i < fSize; ++i ) {
    fMeans[ i ] = *( itv + i );
    if ( fSigmas )
      fSigmas[ i ] = *( its + i );
  }
}

//_______________________________________________________________________________
// Destructor
Analysis::CLsArray::~CLsArray() {
  if ( fMeans )
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
    for ( int i = 0; i < fSize; ++i )
      fMeans[ i ] = other.fMeans[ i ];
  }

  if ( other.fSigmas ) {
    delete[] fSigmas;
    fSigmas = new double[ fSize ];
    for ( int i = 0; i < fSize; ++i )
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

  if ( fType == 'G' and other.fType == 'G' ) {
    double sigmas_res[ fSize ];
    for ( int i = 0; i < fSize; ++i ) {
      means_res[ i ]  = fMeans[ i ] + other.fMeans[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( fSigmas[ i ]*fSigmas[ i ] +
					     other.fSigmas[ i ]*other.fSigmas[ i ] ) );
    }
    return Analysis::CLsArray( fSize, means_res, sigmas_res );
  }
  else if ( fType == 'G' and other.fType == 'P' ) {
    double sigmas_res[ fSize ];
    for ( int i = 0; i < fSize; ++i ) {
      means_res[ i ]  = fMeans[ i ] + other.fMeans[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( fSigmas[ i ]*fSigmas[ i ]
					     + other.fMeans[ i ] ) );
    }
    return Analysis::CLsArray( fSize, means_res, sigmas_res );
  }
  else if ( fType == 'P' and other.fType == 'G' ) {
    double sigmas_res[ fSize ];
    for ( int i = 0; i < fSize; ++i ) {
      means_res[ i ]  = fMeans[ i ] + other.fMeans[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( other.fMeans[ i ]*other.fMeans[ i ]
					     + fMeans[ i ] ) );
    }
    return Analysis::CLsArray( fSize, means_res, sigmas_res );
  }
  else {
    for ( int i = 0; i < fSize; ++i )
      means_res[ i ] = fMeans[ i ] + other.fMeans[ i ];
    return Analysis::CLsArray( fSize, means_res );
  }
}

//_______________________________________________________________________________
// Defines the <-> operator
Analysis::CLsArray
Analysis::CLsArray::operator - ( const Analysis::CLsArray &other ) {

  double means_res[ fSize ];

  if ( fType == 'G' and other.fType == 'G' ) {
    double sigmas_res[ fSize ];
    for ( int i = 0; i < fSize; ++i ) {
      means_res[ i ]  = fMeans[ i ] + other.fMeans[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( fSigmas[ i ]*fSigmas[ i ] -
					     other.fSigmas[ i ]*other.fSigmas[ i ] ) );
    }
    return Analysis::CLsArray( fSize, means_res, sigmas_res );
  }
  else if ( fType == 'G' and other.fType == 'P' ) {
    double sigmas_res[ fSize ];
    for ( int i = 0; i < fSize; ++i ) {
      means_res[ i ]  = fMeans[ i ] + other.fMeans[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( fSigmas[ i ]*fSigmas[ i ]
					     - other.fMeans[ i ] ) );
    }
    return Analysis::CLsArray( fSize, means_res, sigmas_res );
  }
  else if ( fType == 'P' and other.fType == 'G' ) {
    double sigmas_res[ fSize ];
    for ( int i = 0; i < fSize; ++i ) {
      means_res[ i ]  = fMeans[ i ] - other.fMeans[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( other.fMeans[ i ]*other.fMeans[ i ]
					     - fMeans[ i ] ) );
    }
    return Analysis::CLsArray( fSize, means_res, sigmas_res );
  }
  else {
    for ( int i = 0; i < fSize; ++i )
      means_res[ i ] = fMeans[ i ] - other.fMeans[ i ];
    return Analysis::CLsArray( fSize, means_res );
  }
}

//_______________________________________________________________________________
// Defines the <*> operator
Analysis::CLsArray Analysis::operator * ( const double &value, const CLsArray &other ) {

  double means_res[ other.fSize ];

  if ( other.fType == 'G' ) {
    double sigmas_res[ other.fSize ];
    for ( int i = 0; i < other.fSize; ++i ) {
      means_res[ i ]  = value*other.fMeans[ i ];
      sigmas_res[ i ] = value*other.fSigmas[ i ];
    }
    return Analysis::CLsArray( other.fSize, means_res, sigmas_res );
  }
  else {
    for ( int i = 0; i < other.fSize; ++i )
      means_res[ i ]  = value*other.fMeans[ i ];
    return Analysis::CLsArray( other.fSize, means_res );
  }
}

//_______________________________________________________________________________
// Defines the <*> operator
Analysis::CLsArray Analysis::operator * ( const CLsArray &other, const double &value ) {

  int    size( other.fSize );
  double means_res[ size ];

  if ( other.fType == 'G' ) {
    double sigmas_res[ size ];
    for ( int i = 0; i < size; ++i ) {
      means_res[ i ]  = value*other.fMeans[ i ];
      sigmas_res[ i ] = value*other.fSigmas[ i ];
    }
    return Analysis::CLsArray( size, means_res, sigmas_res );
  }
  else {
    for ( int i = 0; i < size; ++i )
      means_res[ i ]  = value*other.fMeans[ i ];
    return Analysis::CLsArray( size, means_res );
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
Analysis::CLsArray Analysis::CLsArray::GenerateGaussian() {
  double res[ fSize ];
  for ( int i = 0; i < fSize; ++i )
    res[ i ] = fGenerator.Gaus( fMeans[ i ], fSigmas[ i ] );
  return Analysis::CLsArray( fSize, res );
}

//_______________________________________________________________________________
// Generates an array filled with random poissonian numbers following the
// distribution given by the means and sigmas owned by this class.
Analysis::CLsArray Analysis::CLsArray::GeneratePoisson() {
  double res[ fSize ];
  for ( int i = 0; i < fSize; ++i )
    res[ i ] = fGenerator.Poisson( fMeans[ i ] );
  return Analysis::CLsArray( fSize, res );
}

//_______________________________________________________________________________
// Gets the gaussian probability of having the given value(s)
double Analysis::CLsArray::GetGaussianProb( const Analysis::CLsArray &values  ) {
  double res( 1 );
  for ( int i = 0; i < fSize; ++i )
    res *= GetGaussian( fMeans[ i ], fSigmas[ i ], values.fMeans[ i ] );

  return res;
}

//_______________________________________________________________________________
// Gets the poissonian probability of having the given value(s)
double Analysis::CLsArray::GetPoissonProb( const Analysis::CLsArray &values  ) {

  double res( 1 );

  for ( int i = 0; i < fSize; ++i )
    res *= GetPoisson( fMeans[ i ], values.fMeans[ i ] );

  return res;
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
  double
    dist = value - mean,
    norm = std::sqrt( 2*M_PI )*sigma,
    s2   = 2*sigma*sigma;
  return std::exp( - dist*dist/s2 )/norm;
}

//_______________________________________________________________________________
// Gets the poissonian probability associated to this value
inline double Analysis::GetPoisson( const double &mean,
				    const double &value ) {
  return std::pow( mean, value )*std::exp( - mean )/std::tgamma( value + 1 );
}
