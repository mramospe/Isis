///////////////////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 08/03/2017
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

namespace Analysis {

  // Enum to define the null and signal-type hypothesis
  enum CLsHypoType { aNull, aSignal };

  // Define the factory class
  class CLsFactory;
  

  class CLsHypothesis {

  public:

    // Constructor given the type and the pointer to the factory
    CLsHypothesis( const int &type, const CLsFactory *factory );

    // Constructor given the type, the factory and the array. This class DOES NOT
    // take ownership of the prior.
    CLsHypothesis( const int &type,
		   const CLsFactory *factory,
		   const General::Doubles &array,
		   CLsFluctuator *fluct = 0,
		   CLsPrior *prior = 0 );

    // Destructor
    ~CLsHypothesis();

    // Return the vector containing the test-statistics values
    const General::Doubles& GetTSVals() const;

    // Return the vector defining this hypothesis
    const General::Doubles& GetHyp() const;

    // Return the p-value for the given test-statistics value
    double PValue( const double &t ) const;

    // Generate < n > events
    void Generate( const size_t &n );

    // Return the poisson probability
    double PoissonProb( const General::Doubles &values ) const;

    // Set the fluctuator class
    inline void SetFluctuator( CLsFluctuator *fluct );

    // Set the vector defining this hypothesis
    inline void SetHypothesis( const General::Doubles &array,
			       CLsFluctuator *fluct = 0,
			       CLsPrior *prior = 0 );

    // Set the prior
    inline void SetPrior( CLsPrior *prior );

    // Get the test-statistics value associated to the given probability
    double TestFromProb( const double &prob );
    
  protected:

    // Pointer to the factory class
    const CLsFactory *fFactory;

    // Class to fluctuate the means in the hypothesis
    CLsFluctuator *fFluct;
    
    // Vector defining the hypothesis
    General::Doubles fHyp;

    // Prior for the probability
    Analysis::CLsPrior *fPrior;

    // Random number generator
    TRandom3 fRndm;

    // Vector with the values of the test-statistics
    General::Doubles fTSVals;

    // Type of the hypothesis
    int fType;
  };

  //_______________________________________________________________________________
  //
  inline void CLsHypothesis::SetFluctuator( CLsFluctuator *fluct ) {
    fFluct = fluct;
  }

  //_______________________________________________________________________________
  //
  inline void CLsHypothesis::SetHypothesis( const General::Doubles &array,
					    CLsFluctuator *fluct,
					    CLsPrior *prior ) {
    
    fTSVals.clear();
    fFluct = fluct;
    fHyp   = array;
    fPrior = prior;
  }

  //_______________________________________________________________________________
  //
  inline void CLsHypothesis::SetPrior( CLsPrior *prior ) {

    fPrior = prior;
  }

  //_______________________________________________________________________________
  //
  inline const General::Doubles& CLsHypothesis::GetHyp() const {

    return fHyp;
  }

  //_______________________________________________________________________________
  //
  inline const General::Doubles& CLsHypothesis::GetTSVals() const {
    
    return fTSVals;
  }
  
}

#endif
