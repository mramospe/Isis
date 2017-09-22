///////////////////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 10/04/2017
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#include "CLsFactory.hpp"
#include "CLsFluctuator.hpp"
#include "CLsHypothesis.hpp"
#include "CLsPrior.hpp"
#include "Definitions.hpp"
#include "Messenger.hpp"
#include "MessengerConfig.hpp"

#include "TMath.h"
#include "TRandom3.h"

#include <algorithm>


//_______________________________________________________________________________

namespace isis {

  //_______________________________________________________________________________
  //
  CLsHypothesis::CLsHypothesis( const Doubles &array,
				CLsFluctuator *fluct,
				CLsPrior *prior ) :
    fFactory( 0 ),
    fFluct( fluct ),
    fHyp( array ),
    fPrior( prior ),
    fRndm( 0 ),
    fType( CLsHypTypes::aNone ) { }

  //_______________________________________________________________________________
  //
  CLsHypothesis::~CLsHypothesis() { }

  //_______________________________________________________________________________
  //
  void CLsHypothesis::generate( const size_t &n ) {

    if ( fFactory ) {

      fTSVals.clear();
    
      for ( size_t i = 0; i < n; ++i ) {

	Doubles vec(fHyp);

	// Randomize vector
	for ( auto it = vec.begin(); it != vec.end(); ++it ) {

	  double mean = *it;
	  
	  // Fluctuate the values
	  if ( fFluct )
	    mean = fFluct->fluctuate(it - vec.begin(), mean);

	  *it = fRndm.Poisson(mean);
	}

	double tst = fFactory->testStat(vec);

	fTSVals.push_back(tst);
      }

      // It is very important to sort the vector
      std::sort(fTSVals.begin(), fTSVals.end());
      
    }
    else
      IError << "CLs factory is not set" << IEndMsg;
  }

  //_______________________________________________________________________________
  //
  double CLsHypothesis::poissonProb( const Doubles &values ) const {

    double prob = 1.;

    auto itv = values.cbegin();
    auto itt = fHyp.cbegin();
    
    // Calculate the probability from a poisson distribution
    while ( itt != fHyp.cend() ) {
      prob *= TMath::Poisson(*itv++, *itt++);

      // Include the prior
      if ( fPrior ) {

	size_t pos = itt - fHyp.cbegin();

	prob *= fPrior->evaluate(pos, *itt, *itv);
      }
    }
  
    return prob;
  }

  //_______________________________________________________________________________
  //
  double CLsHypothesis::pValue( const double &t ) const {
    
    double n;

    // Return the first element greater than < t >. This comes from the definition
    // of CLsb and CLb, which are probabilities with respect to measuring less or
    // equal events as the observed.
    Doubles::const_iterator it =
      std::upper_bound(fTSVals.cbegin(), fTSVals.cend(), t);

    switch ( fType ) {

    case CLsHypTypes::aNull:
      n = fTSVals.cend() - it;
      break;
      
    case CLsHypTypes::aSignal:
      n = it - fTSVals.cbegin();
      break;
      
    case CLsHypTypes::aNone:
      IError << "Hypothesis type is not set" << IEndMsg;
      return 0;
      
    default:
      IError << "Unknown hypothesis type < " << fType << " >" << IEndMsg;
      return 0;
    }

    return n/fTSVals.size();
  }

  //_______________________________________________________________________________
  //
  double CLsHypothesis::testStatFromProb( const double &prob ) {

    size_t np  = fTSVals.size();
    size_t pos = prob*np;

    if ( np == pos )
      return fTSVals.back();
    else {

      double val  = fTSVals.at(pos);
      double step = fTSVals.at(pos + 1) - val;
      
      return val + prob*step;
    }
  }

}
