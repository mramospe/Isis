//////////////////////////////////////////////////////////////
//                                                          //
//  Analysis package                                        //
//                                                          //
// -------------------------------------------------------- //
//                                                          //
//  AUTHOR: Miguel Ramos Pernas                             //
//  e-mail: miguel.ramos.pernas@cern.ch                     //
//                                                          //
//  Last update: 06/06/2016                                 //
//                                                          //
// -------------------------------------------------------- //
//                                                          //
//  Description:                                            //
//                                                          //
//  Definition of the Array class to perform CLs analysis.  //           
//                                                          //
// -------------------------------------------------------- //
//////////////////////////////////////////////////////////////


#include "CLsArray.h"

#include <cmath>


//_______________________________________________________________________________


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
Analysis::CLsArray Analysis::CLsArray::GenerateGaussian() {
  double res[ fSize ];
  for ( int i = 0; i < fSize; i++ )
    res[ i ] = fGenerator.Gaus( fMeans[ i ], fSigmas[ i ] );
  return Analysis::CLsArray( res, fSize );;
}

//_______________________________________________________________________________
// Generates an array filled with random poissonian numbers following the
// distribution given by the means and sigmas owned by this class.
Analysis::CLsArray Analysis::CLsArray::GeneratePoisson() {
  double res[ fSize ];
  for ( int i = 0; i < fSize; i++ )
    res[ i ] = fGenerator.Poisson( fMeans[ i ] );
  return Analysis::CLsArray( res, fSize );
}

//_______________________________________________________________________________
// Gets the gaussian probability of having the given value(s)
double Analysis::CLsArray::GetGaussianProb( const Analysis::CLsArray &values  ) {
  double res( 1 );
  for ( int i = 0; i < fSize; i++ )
    res *= GetGaussian( fMeans[ i ], fSigmas[ i ], values.fMeans[ i ] );

  return res;
}

//_______________________________________________________________________________
// Gets the poissonian probability of having the given value(s)
double Analysis::CLsArray::GetPoissonProb( const Analysis::CLsArray &values  ) {

  double res( 1 );

  for ( int i = 0; i < fSize; i++ )
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
  return 1./( std::sqrt( 2*M_PI )*sigma )*
    std::exp( - ( value - mean )*( value - mean )/( 2*sigma*sigma ) );
}

//_______________________________________________________________________________
// Gets the poissonian probability associated to this value
inline double Analysis::GetPoisson( const double &mean,
				    const double &value ) {
  return std::pow( mean, value )*std::exp( - mean )/std::tgamma( value + 1 );
}
