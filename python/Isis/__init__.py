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

import os, sys

# When importing all from Isis, there will be imported all the modules in the python folder
path = os.path.abspath( __file__ )
path = path[ :path.rfind( '/' ) ]
__all__ = list( set( f[ :f.rfind( '.' ) ]
                     for f in os.listdir( path )
                     if ".py" in f and "__init__" not in f ) )

# If this __init__.py file is executed, it will import all Isis and ROOT modules. Since
# ipython does not support the < import * > for the ROOT modules, if any argument is passed
# when calling this file it will only do < import ROOT >.
if __name__ == "__main__":
    from Isis import *
    if len( sys.argv ) > 1:
        import ROOT
    else:
        from ROOT import *
