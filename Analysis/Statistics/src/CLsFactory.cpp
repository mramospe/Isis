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


#include "CLsFactory.h"
#include "CLsResult.h"
#include "Messenger.h"
#include "MessengerConfig.h"
#include "Utils.h"

#include "TMath.h"

#include <algorithm>
#include <cmath>


//_______________________________________________________________________________

namespace Analysis {

  //_______________________________________________________________________________
  //
  CLsFactory::CLsFactory() :
    fNullHyp( CLsHypoType::aNull, this ),
    fSigHyp( CLsHypoType::aSignal, this ) { }

  //_______________________________________________________________________________
  //
  CLsFactory::CLsFactory( const General::Doubles &h0,
			  const General::Doubles &h1 ) :
    fNullHyp( CLsHypoType::aNull, this, h0 ),
    fSigHyp( CLsHypoType::aSignal, this, h1 ) { }

  //_______________________________________________________________________________
  //
  CLsFactory::~CLsFactory() { }

  //_______________________________________________________________________________
  //
  CLsResult CLsFactory::Calculate( const General::Doubles &array ) const {

    double t = this->TestStat(array);
    
    double alpha = this->Alpha(t);
    double beta  = this->Beta(t);
    double CLb   = this->CLb(t);
    double CLs   = this->CLs(t);
    
    return CLsResult(CLs, CLb, alpha, beta, t);
  }

  //_______________________________________________________________________________
  //
  void CLsFactory::Generate( const size_t &n ) {

    if ( fNullHyp.GetHyp().size() != fSigHyp.GetHyp().size() )
      IError << "Input hypotheses do not have the same sizes" << IEndMsg;
    
    fNullHyp.Generate(n);
    fSigHyp.Generate(n);
  }

  //_______________________________________________________________________________
  //
  double CLsFactory::TestStat( const General::Doubles &values ) const {

    double t0 = fNullHyp.PoissonProb(values);
    double t1 = fSigHyp.PoissonProb(values);

    return -2.*std::log(t0/t1);
  }

}
