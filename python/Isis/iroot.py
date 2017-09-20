#/////////////////////////////////////////////////////////////
#//
#//  Python modules
#//
#// ----------------------------------------------------------
#//
#//  AUTHOR: Miguel Ramos Pernas
#//  e-mail: miguel.ramos.pernas@cern.ch
#//
#//  Last update: 15/09/2017
#//
#// ----------------------------------------------------------
#//
#//  Description:
#//
#//  Wraps the import of the Root modules disabling the
#//  command line options
#//
#// ----------------------------------------------------------
#/////////////////////////////////////////////////////////////


import ROOT

ROOT.PyConfig.IgnoreCommandLineOptions = True
