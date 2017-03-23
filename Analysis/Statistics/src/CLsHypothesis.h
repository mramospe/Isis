///////////////////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 23/03/2017
//
// --------------------------------------------------------------------------------
//
//  Description:
//
//  Main class to perform analysis using the CLs method
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#ifndef CLS_HYPOTHESIS
#define CLS_HYPOTHESIS

#include "CLsFluctuator.h"
#include "CLsPrior.h"
#include "Definitions.h"

#include "TRandom3.h"


//_______________________________________________________________________________

namespace Isis {

  // Enum to define the null and signal-type hypothesis
  enum CLsHypTypes { aNone, aNull, aSignal };

  // Define the factory class
  class CLsFactory;
  

  class CLsHypothesis {

  public:

    // Constructor given the type, the factory and the array. This class DOES NOT
    // take ownership of the prior.
    CLsHypothesis( const Doubles &array = {},
		   CLsFluctuator *fluct = 0,
		   CLsPrior *prior = 0 );

    // Destructor
    ~CLsHypothesis();

    // Return the vector containing the test-statistics values
    const Doubles& getTSVals() const;

    // Return the vector defining this hypothesis
    const Doubles& getHyp() const;

    // Return the p-value for the given test-statistics value
    double pValue( const double &t ) const;

    // Generate < n > events
    void generate( const size_t &n );

    // Return the poisson probability
    double poissonProb( const Doubles &values ) const;

    // Set the CLs factory class pointer
    inline void setFactory( const CLsFactory *factory );

    // Set the fluctuator class
    inline void setFluctuator( CLsFluctuator *fluct );

    // Set the vector defining this hypothesis
    inline void setHyp( const Doubles &array,
			CLsFluctuator *fluct = 0,
			CLsPrior *prior = 0 );

    // Set the prior
    inline void setPrior( CLsPrior *prior );

    // Set the type
    inline void setType( const int &type );

    // Get the test-statistics value associated to the given probability
    double testStatFromProb( const double &prob );
    
  protected:

    // Pointer to the factory class
    const CLsFactory *fFactory;

    // Class to fluctuate the means in the hypothesis
    CLsFluctuator *fFluct;
    
    // Vector defining the hypothesis
    Doubles fHyp;

    // Prior for the probability
    CLsPrior *fPrior;

    // Random number generator
    TRandom3 fRndm;

    // Vector with the values of the test-statistics
    Doubles fTSVals;

    // Type of the hypothesis
    int fType;
  };

  //_______________________________________________________________________________
  //
  inline const Doubles& CLsHypothesis::getHyp() const {

    return fHyp;
  }

  //_______________________________________________________________________________
  //
  inline const Doubles& CLsHypothesis::getTSVals() const {
    
    return fTSVals;
  }

  //_______________________________________________________________________________
  //
  inline void CLsHypothesis::setFactory( const CLsFactory *factory ) {

    fFactory = factory;
  }
  
  //_______________________________________________________________________________
  //
  inline void CLsHypothesis::setFluctuator( CLsFluctuator *fluct ) {
    fFluct = fluct;
  }

  //_______________________________________________________________________________
  //
  inline void CLsHypothesis::setHyp( const Doubles &array,
				     CLsFluctuator *fluct,
				     CLsPrior *prior ) {
    
    fTSVals.clear();
    fFluct = fluct;
    fHyp   = array;
    fPrior = prior;
  }

  //_______________________________________________________________________________
  //
  inline void CLsHypothesis::setPrior( CLsPrior *prior ) {

    fPrior = prior;
  }

  //_______________________________________________________________________________
  //
  inline void CLsHypothesis::setType( const int &type ) {

    fType = type;
  }
  
}

#endif
