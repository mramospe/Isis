# When importing all from Isis, there will be imported all the modules in the python folder

import os

path = os.path.abspath( __file__ )
path = path[ :path.rfind( '/' ) ]

__all__ = list( set( f[ :f.rfind( '.' ) ]
                     for f in os.listdir( path )
                     if ".py" in f and "__init__" not in f ) )

