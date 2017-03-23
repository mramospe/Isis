//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 21/03/2017
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


//_______________________________________________________________________________

// Wrappers for the class CLsFactory
namespace CLsFact {
  
  //_______________________________________________________________________________
  //
  Isis::CLsResult CalculateFromArray( const Isis::CLsFactory &factory, py::list array ) {

    auto vector = IBoost::boostListToStdVec<double>( array );

    return factory.Calculate( vector );
  }

  //_______________________________________________________________________________
  //
  Isis::CLsResult (Isis::CLsFactory::*CalculateFromDouble)( const double &tstat ) const =
    &Isis::CLsFactory::Calculate;

  //_______________________________________________________________________________
  //
  Isis::CLsHypothesis GetNullHyp( Isis::CLsFactory &factory ) {

    return *(factory.GetNullHyp());
  }

  //_______________________________________________________________________________
  //
  Isis::CLsHypothesis GetSigHyp( Isis::CLsFactory &factory ) {

    return *(factory.GetSigHyp());
  }

  //_______________________________________________________________________________
  //
  double TestStat( const Isis::CLsFactory &factory, py::list values ) {

    auto vector = IBoost::boostListToStdVec<double>( values );

    return factory.TestStat( vector );
  }
}

// Wrappers for the class CLsFluctuator
namespace CLsFluct {

  struct CLsFluctWrap : Isis::CLsFluctuator, py::wrapper<Isis::CLsFluctuator> {
    
    double Fluctuate( const size_t &pos, const double &value ) {

      return this->get_override("Fluctuate")(pos, value);
    }
  };
}

// Wrappers for the class CLsHypothesis
namespace CLsHyp {

  //_______________________________________________________________________________
  //
  boost::shared_ptr<Isis::CLsHypothesis>
  Constructor( py::list lst,
	       Isis::CLsFluctuator *fluct,
	       Isis::CLsPrior *prior ) {

    auto array = IBoost::boostListToStdVec<double>( lst );
    auto hyp   = new Isis::CLsHypothesis(array, fluct, prior);

    return boost::shared_ptr<Isis::CLsHypothesis>( hyp );
  }

  //_______________________________________________________________________________
  //
  boost::shared_ptr<Isis::CLsHypothesis>
  Constructor_NoPrior( py::list lst,
		       Isis::CLsFluctuator *fluct ) {

    auto array = IBoost::boostListToStdVec<double>( lst );
    auto hyp   = new Isis::CLsHypothesis(array, fluct);

    return boost::shared_ptr<Isis::CLsHypothesis>( hyp );
  }

  //_______________________________________________________________________________
  //
  boost::shared_ptr<Isis::CLsHypothesis>
  Constructor_NoFluctNoPrior( py::list lst ) {

    auto array = IBoost::boostListToStdVec<double>( lst );
    auto hyp   = new Isis::CLsHypothesis(array);

    return boost::shared_ptr<Isis::CLsHypothesis>( hyp );
  }

  //_______________________________________________________________________________
  //
  py::list GetHyp( const Isis::CLsHypothesis &hyp ) {

    auto vec = hyp.GetHyp();

    return IBoost::stdVecToBoostList( vec );
  }

  //_______________________________________________________________________________
  //
  double PoissonProb( const Isis::CLsHypothesis &hyp, py::list values ) {

    auto vec = IBoost::boostListToStdVec<double>( values );

    return hyp.PoissonProb( vec );
  }

  //_______________________________________________________________________________
  //
  py::list GetTSVals( const Isis::CLsHypothesis &hyp ) {

    auto vec = hyp.GetTSVals();

    return IBoost::stdVecToBoostList( vec );
  }

  //_______________________________________________________________________________
  //
  void SetHyp( Isis::CLsHypothesis &hyp,
	       py::list lst,
	       Isis::CLsFluctuator *fluct = 0,
	       Isis::CLsPrior *prior = 0 ) {

    auto vec = IBoost::boostListToStdVec<double>( lst );

    hyp.SetHyp(vec, fluct, prior);
  }

  BOOST_PYTHON_FUNCTION_OVERLOADS(SetHyp_Overloads, SetHyp, 2, 4);
}

// Wrappers for the class CLsPrior
namespace CLsPrior {

  struct CLsPriorWrap : Isis::CLsPrior, py::wrapper<Isis::CLsPrior> {

    double Evaluate( const size_t &pos, const double &mean, const double &value ) {

      return this->get_override("Evaluate")(pos, mean, value);
    }
  };
}
