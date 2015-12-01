#/////////////////////////////////////////////////////////////
#// ------------------------------------------------------- //
#//                                                         //
#//  AUTHOR: Miguel Ramos Pernas                            //
#//  e-mail: miguel.ramos.pernas@cern.ch                    //
#//                                                         //
#//  Last update: 01/12/2015                                //
#//                                                         //
#// ------------------------------------------------------- //
#/////////////////////////////////////////////////////////////


import os, sys, pkgutil, inspect


# If this __init__.py file is executed, it will import all Isis and ROOT modules. Since
# ipython does not support the < import * > for the ROOT modules, if any argument is passed
# when executing this file it will only do < import ROOT >.
if __name__ == "__main__":
    from Isis import *
    if len( sys.argv ) > 1:
        import ROOT
    else:
        from ROOT import *
# All the classes can be initialized from Isis directly, without having to specify the
# module. However it is recommended to use the complete path of modules to avoid
# confusions. Obviously, when importing all from Isis, there will be imported all the
# modules in the python folder.
else:
    __all__ = []
    for loader, name, is_pkg in pkgutil.walk_packages( __path__ ):
        module = loader.find_module( name ).load_module( name )
        for name, value in inspect.getmembers( module ):
            if name.startswith( '__' ):
                continue
            globals()[ name ] = value
            __all__.append( name )
