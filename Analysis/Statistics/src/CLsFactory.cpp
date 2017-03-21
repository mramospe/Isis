///////////////////////////////////////////////////////////////////////////////////
//
//  Analysis package
//
// --------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 21/03/2017
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#include "CLsFactory.h"
#include "CLsResult.h"
#include "Definitions.h"
#include "Messenger.h"
#include "MessengerConfig.h"
#include "Utils.h"

#include "TMath.h"

#include <algorithm>
#include <cmath>


//_______________________________________________________________________________

namespace Isis {

  //_______________________________________________________________________________
  //
  CLsFactory::CLsFactory() : fNullHyp( 0 ), fSigHyp( 0 ) { }

  //_______________________________________________________________________________
  //
  CLsFactory::CLsFactory( CLsHypothesis &h0,
			  CLsHypothesis &h1 ) {

    this->SetNullHyp(h0);
    
    this->SetSigHyp(h1);
  }

  //_______________________________________________________________________________
  //
  CLsFactory::~CLsFactory() { }

  //_______________________________________________________________________________
  //
  CLsResult CLsFactory::Calculate( const double &tstat ) const {

    double alpha = this->Alpha(tstat);
    double beta  = this->Beta(tstat);
    double CLb   = this->CLb(tstat);
    double CLs   = this->CLs(tstat);
    
    return CLsResult(CLs, CLb, alpha, beta, tstat);
  }

  //_______________________________________________________________________________
  //
  void CLsFactory::Generate( const size_t &n ) {

    if ( fNullHyp->GetHyp().size() != fSigHyp->GetHyp().size() )
      IError << "Input hypotheses do not have the same sizes" << IEndMsg;
    
    fNullHyp->Generate(n);

    fSigHyp->Generate(n);
  }

  //_______________________________________________________________________________
  //
  double CLsFactory::TestStat( const Doubles &values ) const {

    double t0 = fNullHyp->PoissonProb(values);

    double t1 = fSigHyp->PoissonProb(values);

    return -2.*std::log(t0/t1);
  }

}
