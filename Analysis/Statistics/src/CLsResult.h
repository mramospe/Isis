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
//  Class to store the output of a CLs computation
//
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////


#ifndef CLS_RESULT
#define CLS_RESULT

#include "Math/QuantFuncMathCore.h"

#include <cmath>


//_______________________________________________________________________________

namespace Analysis {

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
    double Alpha() const;

    // Return the p-value of the signal hypothesis
    double Beta() const;
    
    // Return the CLb
    double CLb() const;

    // Return the CLs
    double CLs() const;

    // Return the CLsb
    double CLsb() const;
    
    // Return the possition on a gaussian of mean = 0 and sigma = 1 for the result. If
    // it is greater than 0, it is equivalent to the significance.
    double GaussSigPos() const;

    // Return whether the result establishes a discovery or not in the two-sided
    // definition of the significance
    bool IsDiscovery() const;
    
    // Return whether the result establishes an evidence or not in the two-sided
    // definition of the significance
    bool IsEvidence() const;

    // Return the value for the test statistics
    double TestStat() const;
    
    // Return the significance
    double Significance() const;

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
  inline double CLsResult::Alpha() const {

    return fAlpha;
  }

  //_______________________________________________________________________________
  //
  inline double CLsResult::Beta() const {

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
  inline double CLsResult::GaussSigPos() const {
    
    return ROOT::Math::normal_quantile_c(fAlpha, 1);
  }

  //_______________________________________________________________________________
  //
  inline bool CLsResult::IsDiscovery() const {

    return fSig >= DiscoveryStdPVal;
  }

  //_______________________________________________________________________________
  //
  inline bool CLsResult::IsEvidence() const {

    return fSig >= EvidenceStdPVal;
  }

  //_______________________________________________________________________________
  //
  inline double CLsResult::TestStat() const {

    return fTestStat;
  }

  //_______________________________________________________________________________
  //
  inline double CLsResult::Significance() const {

    return fSig;
  }

}

#endif
