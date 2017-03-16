//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 16/03/2017
//
// -------------------------------------------------------------------------------
//
//  Description:
//
//  Wrappers from the Statistics library
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#include <boost/python.hpp>
#include <boost/python/list.hpp>
#include <boost/shared_ptr.hpp>

#include "GlobalWrappers.h"

#include "CLsFactory.h"
#include "CLsFluctuator.h"
#include "CLsHypothesis.h"
#include "CLsPrior.h"
#include "CLsResult.h"

#include "TPython.h"

namespace py = boost::python;
namespace an = Analysis;


//_______________________________________________________________________________

// Wrappers for the class CLsFactory
namespace CLsFact {
  
  //_______________________________________________________________________________
  //
  an::CLsResult CalculateFromArray( const an::CLsFactory &factory, py::list array ) {

    auto vector = IBoost::BoostListToStdVec<double>( array );

    return factory.Calculate( vector );
  }

  //_______________________________________________________________________________
  //
  an::CLsResult (an::CLsFactory::*CalculateFromDouble)( const double &tstat ) const =
    &an::CLsFactory::Calculate;

  //_______________________________________________________________________________
  //
  an::CLsHypothesis GetNullHyp( an::CLsFactory &factory ) {

    return *(factory.GetNullHyp());
  }

  //_______________________________________________________________________________
  //
  an::CLsHypothesis GetSigHyp( an::CLsFactory &factory ) {

    return *(factory.GetSigHyp());
  }

  //_______________________________________________________________________________
  //
  double TestStat( const an::CLsFactory &factory, py::list values ) {

    auto vector = IBoost::BoostListToStdVec<double>( values );

    return factory.TestStat( vector );
  }
}

// Wrappers for the class CLsFluctuator
namespace CLsFluct {

  struct CLsFluctWrap : an::CLsFluctuator, py::wrapper<an::CLsFluctuator> {
    
    double Fluctuate( const size_t &pos, const double &value ) {

      return this->get_override("Fluctuate")(pos, value);
    }
  };
}

// Wrappers for the class CLsHypothesis
namespace CLsHyp {

  //_______________________________________________________________________________
  //
  boost::shared_ptr<an::CLsHypothesis>
  Constructor( py::list lst,
	       an::CLsFluctuator *fluct,
	       an::CLsPrior *prior ) {

    auto array = IBoost::BoostListToStdVec<double>( lst );
    auto hyp   = new an::CLsHypothesis(array, fluct, prior);

    return boost::shared_ptr<an::CLsHypothesis>( hyp );
  }

  //_______________________________________________________________________________
  //
  boost::shared_ptr<an::CLsHypothesis>
  Constructor_NoPrior( py::list lst,
		       an::CLsFluctuator *fluct ) {

    auto array = IBoost::BoostListToStdVec<double>( lst );
    auto hyp   = new an::CLsHypothesis(array, fluct);

    return boost::shared_ptr<an::CLsHypothesis>( hyp );
  }

  //_______________________________________________________________________________
  //
  boost::shared_ptr<an::CLsHypothesis>
  Constructor_NoFluctNoPrior( py::list lst ) {

    auto array = IBoost::BoostListToStdVec<double>( lst );
    auto hyp   = new an::CLsHypothesis(array);

    return boost::shared_ptr<an::CLsHypothesis>( hyp );
  }

  //_______________________________________________________________________________
  //
  py::list GetHyp( const an::CLsHypothesis &hyp ) {

    auto vec = hyp.GetHyp();

    return IBoost::StdVecToBoostList( vec );
  }

  //_______________________________________________________________________________
  //
  double PoissonProb( const an::CLsHypothesis &hyp, py::list values ) {

    auto vec = IBoost::BoostListToStdVec<double>( values );

    return hyp.PoissonProb( vec );
  }

  //_______________________________________________________________________________
  //
  py::list GetTSVals( const an::CLsHypothesis &hyp ) {

    auto vec = hyp.GetTSVals();

    return IBoost::StdVecToBoostList( vec );
  }

  //_______________________________________________________________________________
  //
  void SetHyp( an::CLsHypothesis &hyp,
	       py::list lst,
	       an::CLsFluctuator *fluct = 0,
	       an::CLsPrior *prior = 0 ) {

    auto vec = IBoost::BoostListToStdVec<double>( lst );

    hyp.SetHyp(vec, fluct, prior);
  }

  BOOST_PYTHON_FUNCTION_OVERLOADS(SetHyp_Overloads, SetHyp, 2, 4);
}

// Wrappers for the class CLsPrior
namespace CLsPrior {

  struct CLsPriorWrap : an::CLsPrior, py::wrapper<an::CLsPrior> {

    double Evaluate( const size_t &pos, const double &mean, const double &value ) {

      return this->get_override("Evaluate")();
    }
  };
}
