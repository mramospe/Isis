#////////////////////////////////////////////////////////////////
#//
#//  Python modules
#//
#// -------------------------------------------------------------
#//
#//  AUTHOR: Miguel Ramos Pernas
#//  e-mail: miguel.ramos.pernas@cern.ch
#//
#//  Last update: 12/04/2017
#//
#// -------------------------------------------------------------
#//
#//  Description:
#//
#//  Define the way to import the python modules
#//
#// -------------------------------------------------------------
#////////////////////////////////////////////////////////////////


import os


'''
Import all packages and modules in this directory
'''
path = os.path.dirname(os.path.realpath(__file__))
flst = os.listdir(path)
pkgs = filter(lambda f: os.path.isdir(os.path.join(path, f)), flst)
pfls = filter(lambda f: os.path.isfile(os.path.join(path, f)) and f != __file__, flst)
mods = [el[:el.rfind('.')] for el in pfls]

__all__ = pkgs + mods

del os, path, flst, pkgs, pfls, mods, el

