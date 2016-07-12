#/////////////////////////////////////////////////////////////
#// ------------------------------------------------------- //
#//                                                         //
#//  AUTHOR: Miguel Ramos Pernas                            //
#//  e-mail: miguel.ramos.pernas@cern.ch                    //
#//                                                         //
#//  Last update: 12/07/2016                                //
#//                                                         //
#// ------------------------------------------------------- //
#/////////////////////////////////////////////////////////////


import sys, pkgutil, inspect


# If this __init__.py file is executed, it will import all Isis and ROOT modules. Since
# ipython does not support the < import * > for the ROOT modules, if any argument is passed
# when executing this file it will only do < import ROOT >.
if __name__ == "__main__":
    from Isis import *
    if len( sys.argv ) > 1:
        import ROOT
    else:
        from ROOT import *
