#/////////////////////////////////////////////////////////////
#//                                                         //
#//  Python modules                                         //
#//                                                         //
#// ------------------------------------------------------- //
#//                                                         //
#//  AUTHOR: Miguel Ramos Pernas                            //
#//  e-mail: miguel.ramos.pernas@cern.ch                    //
#//                                                         //
#//  Last update: 08/04/2016                                //
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
from Isis.Utils import FormatEvalExpr, JoinDicts, MergeDicts


#_______________________________________________________________________________
# Class to manage data, specially designed to work together with Root files
# and trees
class DataManager:

    def __init__( self, name = False, ifile = False, tnames = [], **kwargs ):
        ''' The constructor works in three different ways. If < ifile > is a dictionary,
        the current variables in the class are set to those stored in it. If < ifile >
        is a string, by default it is considered that the input file is a Root file,
        and < tnames > has to be the list of TTree names in the file. In < kwargs >, you
        can specify the file type ( ftype = 'root'/'txt' ). If it is set to 'txt', then
        < tnames > could be a list containing the names of the different variables or,
        if it is left in blanck, it will take the names of the variables situated in the
        first line of the file. The number of variables specified should match the number
        of columns in the file. Otherwise one has to specify via < colidx = [ 1, 6, ... ] >
        the index for the columns to be stored. Other attributes can be added too. '''

        ''' These are the main attributes that this class has '''
        self.Iterator    = 0
        self.Name        = ''
        self.Nentries    = 0
        self.OwnsTargets = False
        self.Targets     = {}
        self.Variables   = {}

        ''' The constructor starts here '''
        if name:
            self.Name = name
            ''' First check if < ifile > is a string or if it can be used as a dictionary'''
            if isinstance( ifile, str ):

                if 'ftype' in kwargs:
                    ftype = kwargs[ 'ftype' ]
                    del kwargs[ 'ftype' ]
                else:
                    ftype = 'root'

                if ftype in ( 'root', 'Root', 'ROOT' ):
                    ''' This is the constructor for Root files '''
                    if ifile and tnames:
                        self.AddTarget( ifile, tnames )
                    elif ifile and not tnames:
                        print 'WARNING: Arguments for DataManager class using root files are < name > < file path > and  < tree name >'

                elif ftype in ( 'txt', 'TXT' ):
                    ''' This is the constructor for txt files '''
                    if ifile and tnames:
                        self.StoreTxtData( ifile, tnames, **kwargs )
                    elif ifile and not tnames:
                        print 'WARNING: Arguments for DataManager class using txt files are < file path > and < variables names >'
                
                else:
                    ''' If the type specified is not recognised a warning message is sent '''
                    print 'WARNING: File format <', ftype, '> for DataManager class not known'
            
            else:
                ''' This is the constructor using a dictionary (the ftype value is omitted) '''
                self.AddDataFromDict( ifile )

        for kw in kwargs:
            setattr( self, kw, kwargs[ kw ] )            

    def __add__( self, other ):
        ''' Allows merging two objects of this class. The new manager owns all the targets. '''
        mngr          = DataManager()
        mngr.Nentries = self.Nentries + other.Nentries
        mngr.Targets.update( self.Targets  )
        mngr.Targets.update( other.Targets )
        true_vars = [ var for var in self.Variables if var in other.Variables ]
        for var in true_vars:
            mngr.Variables[ var ] = self.Variables[ var ] + other.Variables[ var ]
        self.OwnsTargets  = False
        other.OwnsTargets = False
        mngr.Name = self.Name + '+' + other.Name
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

    def AddDataFromDict( self, dict2add ):
        ''' Adds data from a dictionary of lists. First checks that all the lists in the
        dictionary have the same size. Then a check is made to see if some variables are
        present in the current manager. If so, all of them have to be present. '''
        lgth = len( dict2add.itervalues().next() )
        if not all( len( values ) == lgth for kw, values in dict2add.iteritems() ):
            print 'WARNING:', self.Name, '=> The lists to add have not got the same size. Data not merged.'
            return
        if self.Nentries:
            if lgth != self.Nentries:
                print 'WARNING:', self.Name, '=> The length of the data to merge does not match the size of the manager. Data not merged.'
                return
            if all( el in self.Variables for el in dict2add ):
                for el, values in dict2add.iteritems():
                    self.Variables[ el ] += values
                    self.Nentries        += lgth
            for kw, values in dict2add.iteritems():
                if kw in self.Variables:
                    print 'WARNING:', self.Name, '=> Variable <', kw, '> already in the manager; not stored'
                else:
                    self.Variables[ kw ] = values
        else:
            self.Variables = dict2add
            self.Nentries  = lgth

    def AddTarget( self, fname, tnames ):
        ''' Adds a new target file and/or tree to the class. One has to provide the file
        name and the tree names. '''
        if not self.OwnsTargets: self.OwnsTargets = True
        if fname not in [ ifile.GetName() for ifile in self.Targets ]:
            ifile = TFile.Open( fname, 'READ' )
            tlist = [ ifile.Get( tname ) for tname in tnames ]
            self.Targets[ ifile ]  = tlist
            print self.Name, '=> Added target <', fname, '> and trees:', tnames
        else:
            ifile = [ ifile for ifile in self.Targets if ifile.GetName() == fname ][ 0 ]
            tlist = [ ifile.Get( tname ) for tname in tnames ]
            self.Targets[ ifile ] += tlist
            print self.Name, '=> Added trees: ', list( tlist ), 'from target <', ifile.GetName(), '>'
        if self.Variables:
            dictlist = [ DictFromTree( tree, self.Variables.keys() ) for tree in tlist ]
            self.Variables.update( MergeDicts( *dictlist ) )
            self.Nentries = len( self.Variables[ self.Variables.keys()[ 0 ] ] )
            
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
                    print 'WARNING:', self.Name, '=> Variable <', name, '> already booked'
            dictlist       = [ DictFromTree( tree, truevars ) for tree in tlist ]
            self.Variables.update( MergeDicts( *dictlist ) )
            self.Nentries  = len( self.Variables[ name ] )
        else:
            print 'WARNING: No targets added to the manager, could not book variables:', var_names

    def Copy( self, *args, **kwargs ):
        ''' Returns a copy of this class that does not own the targets. '''
        if 'cuts' in kwargs: cmngr = self.CutSample( kwargs[ 'cuts' ] )
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
        for kw, vlist in self.Variables.iteritems():
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
            print 'WARNING: This DataManager does not own its targets'

    def GetCutList( self, cut ):
        ''' This method allows to obtain a list with the events that satisfy the cuts
        given '''
        cut, variables = FormatEvalExpr( cut, math )
        values = [ self.Variables[ var ] for var in variables ]
        for ivar in xrange( len( variables ) ):
            cut = cut.replace( variables[ ivar ], 'values[ %i ][ ievt ]' %ivar )
        return eval( '[ ievt for ievt in xrange( self.Nentries ) if ' + cut + ' ]' )

    def GetEntries( self, selection = False ):
        ''' Gets the number of entries of the class. If a cut selection is given, it is
        calculated the number of events that satisfy those cuts. '''
        if selection:
            return len( self.GetCutList( selection ) )
        else:
            return self.Nentries

    def GetEventDict( self, ievt ):
        ''' Returns a dictionary with the values of the variables at the given entry '''
        return dict( ( var, values[ ievt ] ) for var, values in self.Variables.iteritems() )

    def GetEventTuple( self, ievt, *args ):
        ''' Gets the event at position ievt. Allows to get only the variables in < args >
        in the order specified. '''
        if len( args ):
            return tuple( [ self.Variables[ var ][ ievt ] for var in args ] )
        else:
            return tuple( [ values[ ievt ] for var, values in self.Variables.iteritems() ] )

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
        if 'EvtsInRows' in kwargs: trans = kwargs[ 'EvtsInRows' ]
        else: trans = True
        if '*' in args:
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

    def GetVarEvents( self, arg, cut = False ):
        ''' If < arg > is a variable, it gets the list of values for it. If it is an expression,
        it returns a list with the values corresponding to it. '''
        if cut:
            entries = self.GetCutList( cut )
        else:
            entries = xrange( self.Nentries )
        if arg in self.Variables:
            values = self.Variables[ arg ]
            return [ values[ ievt ] for ievt in entries ]
        else:
            arg, variables = FormatEvalExpr( arg, math )
            values = [ self.Variables[ var ] for var in variables ]
            nvars  = len( variables )
            for ivar in xrange( nvars ):
                arg = arg.replace( variables[ ivar ], 'values[ %i ][ ievt ]' %ivar )
            return eval( '[ %s for ievt in entries ]' %arg )

    def GetVarNames( self ):
        ''' Gets the name of the variables in the class '''
        return self.Variables.keys()

    def MakeHistogram( self, var, wvar = False, **kwargs ):
        ''' Makes the histogram of the given variable. A selection can be applied
        introducing < cuts >, as well as the name and the title can be defined in a
        similar way too. '''
        if 'name'  not in kwargs:  kwargs[ 'name'  ]  = var
        if 'title' not in kwargs:  kwargs[ 'title' ]  = var
        if 'xtitle' not in kwargs: kwargs[ 'xtitle' ] = var
        if 'cuts' in kwargs: cuts = kwargs[ 'cuts' ]
        else: cuts = False
        var = self.GetVarEvents( var, cuts )
        if wvar:
            wvar = self.GetVarEvents( wvar, cuts )
        return MakeHistogram( var, wvar, **kwargs )

    def MakeHistogram2D( self, xvar, yvar, wvar = False, **kwargs ):
        ''' Makes the 2-dimensional histogram of the given variables '''
        if 'name'   not in kwargs: kwargs[ 'name'   ] = xvar + 'vs' + yvar
        if 'title'  not in kwargs: kwargs[ 'title'  ] = xvar + 'vs' + yvar
        if 'xtitle' not in kwargs: kwargs[ 'xtitle' ] = xvar
        if 'ytitle' not in kwargs: kwargs[ 'ytitle' ] = yvar
        if 'cuts' in kwargs: cuts = kwargs[ 'cuts' ]
        else: cuts = False
        if wvar:
            wvar = self.GetVarEvents( wvar, cuts )
        xvar = self.GetVarEvents( xvar, cuts )
        yvar = self.GetVarEvents( yvar, cuts )
        return MakeHistogram2D( xvar, yvar, wvar, **kwargs )

    def MakeScatterPlot( self, xvar, yvar, xerr = False, yerr = False, **kwargs ):
        ''' Creates a graph object with the points corresponding to two variables '''
        if 'name'   not in kwargs: kwargs[ 'name'   ] = xvar + 'vs' + yvar
        if 'title'  not in kwargs: kwargs[ 'title'  ] = xvar + 'vs' + yvar
        if 'xtitle' not in kwargs: kwargs[ 'xtitle' ] = xvar
        if 'ytitle' not in kwargs: kwargs[ 'ytitle' ] = yvar
        if 'cuts' in kwargs: cuts = kwargs[ 'cuts' ]
        else: cuts = False
        xvar = self.GetVarEvents( xvar, cuts )
        yvar = self.GetVarEvents( yvar, cuts )
        if xerr: xerr = self.GetVarEvents( xerr, cuts )
        if yerr: yerr = self.GetVarEvents( yerr, cuts )
        return MakeScatterPlot( xvar, yvar, xerr, yerr, **kwargs )

    def MakeVariable( self, varname, arg, function = False ):
        ''' Makes another variable using those in the class. There are two different
        ways to do it. The first one consists on specifying the new variable name,
        the name of the variables used by the function ( ordered in a list ) and the
        function itself. The computation of the new variable is going to be performed
        passing the variables to the function as normal entries ( *args, where args is
        the list of values ). The second method consists on specifying only the name
        of the variable and an expression in < arg >. The values will be processed
        then for each entry taking into account the value obtained when evaluating the
        expression. '''
        if function:
            new_variable = self.Nentries*[ 0. ]
            var_tensor   = [ self.Variables[ vname ] for vname in arg ]
            lvars        = xrange( len( arg ) )
            for ievt in xrange( self.Nentries ):
                values               = [ var_tensor[ ivar ][ ievt ] for ivar in lvars ]
                new_variable[ ievt ] = function( *values )
            self.Variables[ varname ] = new_variable
        else:
            self.Variables[ varname ] = self.GetVarEvents( arg )

    def NewEvent( self, dic ):
        ''' Adds a new event to the manager. A value for all the variables has to be
        provided. '''
        for key, values in self.Variables.iteritems():
            values.append( dic[ key ] )
        self.Nentries += 1

    def Print( self, *args, **kwargs ):
        ''' Prints the information of the class as well as the values for the first 20
        events. If < events > is introduced as an input, the number of events showed
        would be that specified by the user. If < cut > is specified only will be
        showed the events that statisfy the given cut. If < name > is specified this
        name is going to be printed before the other information. '''

        if 'cut' not in kwargs:
            kwargs[ 'cut' ] = False
        if 'events' not in kwargs:
            kwargs[ 'events' ] = False
        if 'variables' not in kwargs:
            kwargs[ 'variables' ] = self.Variables.keys()
            kwargs[ 'variables' ].sort()

        max_length = 0
        for var in kwargs[ 'variables' ]:
            lvar = len( var )
            if lvar > max_length:
                max_length = lvar

        ''' Prints the name of the manager '''
        if self.Name:
            lname = len( self.Name )
            print '\n' + 3*lname*'*' + '\n' + lname*' ' + self.Name + '\n' + 3*lname*'*'
        
        ''' Prints the targets '''
        if self.Targets:
            print '\nFiles attached:'
            for ifile in self.Targets:
                out = ' - ' + ifile.GetName() + ': '
                for tree in self.Targets[ ifile ]:
                    out += tree.GetName() + ', '
                print out[ :-2 ]

        ''' Prints the variables '''
        if self.Variables:
            vout      = ' '
            variables = []
            if len( args ):
                variables = args
                for var in args:
                    vout      += var + ', '
            else:
                for var in kwargs[ 'variables' ]:
                    variables.append( var )
                    vout      += var + ', '
            print '\nVariables:' + vout[ :-2 ] + '\n'

            ''' Prints the values of the variables '''
            vout = ' '
            for var in variables:
                vout += '%.3e' + '\t|\t'
            vout = vout[ :-2 ]

            if kwargs[ 'cut' ]:
                evlist = self.GetCutList( kwargs[ 'cut' ] )
            else:
                evlist = range( self.Nentries )

            if kwargs[ 'events' ]:
                i = 0
                for ievt in evlist:
                    if i == kwargs[ 'events' ]: break
                    i += 1
                    print vout % self.GetEventTuple( ievt, *variables )
            else:
                for ievt in evlist:
                    if ievt and ievt % 20 == 0:
                        if raw_input(
                            '< Introduce q to exit, and any other input to continue printing >: '
                            ) == 'q': break
                    print vout % self.GetEventTuple( ievt, *variables )
        else:
            print 'ERROR: No variables booked in this manager'

    def RemoveVariable( self, var ):
        ''' Removes a booked variable '''
        del self.Variables[ var ]

    def Save( self, name, tree_name = False, **kwargs ):
        ''' Saves the given class values in a TTree. If only < name > is defined, it is
        considered as the tree name, so it is written in the external directory ( to
        be constructed and accesible in the main program ), if two names are provided
        the first one is considered as the file name and the second the tree name. If
        < close > is provided, and if its value is false, this method will return
        the output file. If in < kwargs > the < ftype > key is set to 'txt', then the
        output will be considered as a txt where the columns correspond to each variable in
        alphabetical order. In any case the variables to be stored can be specified using
        the keyword < variables >, providing a list with them. '''
        if 'ftype' in kwargs: ftype = kwargs[ 'ftype' ]
        else: ftype = 'root'
        if 'variables' in kwargs: variables = kwargs[ 'variables' ]
        else: variables = self.Variables.keys()
        if 'close' in kwargs: close = kwargs[ 'close' ]
        else: close = True
        if ftype in ( 'root', 'Root', 'ROOT' ):
            if tree_name:
                ofile = TFile.Open( name, 'RECREATE' )
                print 'Saving tree with name <', tree_name, '> in <', name, '>'
                TreeFromDict( tree_name, self.Variables )
            else:
                print ( 'Saving tree with name <', name, '> in <', gDirectory.GetName(), '>' )
                TreeFromDict( name, self.Variables )
            print 'Written', self.Nentries, 'entries'
            if close: ofile.Close()
            else: return ofile
        elif ftype in ( 'txt', 'TXT' ):
            ofile = open( name, 'wt' )
            print 'Saving txt data in file <', name, '>'
            varvalues = [ self.Variables[ var ] for var in variables ]
            out = ''
            for var in variables:
                out += var + '\t'
            ofile.write( out[ :-1 ] + '\n' )
            for ievt in xrange( self.Nentries ):
                out = ''
                for var in varvalues:
                    out += str( var[ ievt ] ) + '\t'
                ofile.write( out[ :-1 ] + '\n' )
            if close: ofile.close()
            else: return ofile

    def StoreTxtData( self, fname, tnames = [], **kwargs ):
        ''' Method to store the values almacenated on a txt file. To see the functionality
        of this method take a look at the function < DictFromTxt >. '''
        if not tnames and not kwargs:
            tnames = self.Variables.keys()
        dict2add = DictFromTxt( fname, tnames, **kwargs )
        self.AddDataFromDict( dict2add )
        
        print 'WARNING:', self.Name, '=> Variable <', var, '> not in the manager; not stored'
        print 'Storing', len( columns ), 'variables from txt file <', fname, '>'
        self.Nentries = len( self.Variables[ self.Variables.keys()[ 0 ] ] )
        ifile.close()

#_______________________________________________________________________________
# If the input is a string, returns an array with values of a certain type
# depending on the identifier located in the title. If the input is a list, it
# finds the type of values located on it, and returns the apropiated array.
def ArrayType( branch ):
    if isinstance( branch, str ):
        if   '/F' in branch:
            return array( 'f', [ 0 ] )
        elif '/D' in branch:
            return array( 'd', [ 0 ] )
        elif '/I' in branch:
            return array( 'i', [ 0 ] )
        elif '/O' in branch:
            return array( 'b', [ 0 ] )
        else:
            print 'ERROR: Type not found in <', branch, '>'
    else:
        if   isinstance( branch[ 0 ], float ):
            return array( 'd', [ 0 ] )
        elif isinstance( branch[ 0 ], int ):
            return array( 'i', [ 0 ] )
        elif isinstance( branch[ 0 ], bool ):
            return array( 'b', [ 0 ] )
        else:
            print 'ERROR: Could not extract the type in <', branch[ 0 ], '>'
    
#_______________________________________________________________________________
# This function creates a new branch in the given tree using the values stored
# in a list
def BranchFromList( brname, tree, lst ):
    if len( lst ) != tree.GetEntries():
        print 'ERROR: The size of the input list and the tree is not the same'
    tree.SetBranchStatus( '*', False )
    var    = ArrayType( lst )
    branch = tree.Branch( brname, var, brname + '/' + var.typecode.upper() )
    for ievt in xrange( tree.GetEntries() ):
        tree.GetEntry( ievt )
        var[ 0 ] = lst[ ievt ]
        branch.Fill()
    branch.Write()
    tree.SetBranchStatus( '*', True )

#_______________________________________________________________________________
# Creates a new dictionary containing the values of the variables stored in a
# TTree object. The input is composed by the tree and the variables to be
# stored ( given in a list ).
def DictFromTree( tree, varlist ):
    tree.SetBranchStatus( '*', False )
    avars, tvals = [], []
    for var in varlist:
        tree.SetBranchStatus( var, True )
        avars.append( ArrayType( tree.GetBranch( var ).GetTitle() ) )
        tvals.append( [] )
        tree.SetBranchAddress( var, avars[ -1 ] )
    rvars = range( len( varlist ) )
    for ievt in xrange( tree.GetEntries() ):
        tree.GetEntry( ievt )
        for i in rvars:
            tvals[ i ].append( avars[ i ][ 0 ] )
    tree.ResetBranchAddresses()
    tree.SetBranchStatus( '*', True )
    return dict( ( var, tvals[ i ] ) for var, i in zip( varlist, rvars ) )

#_______________________________________________________________________________
# Creates a new dictionary containing the values of the variables stored on
# a txt file. The file path is specified in < fname >, while the names of the
# variables are given as a list in < tnames >. In < kwargs >, the column index
# to be read has to be given as colidx = [ 1, 3, 5, ... ]. If < tnames > is
# provided, and the first row has the names of the variables, it has
# preference over the < colidx > variable. In the case where the first row
# does not have the names, < tnames > and < columns > must match.
def DictFromTxt( fname, tnames = [], **kwargs ):
    ifile = open( fname, 'rt' )
    line  = ifile.readline().split()
    if 'colidx' in kwargs:
        columns = kwargs[ 'colidx' ]
    else:
        columns = range( len( line ) )
    if all( isinstance( line[ i ], str ) for i in columns ):
        if tnames:
            columns = [ columns[ i ] for i in columns if tnames[ i ] == line[ i ] ]
        else:
            tnames = [ line[ i ] for i in columns ]
        line = ifile.readline().split()
    elif any( isinstance( line[ i ], str ) for i in columns ):
        print 'ERROR: The first line of the input file has not the correct format'
        return
    else:
        if not tnames:
            print 'ERROR: The names of the variables in the columns have to be specified'
            return
        elif len( tnames ) != len( columns ):
            print 'ERROR: The names of the variables and the column index must match'
            return
    print 'Storing', len( columns ), 'variables from txt file <', fname, '>'
    convfuncs, varvalues = [], []
    for index, icol in enumerate( columns ):
        value = line[ icol ]
        try:
            int( value )
            convfuncs.append( int )
            isint = True
        except:
            try:
                float( value )
                convfuncs.append( float )
            except:
                print 'ERROR: Format for column <', i, '> not recognised'
        varvalues.append( [ convfuncs[ -1 ]( value ) ] )
    for line in ifile:
        line = line.split()
        for index, icol in enumerate( columns ):
            varvalues[ index ].append( convfuncs[ index ]( line[ icol ] ) )
    ifile.close()
    return { name: varvalues[ index ] for index, name in enumerate( tnames ) }

#_______________________________________________________________________________
# This function almacenates the values of a leaf in a TTree into a list, given
# the tree and the branch name
def ListFromBranch( brname, tree ):
    tree.SetBranchStatus( '*', False )
    tree.SetBranchStatus( brname, True )
    branch = tree.GetBranch( brname )
    var    = ArrayType( branch.GetTitle() )
    tree.SetBranchAddress( brname, var )
    lst = []
    for ievt in xrange( tree.GetEntries() ):
        tree.GetEntry( ievt )
        lst.append( var[ 0 ] )
    tree.ResetBranchAddresses()
    tree.SetBranchStatus( '*', True )
    return lst

#_______________________________________________________________________________
# Creates a new tree with the lists stored in a dictionary. The name of the
# branches are given by the keys in the dictionary. The names are sorted.
def TreeFromDict( name, dic, **kwargs ):
    if 'level' not in kwargs: kwargs[ 'level' ] = 0
    if 'title' not in kwargs: kwargs[ 'title' ] = name
    tree      = TTree( name, kwargs[ 'title' ], kwargs[ 'level' ] )
    variables = dic.keys()
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
