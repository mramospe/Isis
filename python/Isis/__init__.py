#/////////////////////////////////////////////////////////////
#// ------------------------------------------------------- //
#//                                                         //
#//  AUTHOR: Miguel Ramos Pernas                            //
#//  e-mail: miguel.ramos.pernas@cern.ch                    //
#//                                                         //
#//  Last update: 02/12/2015                                //
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
# All the classes and functions located in the main Isis directory will be loaded, so they 
# can be initialized directly. However it is recommended to use the complete path of modules
# to avoid confusions. Obviously, when importing all from Isis, there will be imported all
# the modules in the python folder.
else:
    __all__ = []
    for loader, modname, is_pkg in pkgutil.walk_packages( __path__ ):
        module = loader.find_module( modname ).load_module( modname )
        globals()[ modname ] = module
        __all__.append( modname )
        for name, value in inspect.getmembers( module ):
            if not inspect.ismodule( value ) and not name.startswith( '__' ):
                if value.__module__ == modname:
                    globals()[ name ] = value
                    __all__.append( name )
