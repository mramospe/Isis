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
///////////////////////////////////////////////////////////////////////////////////


#include "Definitions.h"
#include "Messenger.h"
#include "MessengerConfig.h"

#include "CLsFactory.h"
#include "CLsFluctuator.h"
#include "CLsHypothesis.h"
#include "CLsPrior.h"

#include "TMath.h"
#include "TRandom3.h"

#include <algorithm>


//_______________________________________________________________________________

namespace Analysis {

  //_______________________________________________________________________________
  //
  CLsHypothesis::CLsHypothesis( const int &type, const CLsFactory *factory ) :
    fFactory( factory ),
    fFluct( 0 ),
    fPrior( 0 ),
    fRndm( 0 ) { }

  //_______________________________________________________________________________
  //
  CLsHypothesis::CLsHypothesis( const int &type,
				const CLsFactory *factory,
				const General::Doubles &array,
				CLsFluctuator *fluct,
				CLsPrior *prior ) :
    fFactory( factory ),
    fFluct( fluct ),
    fHyp( array ),
    fPrior( prior ),
    fRndm( 0 ),
    fType( type ) { }

  //_______________________________________________________________________________
  //
  CLsHypothesis::~CLsHypothesis() { }

  //_______________________________________________________________________________
  //
  void CLsHypothesis::Generate( const size_t &n ) {

    size_t new_size = std::abs(n - fTSVals.size());
  
    bool change = (fTSVals.empty() || new_size);

    switch ( change ) {

    case true:

      for ( size_t i = fTSVals.size(); i < new_size; ++i ) {

	General::Doubles vec(fHyp);

	// Randomize vector
	for ( auto it = vec.begin(); it != vec.end(); ++it ) {

	  double mean = *it;
	  
	  // Fluctuate the values
	  if ( fFluct )
	    mean = fFluct->Fluctuate(it - vec.begin(), mean);

	  *it = fRndm.Poisson(mean);
	}
      
	double tst = fFactory->TestStat(vec);

	fTSVals.push_back(tst);
      }

      std::sort(fTSVals.begin(), fTSVals.end());
      
      break;
    
    default:
      break;
    }
  }

  //_______________________________________________________________________________
  //
  double CLsHypothesis::PoissonProb( const General::Doubles &values ) const {

    double prob = 1.;

    auto itv = values.cbegin();
    auto itt = fHyp.cbegin();
    
    // Calculate the probability from a poisson distribution
    while ( itt != fHyp.cend() ) {
      prob *= TMath::Poisson(*itv++, *itt++);

      // Include the prior
      if ( fPrior ) {

	size_t pos = itt - fHyp.cbegin();

	prob *= fPrior->Evaluate(pos, *itt, *itv);
      }
    }
  
    return prob;
  }

  //_______________________________________________________________________________
  //
  double CLsHypothesis::PValue( const double &t ) const {
  
    General::Doubles::const_iterator it =
      std::lower_bound(fTSVals.cbegin(), fTSVals.cend(), t);
  
    double n;
  
    switch ( fType ) {

    case CLsHypoType::aNull:
      n = fTSVals.cend() - it;
      break;
    case CLsHypoType::aSignal:
      n = it - fTSVals.cbegin();
      break;
    default:
      IError << "Unknown hypothesis type" << IEndMsg;
      return 0;
    }

    return n/fTSVals.size();
  }

  //_______________________________________________________________________________
  //
  double CLsHypothesis::TestFromProb( const double &prob ) {

    size_t np  = fTSVals.size();
    size_t pos = prob*np;

    if ( np == pos )
      return fTSVals.back();
    else
      return (fTSVals.at(pos) + fTSVals.at(pos + 1))/2.;
  }

}
