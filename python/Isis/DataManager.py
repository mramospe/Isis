#/////////////////////////////////////////////////////////////
#//                                                         //
#//  Python modules                                         //
#//                                                         //
#// ------------------------------------------------------- //
#//                                                         //
#//  AUTHOR: Miguel Ramos Pernas                            //
#//  e-mail: miguel.ramos.pernas@cern.ch                    //
#//                                                         //
#//  Last update: 16/11/2015                                //
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
from Isis.Utils import JoinDicts, MergeDicts
from Isis.PlotTools import MakeHistogram, MakeHistogram2D, MakeScatterPlot


#_______________________________________________________________________________
# Class to manage data, specially designed to work together with Root files
# and trees
class DataManager:

    def __init__( self, *args, **kwargs ):
        ''' Constructor given a name for the manager and arguments. The variable
        < args > has to contain the name of the input file and the trees inside.
        Attributes can be added as well.'''

        for kw in kwargs:
            setattr( self, kw, kwargs[ kw ] )

        nargs = len( args )

        self.Iterator    = 0
        self.Nentries    = 0
        self.OwnsTargets = False
        self.Targets     = {}
        self.Variables   = {}

        ''' Error output for bad construction '''
        if nargs >= 2:
            self.AddTarget( *args )
        elif nargs == 1:
            print "Arguments for DataManager class are file_name and tree_name"
            exit()

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

    def AddTarget( self, *args ):
        ''' Adds a new target file and/or tree to the class '''
        if not self.OwnsTargets: self.OwnsTargets = True
        if args[ 0 ] not in [ ifile.GetName() for ifile in self.Targets ]:
            ifile = TFile.Open( args[ 0 ], "READ" )
            tlist = [ ifile.Get( tname ) for tname in args[ 1: ] ]
            self.Targets[ ifile ]  = tlist
            print "Added target <", args[ 0 ], "> and trees:", list( args[ 1: ] )
        else:
            ifile = [ ifile for ifile in self.Targets if ifile.GetName() == args[ 0 ] ][ 0 ]
            tlist = [ ifile.Get( tname ) for tname in args[ 1: ] ]
            self.Targets[ ifile ] += tlist
            print "Added trees: ", list( tlist ), "to target <", ifile.GetName(), ">"
        for name in self.Variables:
            vvals = self.Variables[ name ]
            for tree in tlist:
                for ievt in range( tree.GetEntries() ):
                    tree.GetEntry( ievt )
                    vvals.append( getattr( tree, name ) )
        if not self.Nentries and self.Variables:
            self.Nentries = len( vvals )

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
            var_names = [ brlist.At( i ).GetName() for i in range( brlist.GetSize() ) ]
            for tree in tlist[ 1: ]:
                brlist        = tree.GetListOfBranches()
                new_var_names = []
                for i in range( brlist.GetSize() ):
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
            dictlist = []
            tlist    = self.GetListOfTrees()
            for name in var_names:
                if name not in self.Variables:
                    truevars.append( name )
                else:
                    print "Variable", name, "already booked"
            for tree in tlist:
                dictlist.append( DictFromTree( tree, *truevars ) )
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
        ''' Closes all the target files owned by the class '''
        for ifile in self.Targets:
            for itree in self.Targets[ ifile ]:
                itree = 0
            ifile.Close()
            del self.Targets[ ifile ]

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
        for ivar in range( nvars ):
            cut = cut.replace( var_list[ ivar ], "values[ %i ][ ievt ]" %ivar )
        return eval( "[ ievt for ievt in range( self.Nentries ) if " + cut + " ]" )

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
        res_list = []
        if cut:
            for ievt in self.GetCutList( cut ):
                res_list.append( self.Variables[ variable ][ ievt ] )
        else:
            for ievt in range( self.Nentries ):
                res_list.append( self.Variables[ variable ][ ievt ] )
        return res_list

    def GetVarNames( self ):
        ''' Gets the name of the variables in the class '''
        return [ var for var in self.Variables ]

    def MakeHistogram( self, var, nbins = 100, **kwargs ):
        ''' Makes the histogram of the given variable. A selection can be applied
        introducing < cuts >, as well as the name and the title can be defined in a
        similar way too. '''
        if "name" in kwargs: name = kwargs[ "name" ]
        else: name = var
        if "title" in kwargs: title = kwargs[ "title" ]
        else: title = var
        if "cuts" in kwargs: vvar = self.GetVarEvents( var, kwargs[ "cuts" ] )
        else: vvar = self.Variables[ var ]
        return MakeHistogram( vvar, nbins, **kwargs )

    def MakeHistogram2D( self, xvar, yvar, wvar = False, **kwargs ):
        ''' Makes the 2-dimensional histogram of the given variables '''
        if "name" in kwargs: name = kwargs[ "name" ]
        else: kwargs[ "name" ] = xvar + "vs" + yvar
        if "title" in kwargs: title = kwargs[ "title" ]
        else: kwargs[ "title" ] = xvar + "vs" + yvar
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
        if "xbins" in kwargs: xbins = kwargs[ "xbins" ]
        else: xbins = 100
        if "ybins" in kwargs: ybins = kwargs[ "ybins" ]
        else: ybins = 100
        if "xmax" in kwargs: xmax = kwargs[ "xmax" ]
        else: xmax = min( xvar )
        if "ymax" in kwargs: ymax = kwargs[ "ymax" ]
        else: ymax = min( yvar )
        if "xmin" in kwargs: xmin = kwargs[ "xmin" ]
        else: xmin = min( xvar )
        if "ymin" in kwargs: ymin = kwargs[ "ymin" ]
        else: ymin = min( yvar )
        if "vtype" in kwargs: tp = kwargs[ "vtype" ]
        else: tp = "double"
        if "xtitle" not in kwargs: kwargs[ "xtitle" ] = xvar
        if "ytitle" not in kwargs: kwargs[ "ytitle" ] = yvar
        return MakeHistogram2D( vxvar, vyvar, vwvar, **kwargs )

    def MakeScatterPlot( self, xvar, yvar, **kwargs ):
        ''' Creates a graph object with the points corresponding to two variables '''
        if "name" in kwargs: name = kwargs[ "name" ]
        else: kwargs[ "name" ] = xvar + "vs" + yvar
        if "title" in kwargs: title = kwargs[ "title" ]
        else: kwargs[ "title" ] = xvar + "vs" + yvar
        if "cuts" in kwargs:
            vxvar = self.GetVarEvents( xvar, kwargs[ "cuts" ] )
            vyvar = self.GetVarEvents( yvar, kwargs[ "cuts" ] )
        else:
            vxvar = self.Variables[ xvar ]
            vyvar = self.Variables[ yvar ]
        if "xtitle" not in kwargs: kwargs[ "xtitle" ] = xvar
        if "ytitle" not in kwargs: kwargs[ "ytitle" ] = yvar
        return MakeScatterPlot( vxvar, vyvar, **kwargs )

    def MakeVariable( self, var_name, arg_list, function ):
        ''' Makes another variable using those in the class. There have to be specified:
        the new variable name, the name of the variables used by the function
        ( ordered in a list ) and the function itself. The computation of the new
        variable is going to be performed passing the function the variables as normal
        entries ( *args, where args is the list of values ). '''
        new_variable = self.Nentries*[ 0. ]
        var_tensor   = [ self.Variables[ vname ] for vname in arg_list ]

        lvars = range( len( arg_list ) )
        for ievt in range( self.Nentries ):
            values               = [ var_tensor[ ivar ][ ievt ] for ivar in lvars ]
            new_variable[ ievt ] = function( *values )

        self.Variables[ var_name ] = new_variable

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

    def Save( self, *args, **kwargs ):
        ''' Saves the given class values in a TTree. If args has only one name, it is
        considered as the tree name, so its written in the external directory ( to
        be constructed and accesible in the main program ), if two names are provided
        the first one is considered as the file name and the second the tree name. If
        < close > is provided, and if its value is false, this method will return
        the output file.  '''
        if len( args ) == 2:
            output_file = TFile.Open( args[ 0 ], "RECREATE" )
            print "Saving tree with name <", args[ 1 ], "> in <", args[ 0 ], ">"
            TreeFromDict( args[ 1 ], self.Variables )
        else:
            print ( "Saving tree with name <", args[ 1 ], "> in <", gDirectory.GetName(), ">" )
            TreeFromDict( args[ 0 ], self.Variables )
        print "Written", self.Nentries, "events"
        if "close" in kwargs:
            if kwargs[ "close" ]:
                output_file.Close()
            else:
                return output_file
        else:
            output_file.Close()

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
    for ievt in range( tree.GetEntries() ):
        tree.GetEntry( ievt )
        var[ 0 ] = lst[ ievt ]
        branch.Fill()
    branch.Write()

#_______________________________________________________________________________
# Creates a new dictionary containing the values of the variables stored in a
# TTree object. The input is composed by the tree and the variables to be
# stored.
def DictFromTree( tree, *args ):
    avars = []
    tvals = []
    for var in args:
        avars.append( ArrayType( tree.GetBranch( var ).GetTitle() ) )
        tvals.append( [] )
        tree.SetBranchAddress( var, avars[ -1 ] )
    rvars = range( len( args ) )
    for ievt in range( tree.GetEntries() ):
        tree.GetEntry( ievt )
        for i in rvars:
            tvals[ i ].append( avars[ i ][ 0 ] )
    tree.ResetBranchAddresses()
    dic = {}
    for var, i in zip( args, rvars ):
        dic[ var ] = tvals[ i ]
    return dic

#_______________________________________________________________________________
# This function almacenates the values of a leaf in a TTree into a list, given
# the tree and the branch name
def ListFromBranch( brname, tree ):
    branch  = tree.GetBranch( brname )
    var     = ArrayType( branch.GetTitle() )
    tree.SetBranchAddress( brname, var )
    lst = []
    for ievt in range( tree.GetEntries() ):
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
    avars = []
    tvals = []
    for var in variables:
        avars.append( ArrayType( dic[ var ] ) )
        tvals.append( dic[ var ] )
        tree.Branch( var, avars[ -1 ], var + '/' + avars[ -1 ].typecode.upper() )
    rvars = range( len( tvals ) )
    for ievt in range( len( dic[ var ] ) ):
        for i in rvars:
            avars[ i ] = tvals[ i ][ ievt ]
        tree.Fill()
        if ievt % 100000 == 0:
            tree.AutoSave()
    tree.AutoSave()
    return tree
