#/////////////////////////////////////////////////////////////
#//                                                         //
#//  Python modules                                         //
#//                                                         //
#// ------------------------------------------------------- //
#//                                                         //
#//  AUTHOR: Miguel Ramos Pernas                            //
#//  e-mail: miguel.ramos.pernas@cern.ch                    //
#//                                                         //
#//  Last update: 11/11/2015                                //
#//                                                         //
#// ------------------------------------------------------- //
#//                                                         //
#//  Description:                                           //
#//                                                         //
#//  Defines some utils to manage python objects.           //
#//                                                         //
#// ------------------------------------------------------- //
#/////////////////////////////////////////////////////////////


from ROOT import TFile, TDirectoryFile
import os


#_______________________________________________________________________________
# Displays the given time in the format [ w, d, h, min, s ]. If one of the
# quantities is zero, it is not displayed.
def FormatTime( itime ):
    conv  = [ 60, 60, 24, 7 ]
    vlist = [ "s", "min", "h", "d", "w" ]
    vals  = { "s": itime, "min": 0, "h": 0, "d": 0, "w": 0 }
    for cval, nunit, ounit in zip( conv, vlist[ 1: ], vlist[ :-1 ] ):
        vals[ nunit ], vals[ ounit ] = divmod( int( vals[ ounit ] ), cval )
    vlist.reverse()
    strout = ""
    for kw in vlist:
        val = vals[ kw ]
        if val:
            strout += str( vals[ kw ] ) + kw + " "
    if strout:
        return strout[ :-1 ]
    else:
        return "0s"

#_______________________________________________________________________________
# Joins dictionaries with different keys into one. If some of them have the
# same key, this one is not considered and not added to the final dictionary.
def JoinDicts( *args ):
    rdict = {}
    for dic in args:
        for key in dic:
            if key in rdict:
                del rdict[ key ]
                print "Key <", key, "> already in dictionary. Not considered."
            else:
                rdict[ key ] = dic[ key ]
    return rdict

#_______________________________________________________________________________
# If the input is a list, it gets the length of the maximum string located
# inside it. If it is a dictionary, it gets the maximum value of the strings
# associated to the keys of it ( it has to be a dictionary of strings ).
def LargestString( lstdic ):
    maxlen = 0
    if isinstance( lstdic, list ):
        for el in lstdic:
            newlen = len( el )
            if newlen > maxlen: maxlen = newlen
    elif isinstance( lstdic, dict ):
        for kw in lstdic:
            newlen = len( lstdic[ kw ] )
            if newlen > maxlen: maxlen = newlen
    else:
        print "The input parameter is not a list nor a dictionary, returned 0"
    return maxlen

#_______________________________________________________________________________
# This function merges the values in various dictionaries into one
def MergeDicts( *args ):
    kvars = args[ 0 ].keys()
    for dic in args[ 1: ]:
        keys = dic.keys()
        for key in kvars:
            if key not in keys:
                kvars.remove( key )
    rdic = {}
    for key in kvars:
        rdic[ key ] = []
        for dic in args:
            rdic[ key ] += dic[ key ]
    return rdic

#_______________________________________________________________________________
# This class allows to manage on an easier way Root files and folders, keeping
# the information on a dictionary. It also increases the information displayed
# when managing them.
class SmartFile:

    #_______________________________________________________________________________
    # The Root file is created taking into account the option specified ( following
    # the same convention ). The < recreate > option can be forced adding a < F >
    # at the beginning or at the end. The option provided can mix uppercase and
    # lowercase letters.
    def __init__( self, fname, option = "RECREATE", **kwargs ):

        for el in kwargs:
            setattr( self, el, kwargs[ el ] )

        option = option.upper()

        ''' Here the different possibilities to read and write the file are processed '''
        if option in ( "NEW", "CREATE", "RECREATE", "FRECREATE", "RECREATEF" ):
            if option == "RECREATE":
                while not fname or os.path.isfile( fname ):
                    if fname:
                        dec = raw_input( "File with name < %s > already exists" % fname +
                                         "\nAre you sure you want to recreate it? (y/n): " )
                        if   dec == 'y': break
                        elif dec == 'n': fname = raw_input( "Select another name: " )
            elif option == "FRECREATE" or option == "RECREATEF":
                option = "RECREATE"
            else:
                option = "NEW"
                while not fname or os.path.isfile( fname ):
                    fname = raw_input( "File with name < %s > already exists" % fname +
                                       "\nSelect another name: " )
            read = False
            print "Created new file <", fname, ">"
        elif option == "UPDATE":
            read = True
            print "Opened file for updating <", fname, ">"
        elif option == "READ":
            read = True
            print "Opened file for reading <", fname, ">"
        else:
            print "File option <", option, "> not known"
            return

        self.FileDict = { "dir": TFile.Open( fname, option ), "folders": {} }

        ''' If the file is in read or update mode, the structure is taken '''
        if read:
            def recread( df, path ):
                for key in df.GetListOfKeys():
                    obj = df.Get( key.GetName() )
                    if isinstance( obj, TDirectoryFile ):
                        self.NewFolder( path, obj )
                        recread( obj, path + '/' + key.GetName() )
            f = self.FileDict[ "dir" ]
            for key in f.GetListOfKeys():
                obj = f.Get( key.GetName() )
                if isinstance( obj, TDirectoryFile ):
                    self.NewFolder( '', obj )
                    recread( obj, obj.GetName() )

    #_______________________________________________________________________________
    # Defines the function to delete the class. Since this class owns the file it
    # closes it.
    def __del__( self ):
        self.FileDict[ "dir" ].Close()

    #_______________________________________________________________________________
    # Gets the object at the path provided
    def __getitem__( self, path ):
        return self.FileDict[ "dir" ].Get( path )

    #_______________________________________________________________________________
    # Transforms the hierarchy of the file on a string of a list
    def __str__( self ):
        lst = []
        def recread( dic, path = '/' ):
            name = dic[ "dir" ].GetName()
            flst = dic[ "folders" ]
            for el in flst:
                newpath = path + name + '/'
                lst.append( newpath + el )
                recread( flst[ el ], newpath )
        fdict = self.FileDict[ "folders" ]
        for el in fdict:
            lst.append( el )
            recread( fdict[ el ], '' )
        return str( ( self.GetFileName(), lst ) )

    #_______________________________________________________________________________
    # The print of this class is its string representation
    def __repr__( self ):
        return self.__str__()

    #_______________________________________________________________________________
    # Changes the current directory to the given path
    def cd( self, path ):
        self.FileDict[ "dir" ].cd( path )

    #_______________________________________________________________________________
    # Creates a new folder at the given path. If now < newfolder > is providen, it
    # will take the last section of the path as the new folder. If < newfolder > is
    # a TDirectoryFile, it will asign the direction of the path given to it. On the
    # other hand, if a string is providen it will create a TDirectoryFile with that
    # name in the path given.
    def NewFolder( self, path, newfolder = False ):
        if not isinstance( newfolder, TDirectoryFile ):
            if isinstance( newfolder, str ):
                fname = newfolder
            elif not newfolder:
                path  = path.split( '/' )
                fname = path[ -1 ]
                path  = '/'.join( path[ :-1 ] )
            pfolder = self.GetFolder( path )
            if path: path += '/' + fname
            else:    path  = fname
            if self.Get( path ):
                print "Folder <", path, "> already exists"
                return
            pfolder[ "dir" ].cd()
            newfolder = TDirectoryFile( fname, fname )
            newfolder.Write()
            print "Created new folder in file <", self.GetFileName(), ">:", path
        else:
            pfolder = self.GetFolder( path )
            fname   = newfolder.GetName()
            if path: path += '/' + fname
            else:    path  = fname
            print "Booked new folder from file <", self.GetFileName(), ">:", path

        pfolder[ "folders" ][ fname ] = { "dir"     : newfolder,
                                          "folders" : {} }

    #_______________________________________________________________________________
    # Gets the TObject following the path given
    def Get( self, path ):
        return self.FileDict[ "dir" ].Get( path )

    #_______________________________________________________________________________
    # Gets the folder in the path given
    def GetFolder( self, path ):
        folder = self.FileDict
        if path:
            spath = path.split( '/' )
            for el in spath:
                try:
                    folder = folder[ "folders" ][ el ]
                except:
                    print "Could not access path <", path, ">"
                    return 0
        return folder

    #_______________________________________________________________________________
    # Returns the name of the file
    def GetFileName( self ):
        return self.FileDict[ "dir" ].GetName()

    #_______________________________________________________________________________
    # Saves recursively the folders in the file since the path given
    def Save( self, path = '*' ):
        if path == '*':
            self.FileDict[ "dir" ].Save()
        else:
            self.GetFolder( path ).Save()

    #_______________________________________________________________________________
    # Writes the given object in the path provided. If no path is given it will
    # write it at the first level of the file. The name can also be changed.
    def Write( self, obj, path = False, **kwargs ):
        if "name" in kwargs: name = kwargs[ "name" ]
        else: name = 0
        if path:
            self.cd( path )
            obj.Write( name )
        else:
            self.FileDict[ "dir" ].cd()
            obj.Write()
