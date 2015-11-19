#/////////////////////////////////////////////////////////////
#//                                                         //
#//  Python modules                                         //
#//                                                         //
#// ------------------------------------------------------- //
#//                                                         //
#//  AUTHOR: Miguel Ramos Pernas                            //
#//  e-mail: miguel.ramos.pernas@cern.ch                    //
#//                                                         //
#//  Last update: 19/11/2015                                //
#//                                                         //
#// ------------------------------------------------------- //
#//                                                         //
#//  Description:                                           //
#//                                                         //
#//  Defines classes and functions to manage data from/to   //
#//  Root files and trees.                                  //
#//                                                         //
#// ------------------------------------------------------- //
#/////////////////////////////////////////////////////////////


from ROOT import TFile, TTree, gDirectory
from array import array
from copy import deepcopy
import math
from Isis.Algebra import LongVector, Matrix
from Isis.PlotTools import MakeHistogram, MakeHistogram2D, MakeScatterPlot
from Isis.Utils import JoinDicts, MergeDicts


#_______________________________________________________________________________
# Class to manage data, specially designed to work together with Root files
# and trees
class DataManager:

    def __init__( self, fname = False, tnames = [], **kwargs ):
        ''' The constructor works in two different ways. By default it is considered that
        the input file, whose name is set in < fname >, is a Root file, and < tnames >
        should be the list of TTree names in the file. In < kwargs >, you can specify
        the file type ( fileType = "root"/"txt" ). If it is set to "txt", then < tnames >
        should be a list containing the names of the different variables. The number of
        variables specified should match the number of columns in the file. Otherwise one
        has to specify via < colIndex = [ 1, 3, 6, ... ] > the index for the columns to be
        stored. Other attributes can be added too.'''

        if "fileType" in kwargs:
            fileType = kwargs[ "fileType" ]
            del kwargs[ "fileType" ]
        else:
            fileType = "root"

        ''' These are the main attributes that this class has '''
        self.Iterator    = 0
        self.Nentries    = 0
        self.OwnsTargets = False
        self.Targets     = {}
        self.Variables   = {}

        if fileType in ( "root", "Root", "ROOT" ):
            ''' This is the constructor for Root files '''
            if fname and tnames:
                self.AddTarget( fname, tnames )
            elif fname and not tnames:
                print "Arguments for DataManager class are < file path > and  < tree name >"
                exit()
        elif fileType in ( "txt", "TXT" ):
            ''' This is the constructor for txt files '''
            ifile = open( fname, "rt" )
            line  = ifile.readline().split()
            if "colIndex" in kwargs:
                columns = kwargs[ "colIndex" ]
                del kwargs[ "colIndex" ]
            else:
                columns = range( len( line ) )
            print "Storing", len( columns ), "variables from txt file <", fname, ">"
            convfuncs, varvalues = [], []
            for index, icol in enumerate( columns ):
                value = line[ icol ]
                try:
                    int( value )
                    convfuncs.append( int )
                except:
                    pass
                try:
                    float( value )
                    convfuncs.append( float )
                except:
                    print "Format for column <", i, "> not recognised"
                    exit()
                varvalues.append( [ convfuncs[ -1 ]( value ) ] )
            for line in ifile:
                line = line.split()
                for index, icol in enumerate( columns ):
                    varvalues[ index ].append( convfuncs[ index ]( line[ icol ] ) )
            for index, name in enumerate( tnames ):
                self.Variables[ name ] = varvalues[ index ]
            self.Nentries = len( self.Variables[ name ] )
        else:
            ''' If the type specified is not recognised an error is shown and exits the program '''
            print "File format <", fileType, "> for DataManager class not known"
            exit()

        for kw in kwargs:
            setattr( self, kw, kwargs[ kw ] )            

    def __add__( self, other ):
        ''' Allows merging two objects of this class. The new manager owns all the
        targets. '''
        mngr          = DataManager()
        mngr.Nentries = self.Nentries + other.Nentries
        mngr.Targets.update( self.Targets  )
        mngr.Targets.update( other.Targets )
        true_vars = [ var for var in self.Variables if var in other.Variables ]
        for var in true_vars:
            mngr.Variables[ var ] = self.Variables[ var ] + other.Variables[ var ]
        self.OwnsTargets  = False
        other.OwnsTargets = False
        return mngr

    def __iadd__( self, other ):
        ''' Allows adding another manager variables to this class. The manager added
        looses the possession of the targets. '''
        if self.Nentries:
            mngr = self + other
            other.OwnsTargets = False
            return self + other
        else:
            self = deepcopy( other )
            other.OwnsTargets = False

    def __del__( self ):
        ''' Definition of the delete destructor. If this class owns the target files
        they are closed. '''
        if self.OwnsTargets:
            for tgt in self.Targets:
                for tree in self.Targets[ tgt ]:
                    tree = 0
                tgt.Close()

    def __getitem__( self, var ):
        ''' Gets the values for variable < var > '''
        return self.Variables[ var ]

    def __iter__( self ):
        ''' Definition of the iterator '''
        self.Iterator = 0
        return self

    def __len__( self ):
        ''' Gets the number of entries in the class '''
        return self.Nentries

    def next( self ):
        ''' Sets the new value for the iteration. If it reaches the limit the exception
        is raised. '''
        if self.Iterator == self.Nentries:
            raise StopIteration
        else:
            self.Iterator += 1
            return self.GetEventDict( self.Iterator - 1 )

    def AddTarget( self, fname, tnames ):
        ''' Adds a new target file and/or tree to the class. One has to provide the file
        name and the tree names. '''
        if not self.OwnsTargets: self.OwnsTargets = True
        if fname not in [ ifile.GetName() for ifile in self.Targets ]:
            ifile = TFile.Open( fname, "READ" )
            tlist = [ ifile.Get( tname ) for tname in tnames ]
            self.Targets[ ifile ]  = tlist
            print "Added target <", fname, "> and trees:", tnames
        else:
            ifile = [ ifile for ifile in self.Targets if ifile.GetName() == fname ][ 0 ]
            tlist = [ ifile.Get( tname ) for tname in tnames ]
            self.Targets[ ifile ] += tlist
            print "Added trees: ", list( tlist ), "to target <", ifile.GetName(), ">"
        if self.Variables:
            dictlist        = [ DictFromTree( tree, self.Variables.keys() ) for tree in tlist ]
            self.Variables  = JoinDicts( self.Variables, MergeDicts( *dictlist ) )
            self.Nentries   = len( self.Variables[ self.Variables.keys()[ 0 ] ] )

    def AddVariables( self, *fvars ):
        ''' Adds a new  variable(s) to the class given a name and a list. This method
        checks that all the variables' lists have the same lengths, otherwise it
        exits he program. The input has to be formated as [ var1_name, list1 ],
        [ var2_name, list2 ], ... '''
        if self.Nentries == 0:
            self.Nentries = len( fvars[ 0 ][ 1 ] )
        for el in fvars:
            if len( el[ 1 ] ) == self.Nentries:
                self.Variables[ el[ 0 ] ] = el[ 1 ]
            else:
                print "Variable", el[ 0 ], "to be added to the manager has different length."
                exit()
            
    def BookVariables( self, *var_names ):
        ''' Books a new variable(s) to the class. The variable's values list is filled
        with the variable's values contained in the targets. If a variable already
        exists in the class the process is omited. If < * > is specified, the
        variables are set as those that are common for all the trees. '''
        if len( var_names ) == 1 and '*' in var_names[ 0 ]:
            tlist     = self.GetListOfTrees()
            brlist    = tlist[ 0 ].GetListOfBranches()
            var_names = [ brlist.At( i ).GetName() for i in xrange( brlist.GetSize() ) ]
            for tree in tlist[ 1: ]:
                brlist        = tree.GetListOfBranches()
                new_var_names = []
                for i in xrange( brlist.GetSize() ):
                    brname = brlist.At( i ).GetName()
                    if brname in var_names:
                        new_var_names.append( brname )
                for name in var_names:
                    if name not in new_var_names:
                        var_names.remove( name )
            self.BookVariables( *var_names )
            return
        if len( self.Targets ):
            truevars = []
            tlist    = self.GetListOfTrees()
            for name in var_names:
                if name not in self.Variables:
                    truevars.append( name )
                else:
                    print "Variable", name, "already booked"
            dictlist       = [ DictFromTree( tree, truevars ) for tree in tlist ]
            self.Variables = JoinDicts( self.Variables, MergeDicts( *dictlist ) )
            self.Nentries  = len( self.Variables[ name ] )
        else:
            print "No targets added to the manager, could not book variables:", var_names
            exit()

    def Copy( self, *args, **kwargs ):
        ''' Returns a copy of this class that does not own the targets. '''
        if "cuts" in kwargs: cmngr = self.CutSample( kwargs[ "cuts" ] )
        else: cmngr = self
        if args: variables = args
        else: variables = cmngr.Variables.keys()
        mngr = DataManager()
        for v in variables:
            mngr.Variables[ v ] = deepcopy( cmngr.Variables[ v ] )
        mngr.Nentries = cmngr.Nentries
        return mngr

    def CutSample( self, cut ):
        ''' Returns another DataManager based on the events that satisfy the cuts given.
        The targets are not transfered to the new class. '''
        mngr     = DataManager()
        add_list = self.GetCutList( cut )
        for kwvar in self.Variables:
            vlist = self.Variables[ kwvar ]
            mngr.Variables[ kwvar ] = [ vlist[ i ] for i in add_list ]
        mngr.Nentries = len( add_list )
        return mngr

    def CloseFiles( self ):
        ''' Closes all the target files if they are owned by the class '''
        if self.OwnsTargets:
            for ifile in self.Targets:
                for itree in self.Targets[ ifile ]:
                    itree = 0
                ifile.Close()
                del self.Targets[ ifile ]
        else:
            print "This DataManager does not own his targets"

    def GetCutList( self, cut ):
        ''' This method allows to obtain a list with the events that satisfy the cuts
        given '''
        cut       = cut.replace( "&&", " and " )
        cut       = cut.replace( "||", " or "  )
        cut       = cut.replace( "abs", "fabs" )
        variables = cut
        for el in [ '==', '!=', '<=', '>=', '>', '<',
                    'and', 'or', '(', ')',
                    "+", "-", "*", "/" ]:
            variables = variables.replace( el, '|' )
        variables = variables.replace( ' ', '' )
        variables = variables.split( '|' )
        while '' in variables:
            variables.remove( '' )
        var_list = []
        flist = dir( math )
        for el in variables:
            if el not in flist:
                try:
                    float( el )
                except:
                    if el in self.Variables:
                        var_list.append( el )
                    else:
                        print "Cut in variable", el, "not valid, variable does not exist"
            else:
                cut = cut.replace( el, "math." + el )
        ''' Sorting the list on a reversed way is necessary to prevent missreplacement of
        the variables '''
        var_list.sort()
        var_list.reverse()
        values = [ self.Variables[ var ] for var in var_list ]
        nvars  = len( var_list )
        for ivar in xrange( nvars ):
            cut = cut.replace( var_list[ ivar ], "values[ %i ][ ievt ]" %ivar )
        return eval( "[ ievt for ievt in xrange( self.Nentries ) if " + cut + " ]" )

    def GetEntries( self, selection = False ):
        ''' Gets the number of entries of the class. If a cut selection is given, it is
        calculated the number of events that satisfy those cuts. '''
        if selection:
            return len( self.GetCutList( selection ) )
        else:
            return self.Nentries

    def GetEventDict( self, ievt ):
        ''' Returns a dictionary with the values of the variables at the given entry '''
        return dict( ( var, self.Variables[ var ][ ievt ] ) for var in self.Variables )

    def GetEvent( self, ievt, *args ):
        ''' Gets the event at position ievt. Allows to get only the variables in < args >
        in the order specified. '''
        if len( args ):
            return tuple( [ self.Variables[ var ][ ievt ] for var in args ] )
        else:
            return tuple( [ self.Variables[ var ][ ievt ] for var in self.Variables ] )

    def GetListOfTrees( self ):
        ''' Returns a list with the trees attached to the class '''
        tlist   = []
        rawlist = [ self.Targets[ ifile ] for ifile in self.Targets ]
        for el in rawlist:
            tlist += el
        return tlist

    def GetMatrix( self, *args, **kwargs ):
        ''' Returns a matrix containing the values of the variables specified in < args >.
        If the option < EvtsInRows > is set to False, the matrix will be created as a list
        containing a list for each of the variables. Otherwise the matrix will be a list
        containing a list for each event, with the variables same order as in < args >. If
        '*' appears in < args >, there will be taken all the variables in the manager
        ordered by name '''
        if "EvtsInRows" in kwargs: trans = kwargs[ "EvtsInRows" ]
        else: trans = True
        if "*" in args:
            args = self.Variables.keys()
            args.sort()
        matrix = Matrix( [ self.Variables[ var ] for var in args ] )
        if trans:
            return matrix.Transpose()
        else:
            return matrix

    def GetNvars( self ):
        ''' Gets the number of variables in the class '''
        return len( self.Variables )

    def GetTargets( self ):
        ''' Gets the targets directory of the class. If this method is called, the class
        automatically frees the targets, so their destruction are left to the new owner. '''
        self.OwnsTargets = False
        return self.Targets

    def GetVarEvents( self, variable, cut = False ):
        ''' Gets the list of values for a given variable. If a cut is specified it returns
        the values concerning the sample which satisfies them. '''
        values = self.Variables[ variable ]
        if cut:
            return [ values[ ievt ] for ievt in self.GetCutList( cut ) ]
        else:
            return [ values[ ievt ] for ievt in xrange( self.Nentries ) ]

    def GetVarNames( self ):
        ''' Gets the name of the variables in the class '''
        return [ var for var in self.Variables ]

    def MakeHistogram( self, var, nbins = 100, **kwargs ):
        ''' Makes the histogram of the given variable. A selection can be applied
        introducing < cuts >, as well as the name and the title can be defined in a
        similar way too. '''
        if "name"  not in kwargs: kwargs[ "name"  ] = var
        if "title" not in kwargs: kwargs[ "title" ] = var
        if "cuts" in kwargs: vvar = self.GetVarEvents( var, kwargs[ "cuts" ] )
        else: vvar = self.Variables[ var ]
        return MakeHistogram( vvar, nbins, **kwargs )

    def MakeHistogram2D( self, xvar, yvar, wvar = False, **kwargs ):
        ''' Makes the 2-dimensional histogram of the given variables '''
        if "name"   not in kwargs: kwargs[ "name"   ] = xvar + "vs" + yvar
        if "title"  not in kwargs: kwargs[ "title"  ] = xvar + "vs" + yvar
        if "xtitle" not in kwargs: kwargs[ "xtitle" ] = xvar
        if "ytitle" not in kwargs: kwargs[ "ytitle" ] = yvar
        vwvar = False
        if "cuts" in kwargs:
            if wvar:
                vwvar = self.GetVarEvents( wvar, kwargs[ "cuts" ] )
            vxvar = self.GetVarEvents( xvar, kwargs[ "cuts" ] )
            vyvar = self.GetVarEvents( yvar, kwargs[ "cuts" ] )
        else:
            if wvar:
                vwvar = self.Variables[ wvar ]
            vxvar = self.Variables[ xvar ]
            vyvar = self.Variables[ yvar ]
        return MakeHistogram2D( vxvar, vyvar, vwvar, **kwargs )

    def MakeScatterPlot( self, xvar, yvar, **kwargs ):
        ''' Creates a graph object with the points corresponding to two variables '''
        if "name"   not in kwargs: kwargs[ "name"   ] = xvar + "vs" + yvar
        if "title"  not in kwargs: kwargs[ "title"  ] = xvar + "vs" + yvar
        if "xtitle" not in kwargs: kwargs[ "xtitle" ] = xvar
        if "ytitle" not in kwargs: kwargs[ "ytitle" ] = yvar
        if "cuts" in kwargs:
            vxvar = self.GetVarEvents( xvar, kwargs[ "cuts" ] )
            vyvar = self.GetVarEvents( yvar, kwargs[ "cuts" ] )
        else:
            vxvar = self.Variables[ xvar ]
            vyvar = self.Variables[ yvar ]
        return MakeScatterPlot( vxvar, vyvar, **kwargs )

    def MakeVariable( self, varname, arglist, function ):
        ''' Makes another variable using those in the class. There have to be specified:
        the new variable name, the name of the variables used by the function
        ( ordered in a list ) and the function itself. The computation of the new
        variable is going to be performed passing the function the variables as normal
        entries ( *args, where args is the list of values ). '''
        new_variable = self.Nentries*[ 0. ]
        var_tensor   = [ self.Variables[ vname ] for vname in arglist ]
        lvars        = range( len( arglist ) )
        for ievt in xrange( self.Nentries ):
            values               = [ var_tensor[ ivar ][ ievt ] for ivar in lvars ]
            new_variable[ ievt ] = function( *values )
        self.Variables[ varname ] = new_variable

    def NewEvent( self, dic ):
        ''' Adds a new event to the manager. A value for all the variables has to be
        provided. '''
        for key in self.Variables:
            self.Variables[ key ].append( dic[ key ] )
        self.Nentries += 1

    def Print( self, *args, **kwargs ):
        ''' Prints the information of the class as well as the values for the first 20
        events. If < events > is introduced as an input, the number of events showed
        would be that specified by the user. If < cut > is specified only will be
        showed the events that statisfy the given cut. If < name > is specified this
        name is going to be printed before the other information. '''

        if "cut" not in kwargs:
            kwargs[ "cut" ] = False
        if "events" not in kwargs:
            kwargs[ "events" ] = False
        if "name" not in kwargs:
            if hasattr( self, "name" ):
                kwargs[ "name" ] = self.name
            else:
                kwargs[ "name" ] = "Manager"
        if "variables" not in kwargs:
            kwargs[ "variables" ] = [ var for var in self.Variables ]
            kwargs[ "variables" ].sort()

        max_length = 0
        for var in kwargs[ "variables" ]:
            lvar = len( var )
            if lvar > max_length:
                max_length = lvar

        ''' Prints the name of the manager '''
        lname = len( kwargs[ "name" ] )
        print "\n" + 3*lname*'*' + "\n" + lname*" " + kwargs[ "name" ] + "\n" + 3*lname*'*'
        
        ''' Prints the targets '''
        if len( self.Targets ):
            print "\nFiles attached:"
            for ifile in self.Targets:
                out = " - " + ifile.GetName() + ": "
                for tree in self.Targets[ ifile ]:
                    out += tree.GetName() + ", "
                print out[ :-2 ]

        ''' Prints the variables '''
        if self.Variables:
            vout      = " "
            variables = []
            if len( args ):
                variables = args
                for var in args:
                    vout      += var + ", "
            else:
                for var in kwargs[ "variables" ]:
                    variables.append( var )
                    vout      += var + ", "
            print "\nVariables:" + vout[ :-2 ] + "\n"

            ''' Prints the values of the variables '''
            vout = " "
            for var in variables:
                vout += "%.3e" + "\t|\t"
            vout = vout[ :-2 ]

            if kwargs[ "cut" ]:
                evlist = self.GetCutList( kwargs[ "cut" ] )
            else:
                evlist = range( self.Nentries )

            if kwargs[ "events" ]:
                i = 0
                for ievt in evlist:
                    if i == kwargs[ "events" ]: break
                    i += 1
                    print vout % self.GetEvent( ievt, *variables )
            else:
                for ievt in evlist:
                    if ievt and ievt % 20 == 0:
                        if raw_input(
                            "< Introduce q to exit, and any other input to continue printing >: "
                            ) == "q": break
                    print vout % self.GetEvent( ievt, *variables )
        else:
            print "No variables booked in this manager"

    def RemoveVariable( self, var ):
        ''' Removes a booked variable '''
        del self.Variables[ var ]

    def Save( self, name, tree_name = False, **kwargs ):
        ''' Saves the given class values in a TTree. If only < name > is defined, it is
        considered as the tree name, so it is written in the external directory ( to
        be constructed and accesible in the main program ), if two names are provided
        the first one is considered as the file name and the second the tree name. If
        < close > is provided, and if its value is false, this method will return
        the output file. If in < kwargs > the < fileType > key is set to "txt", then the
        output will be considered as a txt where the columns correspond to each variable in
        alphabetical order. In any case the variables to be stored can be specified using
        the keyword < variables >, providing a list with them. '''
        if "fileType" in kwargs: fileType = kwargs[ "fileType" ]
        else: fileType = "root"
        if "variables" in kwargs: variables = kwargs[ "variables" ]
        else: variables = self.Variables.keys()
        if "close" in kwargs: close = kwargs[ "close" ]
        else: close = True
        if fileType in ( "root", "Root", "ROOT" ):
            if tree_name:
                ofile = TFile.Open( name, "RECREATE" )
                print "Saving tree with name <", tree_name, "> in <", name, ">"
                TreeFromDict( tree_name, self.Variables )
            else:
                print ( "Saving tree with name <", name, "> in <", gDirectory.GetName(), ">" )
                TreeFromDict( name, self.Variables )
            print "Written", self.Nentries, "entries"
            if close: ofile.Close()
            else: return ofile
        elif fileType in ( "txt", "TXT" ):
            ofile = open( name, "wt" )
            print "Saving txt data in file <", name, ">"
            varvalues = [ self.Variables[ var ] for var in variables ]
            for ievt in xrange( self.Nentries ):
                out = ""
                for var in varvalues:
                    out += str( var[ ievt ] ) + "\t"
                ofile.write( out[ :-2 ] + "\n" )
            if close: ofile.close()
            else: return ofile

#_______________________________________________________________________________
# If the input is a string, returns an array with values of a certain type
# depending on the identifier located in the title. If the input is a list, it
# finds the type of values located on it, and returns the apropiated array.
def ArrayType( branch ):
    if isinstance( branch, str ):
        if   "/F" in branch:
            return array( 'f', [ 0 ] )
        elif "/D" in branch:
            return array( 'd', [ 0 ] )
        elif "/I" in branch:
            return array( 'i', [ 0 ] )
        elif "/O" in branch:
            return array( 'b', [ 0 ] )
        else:
            print "Type not found in <", branch, ">"
            exit( 0 )
    else:
        if   isinstance( branch[ 0 ], float ):
            return array( 'd', [ 0 ] )
        elif isinstance( branch[ 0 ], int ):
            return array( 'i', [ 0 ] )
        elif isinstance( branch[ 0 ], bool ):
            return array( 'b', [ 0 ] )
        else:
            print "Could not extract the type in <", branch[ 0 ], ">"
            exit( 0 )
    
#_______________________________________________________________________________
# This function creates a new branch in the given tree using the values stored
# in a list
def BranchFromList( brname, tree, lst ):
    if len( lst ) != tree.GetEntries():
        print "The size of the input list and the tree are not the same"
        return 0
    var    = ArrayType( lst )
    branch = tree.Branch( brname, var, brname + '/' + var.typecode.upper() )
    for ievt in xrange( tree.GetEntries() ):
        tree.GetEntry( ievt )
        var[ 0 ] = lst[ ievt ]
        branch.Fill()
    branch.Write()

#_______________________________________________________________________________
# Creates a new dictionary containing the values of the variables stored in a
# TTree object. The input is composed by the tree and the variables to be
# stored ( given in a list ).
def DictFromTree( tree, varlist ):
    avars, tvals = [], []
    for var in varlist:
        avars.append( ArrayType( tree.GetBranch( var ).GetTitle() ) )
        tvals.append( [] )
        tree.SetBranchAddress( var, avars[ -1 ] )
    rvars = range( len( varlist ) )
    for ievt in xrange( tree.GetEntries() ):
        tree.GetEntry( ievt )
        for i in rvars:
            tvals[ i ].append( avars[ i ][ 0 ] )
    tree.ResetBranchAddresses()
    return dict( ( var, tvals[ i ] ) for var, i in zip( varlist, rvars ) )

#_______________________________________________________________________________
# This function almacenates the values of a leaf in a TTree into a list, given
# the tree and the branch name
def ListFromBranch( brname, tree ):
    branch  = tree.GetBranch( brname )
    var     = ArrayType( branch.GetTitle() )
    tree.SetBranchAddress( brname, var )
    lst = []
    for ievt in xrange( tree.GetEntries() ):
        tree.GetEntry( ievt )
        lst.append( var[ 0 ] )
    tree.ResetBranchAddresses()
    return lst

#_______________________________________________________________________________
# Creates a new tree with the lists stored in a dictionary. The name of the
# branches are given by the keys in the dictionary. The names are sorted.
def TreeFromDict( name, dic, **kwargs ):
    if "level" not in kwargs: kwargs[ "level" ] = 0
    if "title" not in kwargs: kwargs[ "title" ] = name
    tree      = TTree( name, kwargs[ "title" ], kwargs[ "level" ] )
    variables = [ key for key in dic ]
    variables.sort()
    avars, tvals = [], []
    for var in variables:
        avars.append( ArrayType( dic[ var ] ) )
        tvals.append( dic[ var ] )
        tree.Branch( var, avars[ -1 ], var + '/' + avars[ -1 ].typecode.upper() )
    rvars = range( len( tvals ) )
    for ievt in xrange( len( dic[ var ] ) ):
        for i in rvars:
            avars[ i ][ 0 ] = tvals[ i ][ ievt ]
        tree.Fill()
        if ievt % 100000 == 0:
            tree.AutoSave()
    tree.AutoSave()
    return tree
