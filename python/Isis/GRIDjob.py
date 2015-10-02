#//////////////////////////////////////////////////////////
#//                                                      //
#//  Python modules                                      //
#//                                                      //
#// ---------------------------------------------------- //
#//                                                      //
#//  AUTHOR: Miguel Ramos Pernas                         //
#//  e-mail: miguel.ramos.pernas@cern.ch                 //
#//                                                      //
#//  Last update: 08/09/2015                             //
#//                                                      //
#// ---------------------------------------------------- //
#//                                                      //
#//  Description:                                        //
#//                                                      //
#//  Function that executes a given python script in     //
#//  the GRID computation system using DaVinci.          //
#//                                                      //
#// ---------------------------------------------------- // 
#//////////////////////////////////////////////////////////


import os
from Ganga.GPI import Job, Dirac, GaudiPython, SplitByFiles


#_______________________________________________________________________________
# Funtion to execute jobs in GRID
def GRIDjob( script, job_name, data_file, is_local = False ):

    # Initializes GaudiPython application
    app = GaudiPython( project = 'DaVinci', version = 'v36r3p1' )

    app.script            = script
    app.user_release_area = "$HOME/cmtuser"

    # Changes the type of work depending if the job is local or remote
    if is_local:
        j = Job( application = app, backend = Local() )
    else:
        j = Job( application = app, backend = Dirac(), splitter = SplitByFiles() )

    # Sets the job name and the path to the script
    j.name        = job_name
    j.inputfiles += [ os.environ[ "PWD" ] + '/' + script ]

    # Imports the data ( in the file the list has to named by dataLFN )
    dataset = __import__( data_file )

    j.inputdata = dataset.dataList

    # Sets the name of the output files
    j.outputfiles = [ 'DTT.root'  ]

    return j
