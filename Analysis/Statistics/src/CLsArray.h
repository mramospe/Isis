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
  
    // Constructor
    CLsArray();
    CLsArray( const CLsArray &other );
    CLsArray( const double &value );
    CLsArray( const double &value, const double &sigma );
    CLsArray( const DVector &values, const DVector &sigmas = {} );

    // Destructor
    ~CLsArray();

    // Inline methods
    inline size_t GetSize() const;

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
    TRandom3 fGenerator;
    DVector  fMeans;
    DVector  fSigmas;
    char     fType;

    // Methods
    CLsArray  GenerateGaussian();
    CLsArray  GeneratePoisson();
    double    GetGaussianProb( const CLsArray &values );
    double    GetPoissonProb( const CLsArray &values );

  };

  //_______________
  // INLINE METHODS
  inline size_t CLsArray::GetSize() const { return fMeans.size(); }
  
  // Other functions
  inline double GetGaussian( const double &mean, const double &sigma, const double &value );
  inline double GetPoisson( const double &mean, const double &value );

}

#endif
