#=======================================================#
# File to compile the python source of the Isis project #
#-------------------------------------------------------#
# AUTHOR: Miguel Ramos Pernas	                        #
# e-mail: miguel.ramos.pernas@cern.ch	                #
#=======================================================#

import os, sys, py_compile

source_dir = sys.argv[ 1 ]
binary_dir = sys.argv[ 2 ]

if source_dir[ -1 ] != '/': source_dir += '/'
if binary_dir[ -1 ] != '/': binary_dir += '/'

tree = list( os.walk( source_dir ) )

if source_dir != binary_dir:
    replace = True
else:
    replace = False

for idir, dirs, files in tree:
    ndir = idir.replace( source_dir , "" )
    wdir = binary_dir
    if idir[ -1 ] != '/':
        idir += '/'
    if ndir:
        wdir += ndir
    for d in dirs:
        if not os.path.exists( wdir + d ):
            os.system( "mkdir " + wdir + d )
            print "Created new folder in:", wdir + d
    for f in files:
        if f[ -3: ] == ".py":
            ifile = idir + f
            ofile = wdir + f + "c"
            if not os.path.exists( ofile ) or os.stat( ofile ).st_mtime < os.stat( ifile ).st_mtime:
                print "Building file", ofile
                py_compile.compile( ifile )
                if replace:
                    os.system( "mv " + ifile + "c " + ofile )
