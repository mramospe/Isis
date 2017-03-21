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


#include "CLsResult.h"

#include "Math/ProbFuncMathCore.h"

#include <cmath>


//_______________________________________________________________________________

namespace Isis {

  //_______________________________________________________________________________
  // Set to 3 sigma (mean = 0, sigma = 1)
  double EvidenceStdPVal = ROOT::Math::normal_cdf(3, 1, 0);
  
  //_______________________________________________________________________________
  // Set to 5 sigma (mean = 0, sigma = 1)
  double DiscoveryStdPVal = ROOT::Math::normal_cdf(5, 1, 0);

  //_______________________________________________________________________________
  //
  CLsResult::CLsResult( const double &cls,
			const double &clb,
			const double &alpha,
			const double &beta,
			const double &tstat ) :
    fAlpha( alpha ),
    fBeta( beta ),
    fCLb( clb ),
    fCLs( cls ),
    fSig( std::sqrt(tstat) ),
    fTestStat( tstat ) { }

  //_______________________________________________________________________________
  //
  CLsResult::~CLsResult() { }
  
}
