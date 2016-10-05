#//////////////////////////////////////////////////////////
#//
#//  Python modules
#//
#// -------------------------------------------------------
#//
#//  AUTHOR: Miguel Ramos Pernas
#//  e-mail: miguel.ramos.pernas@cern.ch
#//
#//  Last update: 24/07/2015
#//
#// -------------------------------------------------------
#//
#//  Description:
#//
#//  List of functions to manage directories and files
#//  of the system.
#//
#// -------------------------------------------------------
#//////////////////////////////////////////////////////////


import os


#_______________________________________________________________________________
# Lists the directories in a given folder
def ListDirs( mainFolder ):
    return [ subFolder for subFolder in os.listdir( mainFolder )
             if os.path.isdir( os.path.join( mainFolder, subFolder ) ) ]

#_______________________________________________________________________________
# Lists the paths to the directories in the given folder recursively
def ListAllDirs( mainFolder ):

    dirList = []

    for subDir in ListDirs( mainFolder ):
        dirList += [ subDir ]
        dirList += ListAllDirs( mainFolder + '/' + subDir )

    return dirList

#_______________________________________________________________________________
# Lists the paths to the directories in the given folder
def ListDirPaths( mainFolder ):
    return [ mainFolder + '/' + subDir
             for subDir in ListDirs( mainFolder ) ]

#_______________________________________________________________________________
# Lists the paths to the directories in the given folder recursively
def ListAllDirPaths( mainFolder ):

    dirList = []

    for subDir in ListDirPaths( mainFolder ):
        dirList += [ subDir ]
        dirList += ListAllDirPaths( subDir )

    return dirList

#_______________________________________________________________________________
# Lists the files in a given folder
def ListFiles( mainFolder ):
    return [ subFile for subFile in os.listdir( mainFolder )
             if not os.path.isdir( os.path.join( mainFolder, subFile ) ) ]

#_______________________________________________________________________________
# Lists the paths to the files in the given folder recursively
def ListAllFiles( mainFolder ):

    fileList = ListFiles( mainFolder )

    for subDir in ListAllDirPaths( mainFolder ):
        fileList += ListFiles( subDir )

    return fileList

#_______________________________________________________________________________
# Lists the paths to the files in a given folder
def ListFilePaths( mainFolder ):
    return [ mainFolder + '/' + subFile
             for subFile in os.listdir( mainFolder )
             if not os.path.isdir( os.path.join( mainFolder, subFile ) ) ]

#_______________________________________________________________________________
# Lists the paths to the files in a given folder recursively
def ListAllFilePaths( mainFolder ):

    fileList = ListFilePaths( mainFolder )

    for subDir in ListAllDirPaths( mainFolder ):
        fileList += ListFilePaths( subDir )

    return fileList

#_______________________________________________________________________________
# Lists all the directories and files in a given folder, describing the complete
# tree of objects.
def ListTreePaths( mainFolder ):

    pathList = []

    for path in os.listdir( mainFolder ):

        ext_path = mainFolder + '/' + path

        pathList += [ ext_path ]

        if ( os.path.isdir( ext_path ) ):
            pathList += ListTreePaths( ext_path )

    return pathList
