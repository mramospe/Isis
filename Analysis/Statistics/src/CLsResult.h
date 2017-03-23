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
//  Class to store the output of a CLs computation
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#ifndef CLS_RESULT
#define CLS_RESULT

#include "Math/QuantFuncMathCore.h"

#include <cmath>


//_______________________________________________________________________________

namespace Isis {

  // Global variable to control the p-value required to have an evidence
  // default = (3*sigma) in the two-sided definition of the significance
  extern double EvidenceStdPVal;

  // Global variable to control the p-value required to have a discovery
  // default = (5*sigma) in the two-sided definition of the significance
  extern double DiscoveryStdPVal;

  
  class CLsResult {

  public:

    // Constructor given the results for a CLs calculation
    CLsResult( const double &cls,
	       const double &clb,
	       const double &alpha,
	       const double &beta,
	       const double &tstat );

    // Destructor
    ~CLsResult();

    // Return the p-value of the null hypothesis
    double alpha() const;

    // Return the p-value of the signal hypothesis
    double beta() const;
    
    // Return the CLb
    double CLb() const;

    // Return the CLs
    double CLs() const;

    // Return the CLsb
    double CLsb() const;
    
    // Return the possition on a gaussian of mean = 0 and sigma = 1 for the result. If
    // it is greater than 0, it is equivalent to the significance.
    double gaussSigPos() const;

    // Return whether the result establishes a discovery or not in the two-sided
    // definition of the significance
    bool isDiscovery() const;
    
    // Return whether the result establishes an evidence or not in the two-sided
    // definition of the significance
    bool isEvidence() const;

    // Return the value for the test statistics
    double testStat() const;
    
    // Return the significance
    double significance() const;

  protected:

    // p-value of the null hypothesis
    const double fAlpha;

    // p-value of the signal hypothesis
    const double fBeta;

    // 1 - Alpha
    const double fCLb;

    // Beta/(1 - Alpha)
    const double fCLs;

    // Significance
    const double fSig;

    // Test statistics
    const double fTestStat;
  };
  
  
  //_______________________________________________________________________________
  //
  inline double CLsResult::alpha() const {

    return fAlpha;
  }

  //_______________________________________________________________________________
  //
  inline double CLsResult::beta() const {

    return fBeta;
  }

  //_______________________________________________________________________________
  //
  inline double CLsResult::CLb() const {

    return fCLb;
  }

  //_______________________________________________________________________________
  //
  inline double CLsResult::CLs() const {

    return fCLs;
  }

  //_______________________________________________________________________________
  //
  inline double CLsResult::CLsb() const {

    return fBeta;
  }

  //_______________________________________________________________________________
  //
  inline double CLsResult::gaussSigPos() const {
    
    return ROOT::Math::normal_quantile_c(fAlpha, 1);
  }

  //_______________________________________________________________________________
  //
  inline bool CLsResult::isDiscovery() const {

    return fSig >= DiscoveryStdPVal;
  }

  //_______________________________________________________________________________
  //
  inline bool CLsResult::isEvidence() const {

    return fSig >= EvidenceStdPVal;
  }

  //_______________________________________________________________________________
  //
  inline double CLsResult::testStat() const {

    return fTestStat;
  }

  //_______________________________________________________________________________
  //
  inline double CLsResult::significance() const {

    return fSig;
  }

}

#endif
