//////////////////////////////////////////////////////////////////////////////////
//
//  Python wrappers
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 20/11/2016
//
// -------------------------------------------------------------------------------
//
//  Description:
//
//  Configuration of the functions to manage classes and functions from the
//  package General
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#include <boost/python.hpp>
#include <boost/python/scope.hpp>

#include "CutManager.h"
#include "GlobalMessenger.h"
#include "LoopArray.h"
#include "ProgressBar.h"

#include <string>

namespace py = boost::python;
namespace gl = General;


//_______________________________________________________________________________

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CutManager_BookCut_Overloads,
				       gl::CutManager::BookCut,
				       1, 2);

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CutManager_MakeMergedCut_Overloads,
				       gl::CutManager::MakeMergedCut,
				       0, 1);

BOOST_PYTHON_MODULE( PyGeneral ) {

  // Wrapper from CutManager.h
  py::class_<gl::CutManager>("CutManager", py::init<const std::string&>())
    .def(py::init<const gl::CutManager&>())
    .def("BookCut"      , &gl::CutManager::BookCut, CutManager_BookCut_Overloads())
    .def("Clear"        , &gl::CutManager::Clear)
    .def("Close"        , &gl::CutManager::Close)
    .def("GetCut"       , &gl::CutManager::GetCut)
    .def("MakeMergedCut", &gl::CutManager::MakeMergedCut, CutManager_MakeMergedCut_Overloads())
    .def("Open"         , &gl::CutManager::Open)
    .def("Print"        , &gl::CutManager::Print)
    .def("Remove"       , &gl::CutManager::Remove)
    .def("__getitem__"  , &gl::CutManager::operator[])
    ;

  // Wrappers from GlobalMessenger.h
  py::def("SendErrorMsg"  , &gl::SendErrorMsg);
  py::def("SendInfoMsg"   , &gl::SendInfoMsg);
  py::def("SendWarningMsg", &gl::SendWarningMsg);

  { // ANSIFormat scope
    class ANSIFormatNSP { };
    py::scope ANSIFormat = py::class_<ANSIFormatNSP>("ANSIFormat")
      .def_readwrite("ErrorColor"  , &gl::ANSIFormat::ErrorColor)
      .def_readwrite("InfoColor"   , &gl::ANSIFormat::InfoColor)
      .def_readwrite("WarningColor", &gl::ANSIFormat::WarningColor)
      ;
  
    py::enum_<gl::ANSIFormat::Colors>("Colors")
      .value("aBlack"  , gl::ANSIFormat::aBlack)
      .value("aRed"    , gl::ANSIFormat::aRed)
      .value("aGreen"  , gl::ANSIFormat::aGreen)
      .value("aYellow" , gl::ANSIFormat::aYellow)
      .value("aBlue"   , gl::ANSIFormat::aBlue)
      .value("aMagenta", gl::ANSIFormat::aMagenta)
      .value("aCyan"   , gl::ANSIFormat::aCyan)
      .value("aWhite"  , gl::ANSIFormat::aWhite)
      ;
    py::enum_<gl::ANSIFormat::Styles>("Styles")
      .value("aNormal"   , gl::ANSIFormat::aNormal)
      .value("aBold"     , gl::ANSIFormat::aBold)
      .value("aFaint"    , gl::ANSIFormat::aFaint)
      .value("aItalic"   , gl::ANSIFormat::aItalic)
      .value("aUnderline", gl::ANSIFormat::aUnderline)
      ;
  }

  // Wrappers from LoopArray.h
  py::class_<gl::LoopArray>("LoopArray", py::init<>())
    .def(py::init<const size_t&, const size_t&, const size_t&>())
    .def(py::init<const gl::LoopArray&>())
    .def("AddIndex"   , &gl::LoopArray::AddIndex)
    .def("GetNindices", &gl::LoopArray::GetNindices)
    .def("GetNloops"  , &gl::LoopArray::GetNloops)
    .def("GetPos"     , &gl::LoopArray::GetPos)
    .def("Next"       , &gl::LoopArray::Next)
    .def("Start"      , &gl::LoopArray::Start)
    .def("Status"     , &gl::LoopArray::Status)
    .def("__getitem__", &gl::LoopArray::operator[])
    ;
  
  // Wrappers from ProgessBar.h
  py::class_<gl::ProgressBar>("ProgressBar", py::init<const char*>())
    .def("End"     , &gl::ProgressBar::End)
    .def("Print"   , &gl::ProgressBar::Print)
    .def("SetColor", &gl::ProgressBar::SetColor)
    .def("Start"   , &gl::ProgressBar::Start)
    ;
}
