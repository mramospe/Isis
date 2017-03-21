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


//_______________________________________________________________________________

BOOST_PYTHON_MODULE( PyAnalysis ) {

  // Wrapper from AdaptiveBinning.h
  py::class_<Isis::AdaptiveBinning>("AdaptiveBinning", py::init<>())
    .def("GetStruct" , &AdBin::GetStruct)
    .def("GetBinList", &AdBin::GetBinList)
    .def("GetNbins"  , &Isis::AdaptiveBinning::GetNbins)
    ;

  // Wrapper from AdaptiveBinning1D.h
  py::class_<Isis::AdaptiveBinning1D, py::bases<Isis::AdaptiveBinning> >
    ("AdaptiveBinning1D", py::no_init)
    .def("__init__"     , py::make_constructor(&AdBin1D::Constructor))
    .def("__init__"     , py::make_constructor(&AdBin1D::Constructor_NoWgts))
    .def_readonly("Max" , &Isis::AdaptiveBinning1D::GetMax)
    .def_readonly("Min" , &Isis::AdaptiveBinning1D::GetMin)
    .def("GetStruct"    , &AdBin1D::GetStruct, AdBin1D::GetStruct_Overloads())
    ;

  // Wrapper from AdaptiveBinning2D.h
  py::class_<Isis::AdaptiveBinning2D, py::bases<Isis::AdaptiveBinning> >
    ("AdaptiveBinning2D", py::no_init)
    .def("__init__"     , py::make_constructor(&AdBin2D::Constructor))
    .def("__init__"     , py::make_constructor(&AdBin2D::Constructor_NoWgts))
    .def("GetAdjStruct" , &AdBin2D::GetAdjStruct, AdBin2D::GetAdjStruct_Overloads())
    .def("GetStruct"    , &AdBin2D::GetStruct, AdBin2D::GetStruct_Overloads())
    .def_readonly("Xmax", &Isis::AdaptiveBinning2D::GetXmax)
    .def_readonly("Xmin", &Isis::AdaptiveBinning2D::GetXmin)
    .def_readonly("Ymax", &Isis::AdaptiveBinning2D::GetYmax)
    .def_readonly("Ymin", &Isis::AdaptiveBinning2D::GetYmin)
    ;

  // Wrapper from Bin.h. This class is meant to be used in read-only mode.
  py::class_<Isis::Bin>("Bin", py::no_init)
    .def("GetEntries"     , &Isis::Bin::GetEntries)
    .def("GetSumOfWeights", &Isis::Bin::GetSumOfWeights)
    ;

  // Wrapper from Bin1D.h. This class is meant to be used in read-only mode.
  py::class_<Isis::Bin1D, py::bases<Isis::Bin> >("Bin1D", py::no_init)
    .def_readonly("Min", &Isis::Bin1D::GetMin)
    ;

  // Wrapper from Bin2D.h. This class is meant to be used in read-only mode.
  py::class_<Isis::Bin2D, py::bases<Isis::Bin> >("Bin2D", py::no_init)
    .def_readonly("Xmax", &Isis::Bin2D::GetXmax)
    .def_readonly("Xmin", &Isis::Bin2D::GetXmin)
    .def_readonly("Ymax", &Isis::Bin2D::GetYmax)
    .def_readonly("Ymin", &Isis::Bin2D::GetYmin)
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
  py::class_<Isis::CLsResult>("CLsResult",
			      py::init<
			      const double&,
			      const double&,
			      const double&,
			      const double&,
			      const double&>())
    .def_readonly("Alpha"       , &Isis::CLsResult::Alpha)
    .def_readonly("Beta"        , &Isis::CLsResult::Beta)
    .def_readonly("CLb"         , &Isis::CLsResult::CLb)
    .def_readonly("CLs"         , &Isis::CLsResult::CLs)
    .def_readonly("CLsb"        , &Isis::CLsResult::CLsb)
    .def_readonly("TestStat"    , &Isis::CLsResult::TestStat)
    .def_readonly("Significance", &Isis::CLsResult::Significance)
    .def("GaussSigPos"          , &Isis::CLsResult::GaussSigPos)
    .def("IsDiscovery"          , &Isis::CLsResult::IsDiscovery)
    .def("IsEvidence"           , &Isis::CLsResult::IsEvidence)
    ;
    
  // Wrapper from CLsHypothesis
  py::class_<Isis::CLsHypothesis>("CLsHypothesis", py::init<>())
    .def("__init__"        , py::make_constructor(&CLsHyp::Constructor))
    .def("__init__"        , py::make_constructor(&CLsHyp::Constructor_NoPrior))
    .def("__init__"        , py::make_constructor(&CLsHyp::Constructor_NoFluctNoPrior))
    .def("PValue"          , &Isis::CLsHypothesis::PValue)
    .def("GetTSVals"       , &CLsHyp::GetTSVals)
    .def("GetHyp"          , &CLsHyp::GetHyp)
    .def("Generate"        , &Isis::CLsHypothesis::Generate)
    .def("PoissonProb"     , &CLsHyp::PoissonProb)
    .def("SetFactory"      , &Isis::CLsHypothesis::SetFactory)
    .def("SetFluctuator"   , &Isis::CLsHypothesis::SetFluctuator)
    .def("SetHyp"          , &CLsHyp::SetHyp, CLsHyp::SetHyp_Overloads())
    .def("SetPrior"        , &Isis::CLsHypothesis::SetPrior)
    .def("TestStatFromProb", &Isis::CLsHypothesis::TestStatFromProb)
    ;

  py::enum_<Isis::CLsHypTypes>("CLsHypTypes")
    .value("aNone"  , Isis::CLsHypTypes::aNone)
    .value("aNull"  , Isis::CLsHypTypes::aNull)
    .value("aSignal", Isis::CLsHypTypes::aSignal)
    ;
  
  // Wrapper from CLsFactory
  py::class_<Isis::CLsFactory>("CLsFactory", py::init<>())
    .def(py::init<Isis::CLsHypothesis&, Isis::CLsHypothesis&>())
    .def("Alpha"           , &Isis::CLsFactory::Alpha)
    .def("Beta"            , &Isis::CLsFactory::Beta)
    .def("Calculate"       , &CLsFact::CalculateFromArray)
    .def("Calculate"       , CLsFact::CalculateFromDouble)
    .def("CLb"             , &Isis::CLsFactory::CLb)
    .def("CLs"             , &Isis::CLsFactory::CLs)
    .def("CLsb"            , &Isis::CLsFactory::CLsb)
    .def("Generate"        , &Isis::CLsFactory::Generate)
    .def("TestStat"        , &CLsFact::TestStat)
    .add_property("NullHyp", &CLsFact::GetNullHyp, &Isis::CLsFactory::SetNullHyp)
    .add_property("SigHyp" , &CLsFact::GetSigHyp, &Isis::CLsFactory::SetSigHyp)
    ;
  
  // Wrappers from RootUtils
  py::def("GetSafeObject", &RootUtils::GetSafeObject);
}
