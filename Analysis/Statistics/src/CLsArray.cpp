//////////////////////////////////////////////////////////////
//
//  Analysis package
//
// -----------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 02/12/2016
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
				 fType( 'P' ) { }

//_______________________________________________________________________________
// Copy constructor
Analysis::CLsArray::CLsArray( const Analysis::CLsArray &other ) :
  fGenerator( 0 ) {

  size_t size = other.GetSize();
  
  fType  = other.fType;
  
  fMeans = Analysis::CLsArray::DVector( size );
  for ( size_t i = 0; i < size; ++i )
    fMeans[ i ] = other.fMeans[ i ];
    
  if ( other.fSigmas.size() ) {
      
    fSigmas = Analysis::CLsArray::DVector( size );
    for ( size_t i = 0; i < size; ++i )
      fSigmas[ i ] = other.fSigmas[ i ];
  }
}

//_______________________________________________________________________________
// Constructor for single-values ( Poisson )
Analysis::CLsArray::CLsArray( const double &value ) :
  fGenerator( 0 ),
  fMeans( 1, value ),
  fSigmas(),
  fType( 'P' ) { }

//_______________________________________________________________________________
// Constructor for single-values ( Gaussian )
Analysis::CLsArray::CLsArray( const double &value, const double &sigma ) :
  fGenerator( 0 ),
  fMeans( 1, value ),
  fSigmas( 1, sigma ),
  fType( 'G' ) { }

//_______________________________________________________________________________
// Constructor given two lists
Analysis::CLsArray::CLsArray( const std::vector<double> &values,
			      const std::vector<double> &sigmas ) :
  fGenerator( 0 ),
  fMeans( values ),
  fSigmas( sigmas ),
  fType( 'P' ) {
  
  if ( sigmas.size() ) {
    if ( values.size() != sigmas.size() )
      std::cerr <<
	"ERROR: Vectors of values and standard deviations given to the array have different lengths"
		<< std::endl;
    fType   = 'G';
  }

  auto itv = values.begin(), its = sigmas.begin();
  for ( size_t i = 0; i < values.size(); ++i ) {
    fMeans[ i ] = *( itv + i );
    if ( fSigmas.size() )
      fSigmas[ i ] = *( its + i );
  }
}

//_______________________________________________________________________________
// Destructor
Analysis::CLsArray::~CLsArray() { }

//_______________________________________________________________________________


// -- PUBLIC METHODS

//_______________________________________________________________________________
// Defines the <=> operator
Analysis::CLsArray&
Analysis::CLsArray::operator = ( const Analysis::CLsArray &other ) {

  size_t size = other.GetSize();
  
  if ( other.fType )
    fType = other.fType;
  
  if ( other.fMeans.size() ) {
    fMeans = Analysis::CLsArray::DVector( size );
    for ( size_t i = 0; i < size; ++i )
      fMeans[ i ] = other.fMeans[ i ];
  }

  if ( other.fSigmas.size() ) {
    fSigmas = Analysis::CLsArray::DVector(  );
    for ( size_t i = 0; i < size; ++i )
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

  size_t size = other.GetSize();
  Analysis::CLsArray::DVector means_res( size );

  if ( fType == 'G' and other.fType == 'G' ) {
    Analysis::CLsArray::DVector sigmas_res( size );
    for ( size_t i = 0; i < size; ++i ) {
      means_res[ i ]  = fMeans[ i ] + other.fMeans[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( fSigmas[ i ]*fSigmas[ i ] +
					     other.fSigmas[ i ]*other.fSigmas[ i ] ) );
    }
    return Analysis::CLsArray( means_res, sigmas_res );
  }
  else if ( fType == 'G' and other.fType == 'P' ) {
    Analysis::CLsArray::DVector sigmas_res( size );
    for ( size_t i = 0; i < size; ++i ) {
      means_res[ i ]  = fMeans[ i ] + other.fMeans[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( fSigmas[ i ]*fSigmas[ i ]
					     + other.fMeans[ i ] ) );
    }
    return Analysis::CLsArray( means_res, sigmas_res );
  }
  else if ( fType == 'P' and other.fType == 'G' ) {
    Analysis::CLsArray::DVector sigmas_res( size );
    for ( size_t i = 0; i < size; ++i ) {
      means_res[ i ]  = fMeans[ i ] + other.fMeans[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( other.fMeans[ i ]*other.fMeans[ i ]
					     + fMeans[ i ] ) );
    }
    return Analysis::CLsArray( means_res, sigmas_res );
  }
  else {
    for ( size_t i = 0; i < size; ++i )
      means_res[ i ] = fMeans[ i ] + other.fMeans[ i ];
    return Analysis::CLsArray( means_res );
  }
}

//_______________________________________________________________________________
// Defines the <-> operator
Analysis::CLsArray
Analysis::CLsArray::operator - ( const Analysis::CLsArray &other ) {

  size_t size = other.GetSize();
  Analysis::CLsArray::DVector means_res( size );

  if ( fType == 'G' and other.fType == 'G' ) {
    Analysis::CLsArray::DVector sigmas_res( size );
    for ( size_t i = 0; i < size; ++i ) {
      means_res[ i ]  = fMeans[ i ] + other.fMeans[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( fSigmas[ i ]*fSigmas[ i ] -
					     other.fSigmas[ i ]*other.fSigmas[ i ] ) );
    }
    return Analysis::CLsArray( means_res, sigmas_res );
  }
  else if ( fType == 'G' and other.fType == 'P' ) {
    Analysis::CLsArray::DVector sigmas_res( size );
    for ( size_t i = 0; i < size; ++i ) {
      means_res[ i ]  = fMeans[ i ] + other.fMeans[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( fSigmas[ i ]*fSigmas[ i ]
					     - other.fMeans[ i ] ) );
    }
    return Analysis::CLsArray( means_res, sigmas_res );
  }
  else if ( fType == 'P' and other.fType == 'G' ) {
    Analysis::CLsArray::DVector sigmas_res( size );
    for ( size_t i = 0; i < size; ++i ) {
      means_res[ i ]  = fMeans[ i ] - other.fMeans[ i ];
      sigmas_res[ i ] = std::sqrt( std::abs( other.fMeans[ i ]*other.fMeans[ i ]
					     - fMeans[ i ] ) );
    }
    return Analysis::CLsArray( means_res, sigmas_res );
  }
  else {
    for ( size_t i = 0; i < size; ++i )
      means_res[ i ] = fMeans[ i ] - other.fMeans[ i ];
    return Analysis::CLsArray( means_res );
  }
}

//_______________________________________________________________________________
// Defines the <*> operator
Analysis::CLsArray Analysis::operator * ( const double &value, const CLsArray &other ) {

  size_t size = other.GetSize();
  Analysis::CLsArray::DVector means_res( size );

  if ( other.fType == 'G' ) {
    Analysis::CLsArray::DVector sigmas_res( size );
    for ( size_t i = 0; i < size; ++i ) {
      means_res[ i ]  = value*other.fMeans[ i ];
      sigmas_res[ i ] = value*other.fSigmas[ i ];
    }
    return Analysis::CLsArray( means_res, sigmas_res );
  }
  else {
    for ( size_t i = 0; i < size; ++i )
      means_res[ i ]  = value*other.fMeans[ i ];
    return Analysis::CLsArray( means_res );
  }
}

//_______________________________________________________________________________
// Defines the <*> operator
Analysis::CLsArray Analysis::operator * ( const CLsArray &other, const double &value ) {

  size_t size = other.GetSize();
  Analysis::CLsArray::DVector means_res( size );

  if ( other.fType == 'G' ) {
    Analysis::CLsArray::DVector sigmas_res( size );
    for ( size_t i = 0; i < size; ++i ) {
      means_res[ i ]  = value*other.fMeans[ i ];
      sigmas_res[ i ] = value*other.fSigmas[ i ];
    }
    return Analysis::CLsArray( means_res, sigmas_res );
  }
  else {
    for ( size_t i = 0; i < size; ++i )
      means_res[ i ]  = value*other.fMeans[ i ];
    return Analysis::CLsArray( means_res );
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
  size_t size = fMeans.size();
  std::vector<double> res( size );
  for ( size_t i = 0; i < size; ++i )
    res[ i ] = fGenerator.Gaus( fMeans[ i ], fSigmas[ i ] );
  return Analysis::CLsArray( res );
}

//_______________________________________________________________________________
// Generates an array filled with random poissonian numbers following the
// distribution given by the means and sigmas owned by this class.
Analysis::CLsArray Analysis::CLsArray::GeneratePoisson() {
  size_t size = fMeans.size();
  std::vector<double> res( size );
  for ( size_t i = 0; i < size; ++i )
    res[ i ] = fGenerator.Poisson( fMeans[ i ] );
  return Analysis::CLsArray( res );
}

//_______________________________________________________________________________
// Gets the gaussian probability of having the given value(s)
double Analysis::CLsArray::GetGaussianProb( const Analysis::CLsArray &values  ) {
  double res( 1 );
  for ( size_t i = 0; i < values.GetSize(); ++i )
    res *= GetGaussian( fMeans[ i ], fSigmas[ i ], values.fMeans[ i ] );

  return res;
}

//_______________________________________________________________________________
// Gets the poissonian probability of having the given value(s)
double Analysis::CLsArray::GetPoissonProb( const Analysis::CLsArray &values  ) {

  double res( 1 );

  for ( size_t i = 0; i < values.GetSize(); ++i )
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
