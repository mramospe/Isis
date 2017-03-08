//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 08/03/2017
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
namespace an = Analysis;


//_______________________________________________________________________________

// Wrappers for the class AdaptiveBinning
namespace AdBin {

  //_______________________________________________________________________________
  //
  py::list GetBinList( const an::AdaptiveBinning &adbin ) {
  
    return IBoost::StdVecToBoostList( adbin.GetBinList() );
  }

  //_______________________________________________________________________________
  //
  PyObject* GetStruct( const an::AdaptiveBinning &adbin,
		       const char *name = "",
		       const char *title = "" ) {
    return TPython::ObjectProxy_FromVoidPtr( adbin.GetStruct(name, title),
					     "TObject",
					     false );
  }  
}

// Wrappers for the class AdaptiveBinning1D
namespace AdBin1D {

  //_______________________________________________________________________________
  //
  boost::shared_ptr<an::AdaptiveBinning1D> Constructor( size_t occ,
							double vmin,
							double vmax,
							py::list values,
							py::list weights ) {
  
    auto vec_values  = IBoost::BoostListToStdVec<double>( values );
    auto vec_weights = IBoost::BoostListToStdVec<double>( weights );
    auto adbin       = new an::AdaptiveBinning1D(occ, vmin, vmax,
						 vec_values, vec_weights);
  
    return boost::shared_ptr<an::AdaptiveBinning1D>( adbin );
  }

  //_______________________________________________________________________________
  //
  boost::shared_ptr<an::AdaptiveBinning1D> Constructor_NoWgts( size_t occ,
							       double vmin,
							       double vmax,
							       py::list values ) {
    return Constructor(occ, vmin, vmax, values, py::list());
  }

  //_______________________________________________________________________________
  //
  PyObject* GetStruct( const an::AdaptiveBinning1D &adbin,
		       const char *name = "",
		       const char *title = "" ) {
    return TPython::ObjectProxy_FromVoidPtr( adbin.GetStruct(name, title),
					     "TH1D",
					     false );
  }

  BOOST_PYTHON_FUNCTION_OVERLOADS(GetStruct_Overloads, GetStruct, 1, 3);
}

// Wrappers for the class AdaptiveBinning2D
namespace AdBin2D {

  //_______________________________________________________________________________
  //
  boost::shared_ptr<an::AdaptiveBinning2D> Constructor( size_t occ,
							double xmin, double xmax,
							double ymin, double ymax,
							py::list xvalues,
							py::list yvalues,
							py::list weights ) {
  
    auto vec_xvalues = IBoost::BoostListToStdVec<double>( xvalues );
    auto vec_yvalues = IBoost::BoostListToStdVec<double>( yvalues );
    auto vec_weights = IBoost::BoostListToStdVec<double>( weights );
    auto adbin       = new an::AdaptiveBinning2D(occ,
						 xmin, xmax,
						 ymin, ymax,
						 vec_xvalues, vec_yvalues,
						 vec_weights);
  
    return boost::shared_ptr<an::AdaptiveBinning2D>( adbin );
  }

  //_______________________________________________________________________________
  //
  boost::shared_ptr<an::AdaptiveBinning2D> Constructor_NoWgts( size_t occ,
							       double xmin,
							       double xmax,
							       double ymin,
							       double ymax,
							       py::list xvalues,
							       py::list yvalues ) {
  
    return Constructor(occ, xmin, xmax, ymin, ymax, xvalues, yvalues, py::list());
  }

  //_______________________________________________________________________________
  //
  PyObject* GetAdjStruct( const an::AdaptiveBinning2D &adbin,
			  const char *name = "",
			  const char *title = "" ) {
    return TPython::ObjectProxy_FromVoidPtr( adbin.GetAdjStruct(name, title),
					     "TH2Poly",
					     false );
  }

  BOOST_PYTHON_FUNCTION_OVERLOADS(GetAdjStruct_Overloads, GetAdjStruct, 1, 3);

  //_______________________________________________________________________________
  //
  PyObject* GetStruct( const an::AdaptiveBinning2D &adbin,
		       const char *name = "",
		       const char *title = "" ) {
    return TPython::ObjectProxy_FromVoidPtr( adbin.GetStruct(name, title),
					     "TH2Poly",
					     false );
  }

  BOOST_PYTHON_FUNCTION_OVERLOADS(GetStruct_Overloads, GetStruct, 1, 3);
}
