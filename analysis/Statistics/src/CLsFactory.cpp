///////////////////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 19/10/2017
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#include "CLsFactory.hpp"
#include "CLsResult.hpp"
#include "Definitions.hpp"
#include "Exceptions.hpp"
#include "Utils.hpp"

#include "TMath.h"

#include <algorithm>
#include <cmath>


//_______________________________________________________________________________

namespace isis {

  //_______________________________________________________________________________
  //
  CLsFactory::CLsFactory() : fNullHyp( 0 ), fSigHyp( 0 ) { }

  //_______________________________________________________________________________
  //
  CLsFactory::CLsFactory( CLsHypothesis &h0,
			  CLsHypothesis &h1 ) {

    this->setNullHyp(h0);
    
    this->setSigHyp(h1);
  }

  //_______________________________________________________________________________
  //
  CLsFactory::~CLsFactory() { }

  //_______________________________________________________________________________
  //
  CLsResult CLsFactory::calculate( const double &tstat ) const {

    double alpha = this->alpha(tstat);
    double beta  = this->beta(tstat);
    double CLb   = this->CLb(tstat);
    double CLs   = this->CLs(tstat);
    
    return CLsResult(CLs, CLb, alpha, beta, tstat);
  }

  //_______________________________________________________________________________
  //
  void CLsFactory::generate( const size_t &n ) {

    if ( fNullHyp->getHyp().size() != fSigHyp->getHyp().size() )
      throw BaseException("Input hypotheses do not have the same sizes");
    
    fNullHyp->generate(n);

    fSigHyp->generate(n);
  }

  //_______________________________________________________________________________
  //
  double CLsFactory::testStat( const Doubles &values ) const {

    double t0 = fNullHyp->poissonProb(values);

    double t1 = fSigHyp->poissonProb(values);

    return -2.*std::log(t0/t1);
  }

}
