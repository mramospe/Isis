//////////////////////////////////////////////////////////////
//
//  Analysis package
//
// -----------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 17/02/2017
//
// -----------------------------------------------------------
//
//  Description:
//
//  Definition of the Array class to perform CLs analysis.
//
// -----------------------------------------------------------
//////////////////////////////////////////////////////////////


#ifndef CLS_ARRAY
#define CLS_ARRAY

#include "TRandom3.h"


//_______________________________________________________________________________

namespace Analysis {

  class CLsAnalyser;

  class CLsArray {

    friend class CLsAnalyser;

    using DVector = std::vector<double>;
  
  public:
  
    // Main constructor
    CLsArray();

    // Copy constructor
    CLsArray( const CLsArray &other );

    // Constructor for single-values ( Poisson )
    CLsArray( const double &value );

    // Constructor for single-values ( Gaussian )
    CLsArray( const double &value, const double &sigma );

    //  Constructor given two lists
    CLsArray( const DVector &values, const DVector &sigmas = {} );

    // Destructor
    ~CLsArray();

    // Return the length of the array
    inline size_t GetSize() const;

    // Assignment from a given array
    CLsArray& operator =  ( const CLsArray &other );

    // Return the value at < index >
    double operator [] ( const int &index );

    // Add two arrays
    CLsArray operator +  ( const CLsArray &other );

    // Substract two arrays
    CLsArray operator -  ( const CLsArray &other );

    // Multiply scalar*array
    friend CLsArray operator *  ( const double &value, const CLsArray &other );

    // Multiply array*scalar
    friend CLsArray operator *  ( const CLsArray &other, const double &value );

    // Division by a scalar
    CLsArray operator /  ( const double &value );

  protected:
    
    // Random number generator
    TRandom3 fGenerator;

    // Vector storing the means for each bin
    DVector fMeans;

    // Vector storing the standard deviations for each bin
    DVector fSigmas;

    // Type of array (Gaussian or Poissonian)
    char fType;

  protected:
    
    // Generates an array filled with random gaussian numbers following the
    // distribution given by the means and sigmas owned by this class.
    CLsArray GenerateGaussian();

    // Generates an array filled with random poissonian numbers following the
    // distribution given by the means and sigmas owned by this class.
    CLsArray GeneratePoisson();

    // Gets the gaussian probability of having the given value(s)
    double GetGaussianProb( const CLsArray &values );

    // Gets the poissonian probability of having the given value(s)
    double GetPoissonProb( const CLsArray &values );

  };

  // Gets the gaussian probability associated to this value
  inline double GetGaussian( const double &mean, const double &sigma,
			     const double &value );
  
  // Gets the poissonian probability associated to this value
  inline double GetPoisson( const double &mean, const double &value );
  
  //_______________________________________________________________________________
  //
  inline size_t CLsArray::GetSize() const { return fMeans.size(); }

}

#endif
