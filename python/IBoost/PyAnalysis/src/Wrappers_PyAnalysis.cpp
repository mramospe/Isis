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
//  Configuration of the functions to manage classes and functions from the
//  package Analysis
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#include <boost/python.hpp>
#include <boost/python/list.hpp>
#include <boost/shared_ptr.hpp>

#include "GlobalWrappers.h"
#include "PyAdaptiveBinning.h"
#include "PyAnalysisBase.h"
#include "PyStatistics.h"

#include <string>

namespace py = boost::python;
namespace an = Analysis;


//_______________________________________________________________________________

BOOST_PYTHON_MODULE( PyAnalysis ) {

  // Wrapper from AdaptiveBinning.h
  py::class_<an::AdaptiveBinning>("AdaptiveBinning", py::init<>())
    .def("GetStruct" , &AdBin::GetStruct)
    .def("GetBinList", &AdBin::GetBinList)
    .def("GetNbins"  , &an::AdaptiveBinning::GetNbins)
    ;

  // Wrapper from AdaptiveBinning1D.h
  py::class_<an::AdaptiveBinning1D, py::bases<an::AdaptiveBinning> >
    ("AdaptiveBinning1D", py::no_init)
    .def("__init__"     , py::make_constructor(&AdBin1D::Constructor))
    .def("__init__"     , py::make_constructor(&AdBin1D::Constructor_NoWgts))
    .def_readonly("Max" , &an::AdaptiveBinning1D::GetMax)
    .def_readonly("Min" , &an::AdaptiveBinning1D::GetMin)
    .def("GetStruct"    , &AdBin1D::GetStruct, AdBin1D::GetStruct_Overloads())
    ;

  // Wrapper from AdaptiveBinning2D.h
  py::class_<an::AdaptiveBinning2D, py::bases<an::AdaptiveBinning> >
    ("AdaptiveBinning2D", py::no_init)
    .def("__init__"     , py::make_constructor(&AdBin2D::Constructor))
    .def("__init__"     , py::make_constructor(&AdBin2D::Constructor_NoWgts))
    .def("GetAdjStruct" , &AdBin2D::GetAdjStruct, AdBin2D::GetAdjStruct_Overloads())
    .def("GetStruct"    , &AdBin2D::GetStruct, AdBin2D::GetStruct_Overloads())
    .def_readonly("Xmax", &an::AdaptiveBinning2D::GetXmax)
    .def_readonly("Xmin", &an::AdaptiveBinning2D::GetXmin)
    .def_readonly("Ymax", &an::AdaptiveBinning2D::GetYmax)
    .def_readonly("Ymin", &an::AdaptiveBinning2D::GetYmin)
    ;

  // Wrapper from Bin.h. This class is meant to be used in read-only mode.
  py::class_<an::Bin>("Bin", py::no_init)
    .def("GetEntries"     , &an::Bin::GetEntries)
    .def("GetSumOfWeights", &an::Bin::GetSumOfWeights)
    ;

  // Wrapper from Bin1D.h. This class is meant to be used in read-only mode.
  py::class_<an::Bin1D, py::bases<an::Bin> >("Bin1D", py::no_init)
    .def_readonly("Min", &an::Bin1D::GetMin)
    ;

  // Wrapper from Bin2D.h. This class is meant to be used in read-only mode.
  py::class_<an::Bin2D, py::bases<an::Bin> >("Bin2D", py::no_init)
    .def_readonly("Xmax", &an::Bin2D::GetXmax)
    .def_readonly("Xmin", &an::Bin2D::GetXmin)
    .def_readonly("Ymax", &an::Bin2D::GetYmax)
    .def_readonly("Ymin", &an::Bin2D::GetYmin)
    ;

  // Wrapper from CLsFluctuator
  py::class_<CLsFluct::CLsFluctWrap, boost::noncopyable>("CLsFluctuator")
    .def("Fluctuate", py::pure_virtual(&CLsFluct::CLsFluctWrap::Fluctuate))
    ;

  // Wrapper from CLsPrior
  py::class_<CLsPrior::CLsPriorWrap, boost::noncopyable>("CLsPrior")
    .def("Evaluate", py::pure_virtual(&CLsPrior::CLsPriorWrap::Evaluate))
    ;

  // Wrapper from CLsResult
  py::class_<an::CLsResult>("CLsResult",
			    py::init<
			    const double&,
			    const double&,
			    const double&,
			    const double&,
			    const double&>())
    .def_readonly("Alpha"       , &an::CLsResult::Alpha)
    .def_readonly("Beta"        , &an::CLsResult::Beta)
    .def_readonly("CLb"         , &an::CLsResult::CLb)
    .def_readonly("CLs"         , &an::CLsResult::CLs)
    .def_readonly("CLsb"        , &an::CLsResult::CLsb)
    .def_readonly("TestStat"    , &an::CLsResult::TestStat)
    .def_readonly("Significance", &an::CLsResult::Significance)
    .def("GaussSigPos"          , &an::CLsResult::GaussSigPos)
    .def("IsDiscovery"          , &an::CLsResult::IsDiscovery)
    .def("IsEvidence"           , &an::CLsResult::IsEvidence)
    ;
    
  // Wrapper from CLsHypothesis
  py::class_<an::CLsHypothesis>("CLsHypothesis", py::init<>())
    .def("__init__"        , py::make_constructor(&CLsHyp::Constructor))
    .def("__init__"        , py::make_constructor(&CLsHyp::Constructor_NoPrior))
    .def("__init__"        , py::make_constructor(&CLsHyp::Constructor_NoFluctNoPrior))
    .def("PValue"          , &an::CLsHypothesis::PValue)
    .def("GetTSVals"       , &CLsHyp::GetTSVals)
    .def("GetHyp"          , &CLsHyp::GetHyp)
    .def("Generate"        , &an::CLsHypothesis::Generate)
    .def("PoissonProb"     , &CLsHyp::PoissonProb)
    .def("SetFactory"      , &an::CLsHypothesis::SetFactory)
    .def("SetFluctuator"   , &an::CLsHypothesis::SetFluctuator)
    .def("SetHyp"          , &CLsHyp::SetHyp, CLsHyp::SetHyp_Overloads())
    .def("SetPrior"        , &an::CLsHypothesis::SetPrior)
    .def("TestStatFromProb", &an::CLsHypothesis::TestStatFromProb)
    ;

  py::enum_<an::CLsHypTypes>("CLsHypTypes")
    .value("aNone"  , an::CLsHypTypes::aNone)
    .value("aNull"  , an::CLsHypTypes::aNull)
    .value("aSignal", an::CLsHypTypes::aSignal)
    ;
  
  // Wrapper from CLsFactory
  py::class_<an::CLsFactory>("CLsFactory", py::init<>())
    .def(py::init<an::CLsHypothesis&, an::CLsHypothesis&>())
    .def("Alpha"           , &an::CLsFactory::Alpha)
    .def("Beta"            , &an::CLsFactory::Beta)
    .def("Calculate"       , &CLsFact::CalculateFromArray)
    .def("Calculate"       , CLsFact::CalculateFromDouble)
    .def("CLb"             , &an::CLsFactory::CLb)
    .def("CLs"             , &an::CLsFactory::CLs)
    .def("CLsb"            , &an::CLsFactory::CLsb)
    .def("Generate"        , &an::CLsFactory::Generate)
    .def("TestStat"        , &CLsFact::TestStat)
    .add_property("NullHyp", &CLsFact::GetNullHyp, &an::CLsFactory::SetNullHyp)
    .add_property("SigHyp" , &CLsFact::GetSigHyp, &an::CLsFactory::SetSigHyp)
    ;
  
  // Wrappers from RootUtils
  py::def("GetSafeObject", &RootUtils::GetSafeObject);
}
