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


#ifndef CLS_ARRAY
#define CLS_ARRAY

#include "TRandom3.h"


//_______________________________________________________________________________

namespace Analysis {

  class CLsAnalyser;

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
    double     *fMeans;
    double     *fSigmas;
    int         fSize;
    char        fType;

    // Methods
    CLsArray  GenerateGaussian();
    CLsArray  GeneratePoisson();
    double    GetGaussianProb( const CLsArray &values );
    double    GetPoissonProb( const CLsArray &values );

  };

  // Other functions
  inline double GetGaussian( const double &mean, const double &sigma, const double &value );
  inline double GetPoisson( const double &mean, const double &value );

}

#endif
