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


#ifndef CLS_FACTORY
#define CLS_FACTORY

#include "CLsHypothesis.h"
#include "CLsPrior.h"
#include "CLsResult.h"


//_______________________________________________________________________________

namespace Analysis {

  class CLsFactory {

  public:

    // Constructor
    CLsFactory();

    // Constructor given the two hypothesis
    CLsFactory( const General::Doubles &h0, const General::Doubles &h1 );

    // Destructor
    ~CLsFactory();
    
    // Return the p-value of the null hypothesis
    double Alpha( const double &t ) const;

    // Return the p-value of the signal hypothesis
    double Beta( const double &t ) const;

    // Return a class with all the information concerning the CLs method
    CLsResult Calculate( const General::Doubles &array ) const;

    // Calculate CLb
    double CLb( const double &t ) const;

    // Calculate CLs
    double CLs( const double &t ) const;

    // Calculate CLsb
    double CLsb( const double &t ) const;

    // Generate < n > events for each of the hypotheses
    void Generate( const size_t &n = 10000 );
    
    // Return the null hypothesis
    inline CLsHypothesis& GetNullHyp();
    
    // Return the signal hypothesis
    inline CLsHypothesis& GetSigHyp();

    // Return the test statistics for a given array of values
    double TestStat( const General::Doubles &values ) const;

  protected:

    // Null hypothesis
    CLsHypothesis fNullHyp;

    // Signal hypothesis
    CLsHypothesis fSigHyp;

  };
  
  //_______________________________________________________________________________
  //
  inline double CLsFactory::Alpha( const double &t ) const {
    
    return fNullHyp.PValue(t);
  }

  //_______________________________________________________________________________
  //
  inline double CLsFactory::Beta( const double &t ) const {

    return fSigHyp.PValue(t);
  }

  //_______________________________________________________________________________
  //
  inline double CLsFactory::CLb( const double &t ) const {

    return 1. - this->Alpha(t);
  }

  //_______________________________________________________________________________
  //
  inline double CLsFactory::CLs( const double &t ) const {
    
    return this->CLsb(t)/this->CLb(t);
  }

  //_______________________________________________________________________________
  //
  inline double CLsFactory::CLsb( const double &t ) const {

    return this->Beta(t);
  }

  //_______________________________________________________________________________
  //
  inline CLsHypothesis& CLsFactory::GetNullHyp() {

    return fNullHyp;
  }

  //_______________________________________________________________________________
  //
  inline CLsHypothesis& CLsFactory::GetSigHyp() {

    return fSigHyp;
  }

}

#endif
