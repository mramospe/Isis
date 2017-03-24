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
//  Wrappers from the AdaptiveBinning library
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#include "AdaptiveBinning.h"
#include "AdaptiveBinning1D.h"
#include "AdaptiveBinning2D.h"
#include "Bin.h"
#include "Bin1D.h"
#include "Bin2D.h"

#include "TPython.h"

namespace py = boost::python;


//_______________________________________________________________________________

// Wrappers for the class AdaptiveBinning
namespace AdBin {

  //_______________________________________________________________________________
  //
  inline py::list getBinList( const Isis::AdaptiveBinning &adbin ) {
  
    return IBoost::stdVecToBoostList( adbin.getBinList() );
  }

  //_______________________________________________________________________________
  //
  inline PyObject* getStruct( const Isis::AdaptiveBinning &adbin,
		       const char *name = "",
		       const char *title = "" ) {
    
    return TPython::ObjectProxy_FromVoidPtr( adbin.getStruct(name, title),
					     "TObject",
					     false );
  }  
}

// Wrappers for the class AdaptiveBinning1D
namespace AdBin1D {

  //_______________________________________________________________________________
  //
  inline boost::shared_ptr<Isis::AdaptiveBinning1D>
  constructor( size_t occ,
	       double vmin,
	       double vmax,
	       py::list values,
	       py::list weights ) {
  
    auto vec_values  = IBoost::boostListToStdVec<double>( values );
    auto vec_weights = IBoost::boostListToStdVec<double>( weights );
    auto adbin       = new Isis::AdaptiveBinning1D(occ, vmin, vmax,
						   vec_values, vec_weights);
  
    return boost::shared_ptr<Isis::AdaptiveBinning1D>( adbin );
  }

  //_______________________________________________________________________________
  //
  inline boost::shared_ptr<Isis::AdaptiveBinning1D>
  constructor_NoWgts( size_t occ,
		      double vmin,
		      double vmax,
		      py::list values ) {
    
    return constructor(occ, vmin, vmax, values, py::list());
  }

  //_______________________________________________________________________________
  //
  inline PyObject* getStruct( const Isis::AdaptiveBinning1D &adbin,
		       const char *name = "",
		       const char *title = "" ) {
    
    return TPython::ObjectProxy_FromVoidPtr( adbin.getStruct(name, title),
					     "TH1D",
					     false );
  }

  BOOST_PYTHON_FUNCTION_OVERLOADS(getStruct_Overloads, getStruct, 1, 3);
}

// Wrappers for the class AdaptiveBinning2D
namespace AdBin2D {

  //_______________________________________________________________________________
  //
  inline boost::shared_ptr<Isis::AdaptiveBinning2D>
  constructor( size_t occ,
	       double xmin, double xmax,
	       double ymin, double ymax,
	       py::list xvalues,
	       py::list yvalues,
	       py::list weights ) {
  
    auto vec_xvalues = IBoost::boostListToStdVec<double>( xvalues );
    auto vec_yvalues = IBoost::boostListToStdVec<double>( yvalues );
    auto vec_weights = IBoost::boostListToStdVec<double>( weights );
    auto adbin       = new Isis::AdaptiveBinning2D(occ,
						   xmin, xmax,
						   ymin, ymax,
						   vec_xvalues, vec_yvalues,
						   vec_weights);
  
    return boost::shared_ptr<Isis::AdaptiveBinning2D>( adbin );
  }

  //_______________________________________________________________________________
  //
  inline boost::shared_ptr<Isis::AdaptiveBinning2D>
  constructor_NoWgts( size_t occ,
		      double xmin,
		      double xmax,
		      double ymin,
		      double ymax,
		      py::list xvalues,
		      py::list yvalues ) {
  
    return constructor(occ, xmin, xmax, ymin, ymax, xvalues, yvalues, py::list());
  }

  //_______________________________________________________________________________
  //
  inline PyObject* getAdjStruct( const Isis::AdaptiveBinning2D &adbin,
				 const char *name = "",
				 const char *title = "" ) {

    return TPython::ObjectProxy_FromVoidPtr( adbin.getAdjStruct(name, title),
					     "TH2Poly",
					     false );
  }

  BOOST_PYTHON_FUNCTION_OVERLOADS(getAdjStruct_Overloads, getAdjStruct, 1, 3);

  //_______________________________________________________________________________
  //
  inline PyObject* getStruct( const Isis::AdaptiveBinning2D &adbin,
			      const char *name = "",
			      const char *title = "" ) {
    
    return TPython::ObjectProxy_FromVoidPtr( adbin.getStruct(name, title),
					     "TH2Poly",
					     false );
  }

  BOOST_PYTHON_FUNCTION_OVERLOADS(getStruct_Overloads, getStruct, 1, 3);
}
