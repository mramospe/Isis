//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 24/03/2017
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
  inline Isis::CLsResult calculateFromArray( const Isis::CLsFactory &factory,
					     py::list array ) {

    auto vector = IBoost::boostListToStdVec<double>( array );

    return factory.calculate( vector );
  }

  //_______________________________________________________________________________
  //
  Isis::CLsResult
  (Isis::CLsFactory::*calculateFromDouble)( const double &tstat ) const =
    &Isis::CLsFactory::calculate;

  //_______________________________________________________________________________
  //
  inline Isis::CLsHypothesis getNullHyp( Isis::CLsFactory &factory ) {

    return *(factory.getNullHyp());
  }

  //_______________________________________________________________________________
  //
  inline Isis::CLsHypothesis getSigHyp( Isis::CLsFactory &factory ) {

    return *(factory.getSigHyp());
  }

  //_______________________________________________________________________________
  //
  inline double testStat( const Isis::CLsFactory &factory, py::list values ) {

    auto vector = IBoost::boostListToStdVec<double>( values );

    return factory.testStat( vector );
  }
}

// Wrappers for the class CLsFluctuator
namespace CLsFluct {

  struct CLsFluctWrap : Isis::CLsFluctuator, py::wrapper<Isis::CLsFluctuator> {
    
    double fluctuate( const size_t &pos, const double &value ) {

      return this->get_override("fluctuate")(pos, value);
    }
  };
}

// Wrappers for the class CLsHypothesis
namespace CLsHyp {

  //_______________________________________________________________________________
  //
  inline boost::shared_ptr<Isis::CLsHypothesis>
  constructor( py::list lst,
	       Isis::CLsFluctuator *fluct,
	       Isis::CLsPrior *prior ) {

    auto array = IBoost::boostListToStdVec<double>( lst );
    auto hyp   = new Isis::CLsHypothesis(array, fluct, prior);

    return boost::shared_ptr<Isis::CLsHypothesis>( hyp );
  }

  //_______________________________________________________________________________
  //
  inline boost::shared_ptr<Isis::CLsHypothesis>
  constructor_NoPrior( py::list lst,
		       Isis::CLsFluctuator *fluct ) {

    auto array = IBoost::boostListToStdVec<double>( lst );
    auto hyp   = new Isis::CLsHypothesis(array, fluct);

    return boost::shared_ptr<Isis::CLsHypothesis>( hyp );
  }

  //_______________________________________________________________________________
  //
  inline boost::shared_ptr<Isis::CLsHypothesis>
  constructor_NoFluctNoPrior( py::list lst ) {

    auto array = IBoost::boostListToStdVec<double>( lst );
    auto hyp   = new Isis::CLsHypothesis(array);

    return boost::shared_ptr<Isis::CLsHypothesis>( hyp );
  }

  //_______________________________________________________________________________
  //
  inline py::list getHyp( const Isis::CLsHypothesis &hyp ) {

    auto vec = hyp.getHyp();

    return IBoost::stdVecToBoostList( vec );
  }

  //_______________________________________________________________________________
  //
  inline double poissonProb( const Isis::CLsHypothesis &hyp, py::list values ) {

    auto vec = IBoost::boostListToStdVec<double>( values );

    return hyp.poissonProb( vec );
  }

  //_______________________________________________________________________________
  //
  inline py::list getTSVals( const Isis::CLsHypothesis &hyp ) {

    auto vec = hyp.getTSVals();

    return IBoost::stdVecToBoostList( vec );
  }

  //_______________________________________________________________________________
  //
  inline void setHyp( Isis::CLsHypothesis &hyp,
	       py::list lst,
	       Isis::CLsFluctuator *fluct = 0,
	       Isis::CLsPrior *prior = 0 ) {

    auto vec = IBoost::boostListToStdVec<double>( lst );

    hyp.setHyp(vec, fluct, prior);
  }

  BOOST_PYTHON_FUNCTION_OVERLOADS(setHyp_Overloads, setHyp, 2, 4);
}

// Wrappers for the class CLsPrior
namespace CLsPrior {

  struct CLsPriorWrap : Isis::CLsPrior, py::wrapper<Isis::CLsPrior> {

    double evaluate( const size_t &pos, const double &mean, const double &value ) {

      return this->get_override("evaluate")(pos, mean, value);
    }
  };
}
