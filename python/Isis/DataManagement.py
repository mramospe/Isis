#/////////////////////////////////////////////////////////////
#//
#//  Python modules
#//
#// ----------------------------------------------------------
#//
#//  AUTHOR: Miguel Ramos Pernas
#//  e-mail: miguel.ramos.pernas@cern.ch
#//
#//  Last update: 30/11/2016
#//
#// ----------------------------------------------------------
#//
#//  Description:
#//
#//  Defines classes and functions to manage data from/to
#//  Root files and trees.
#//
#// ----------------------------------------------------------
#/////////////////////////////////////////////////////////////


from ROOT import TFile, TTree, gDirectory
from array import array
import math
from Isis.BoostPy.RootTree import DictFromTree, ListFromTree, TreeFromDict, TreeFromList
from Isis.Algebra import LongVector, Matrix
from Isis.PlotTools import MakeHistogram, MakeHistogram2D, MakeScatterPlot
from Isis.Utils import FormatEvalExpr, JoinDicts, MergeDicts, StringListFilter


#_______________________________________________________________________________
# Class to manage data, specially designed to work together with Root files
# and trees
class DataManager:

    def __init__( self, name = False, ifile = False, tnames = [], **kwargs ):
        '''
        The constructor works in three different ways. If < ifile > is a dictionary,
        the current variables in the class are set to those stored in it. If < ifile >
        is a string, by default it is considered that the input file is a Root file,
        and < tnames > has to be the list of TTree names in the file. In < kwargs >, you
        can specify the file type ( ftype = 'root'/'txt' ). If it is set to 'txt', then
        < tnames > could be a list containing the names of the different variables or,
        if it is left in blanck, it will take the names of the variables situated in the
        first line of the file. The number of variables specified should match the number
        of columns in the file. Otherwise one has to specify via < colidx = [ 1, 6, ... ] >
        the index for the columns to be stored. Other attributes can be added too.
        '''

        ''' These are the main attributes that this class has '''
        self.Iterator    = 0
        self.Name        = ''
        self.Nentries    = 0
        self.Targets     = {}
        self.Variables   = {}

        ''' The constructor starts here '''
        if name != False:
            self.Name = name
            
            if ifile:
                ''' First check if < ifile > is a string or if it can be used as a dictionary'''
                if isinstance( ifile, str ):

                    ftype = kwargs.get( 'ftype', 'root' )
                
                    if ftype in ( 'root', 'Root', 'ROOT' ):
                        ''' This is the constructor for Root files '''
                        if ifile and tnames:
                            self.AddTarget( ifile, tnames )
                        elif ifile and not tnames:
                            print 'WARNING:', self.Name, 'Arguments for DataManager class using root files are < name > < file path > and  < tree name >'

                    elif ftype in ( 'txt', 'TXT' ):
                        ''' This is the constructor for txt files '''
                        if ifile and tnames:
                            self.AddDataFromTxt( ifile, tnames, **kwargs )
                        elif ifile and not tnames:
                            print 'WARNING:', self.Name, '=> Arguments for DataManager class using txt files are < file path > and < variables names >'
                
                    else:
                        ''' If the type specified is not recognised a warning message is sent '''
                        print 'WARNING:', self.Name, '=> File format <', ftype, '> for DataManager class not known'
            
                else:
                    ''' This is the constructor using a dictionary (the ftype value is omitted) '''
                    self.AddDataFromDict( ifile )

    def __add__( self, other ):
        '''
        Allows merging two objects of this class
        '''
        mgr          = DataManager()
        mgr.Nentries = self.Nentries + other.Nentries
        mgr.Targets.update( self.Targets  )
        mgr.Targets.update( other.Targets )
        true_vars = [ var for var in self.Variables if var in other.Variables ]
        for var in true_vars:
            mgr.Variables[ var ] = self.Variables[ var ] + other.Variables[ var ]
        mgr.Name = self.Name + '+' + other.Name
        return mgr

    def __iadd__( self, other ):
        '''
        Allows adding another manager variables to this class. The manager added
        looses the possession of the targets.
        '''
        if self.Nentries:
            mgr = self + other
            return self + other
        else:
            self = other.Copy()

    def __getitem__( self, var ):
        '''
        Gets the values for variable < var >
        '''
        return self.Variables[ var ]

    def __iter__( self ):
        '''
        Definition of the iterator
        '''
        self.Iterator = 0
        return self

    def __len__( self ):
        '''
        Gets the number of entries in the class
        '''
        return self.Nentries

    def next( self ):
        '''
        Sets the new value for the iteration. If it reaches the limit the exception
        is raised.
        '''
        if self.Iterator == self.Nentries:
            raise StopIteration
        else:
            self.Iterator += 1
            return self.GetEventDict( self.Iterator - 1 )
        
    def _getVarNames( self, regexps ):
        '''
        Get the variable names for the trees booked in the targets of this class. If < regexps >
        is provided, only the variables matching the expressions in it will be returned.
        '''
        vnames = []
        for fname, tlist in self.Targets.iteritems():
            for tpath in tlist:
                vnames.append( VarsInRootTree( fname = fname, tpath = tpath, regexps = regexps ) )
        return vnames
    
    def AddDataFromDict( self, dict2add ):
        '''
        Adds data from a dictionary of lists. First checks that all the lists in the
        dictionary have the same size. Then a check is made to see if some variables are
        present in the current manager. If so, all of them have to be present.
        '''
        lgth = len( next( dict2add.itervalues() ) )
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

    def AddDataFromTxt( self, fname, tnames = [], **kwargs ):
        '''
        Method to store the values almacenated on a txt file. To see the functionality
        of this method take a look at the function < DictFromTxt >.
        '''
        if not tnames and not kwargs:
            tnames = self.Variables.keys()
        self.AddDataFromDict( DictFromTxt( fname, tnames, **kwargs ) )
        print self.Name, '=> Stored', len( tnames ), 'variables from txt file <', fname, '>'

    def AddTarget( self, fname, tnames ):
        '''
        Adds a new target file and/or tree to the class. One has to provide the file
        name and the tree names.
        '''
        if fname not in self.Targets.keys():
            self.Targets[ fname ] = tnames
            print self.Name, '=> Added target <', fname, '> and trees:', tnames
        else:
            self.Targets[ fname ] += tnames
            print self.Name, '=> Added trees: ', list( tnames ), 'from target <', fname, '>'
        if self.Variables:
            dictlist = [ DictFromTree( fname, tpath, self.Variables.keys(), cuts ) for tpath in tnames ]
            self.Variables.update( MergeDicts( *dictlist ) )
            self.Nentries = len( self.Variables.items()[ 0 ][ 1 ] )
            
    def BookVariables( self, *var_names ):
        '''
        Books a new variable(s) to the class. The variable's values list is filled
        with the variable's values contained in the targets. If a variable already
        exists in the class the process is omited. If < * > is specified, the
        variables are set as those that are common for all the trees.
        '''
        if len( var_names ) == 1 and var_names[ 0 ] == '*':
            vnames    = self._getVarNames()
            var_names = set( vnames[ 0 ] )
            for lst in vnames[ 1: ]:
                var_names = var_names.intersection( lst )
            self.BookVariables( *var_names )
        elif len( self.Targets ):
            vnames    = self._getVarNames( var_names )
            var_names = set( vnames[ 0 ] )
            for lst in vnames[ 1: ]:
                var_names = var_names.intersection( lst )
            truevars = []
            for name in var_names:
                if name not in self.Variables:
                    truevars.append( name )
                else:
                    print 'WARNING:', self.Name, '=> Variable <', name, '> already booked'
            
            ''' Load the variables for each of the targets '''
            dictlist = []
            for fname, tlist in self.Targets.iteritems():
                dictlist += [ DictFromTree( fname, tpath, truevars ) for tpath in tlist ]
            self.Variables.update( MergeDicts( *dictlist ) )
            self.Nentries = len( self.Variables.items()[ 0 ][ 1 ] )
        else:
            print 'WARNING: ', self.Name, '=> No targets added to the manager, could not book variables:', var_names

    def Copy( self, name = '' ):
        '''
        Returns a copy of this class
        '''
        if not name:
            name = self.Name + '_copy'
        return DataManager( name, self.Variables )
    
    def GetCutList( self, cut, mathmod = math ):
        '''
        This method allows to obtain a list with the events that satisfy the cuts given
        '''
        cut, variables = FormatEvalExpr( cut, mathmod )
        varstoadd = [ v for v in variables if v not in self.Variables ]
        if varstoadd:
            print 'WARNING: Loading additional variables to apply the cuts:', varstoadd
            self.BookVariables( *varstoadd )
        values = [ self.Variables[ var ] for var in variables ]
        for ivar in xrange( len( variables ) ):
            cut = cut.replace( variables[ ivar ], 'values[ %i ][ ievt ]' %ivar )
        return eval( '[ ievt for ievt in xrange( self.Nentries ) if ' + cut + ' ]' )

    def GetEntries( self, selection = False, mathmod = math ):
        '''
        Gets the number of entries of the class. If a cut selection is given, it is
        calculated the number of events that satisfy those cuts.
        '''
        if selection:
            return len( self.GetCutList( selection ), mathmod )
        else:
            return self.Nentries

    def GetEventDict( self, ievt ):
        '''
        Returns a dictionary with the values of the variables at the given entry
        '''
        return dict( ( var, values[ ievt ] ) for var, values in self.Variables.iteritems() )

    def GetEventTuple( self, ievt, *args ):
        '''
        Gets the event at position ievt. Allows to get only the variables in < args >
        in the order specified.
        '''
        if len( args ):
            return tuple( [ self.Variables[ var ][ ievt ] for var in args ] )
        else:
            return tuple( [ values[ ievt ] for var, values in self.Variables.iteritems() ] )

    def GetMatrix( self, *args, **kwargs ):
        '''
        Returns a matrix containing the values of the variables specified in < args >.
        If the option < EvtsInRows > is set to False, the matrix will be created as a list
        containing a list for each of the variables. Otherwise the matrix will be a list
        containing a list for each event, with the variables same order as in < args >. If
        '*' appears in < args >, there will be taken all the variables in the manager
        ordered by name
        '''
        trans = kwargs.get( 'EvtsInRows', True )
        if '*' in args:
            args = self.Variables.keys()
            args.sort()
        matrix = Matrix( [ self.Variables[ var ] for var in args ] )
        if trans:
            return matrix.Transpose()
        else:
            return matrix

    def GetNvars( self ):
        '''
        Gets the number of variables in the class
        '''
        return len( self.Variables )

    def GetVarEvents( self, *args, **kwargs ):
        '''
        If an element in < args > is a variable, it gets the list of values for it. If
        it is an expression, it returns a list with the values corresponding to it.
        '''
        cuts    = kwargs.get( 'cuts', False )
        mathmod = kwargs.get( 'mathmod', math )
        
        if cuts:
            entries = self.GetCutList( cuts, mathmod )
        else:
            entries = xrange( self.Nentries )
        
        variables = []
        trueargs  = []
        for v in args:
            if v in self.Variables:
                variables.append( v )
            else:
                v, newv = FormatEvalExpr( v, mathmod )
                variables += newv 
            ''' The module is not imported, so the name must change '''
            trueargs.append( v.replace( mathmod.__name__, 'mathmod' ) )
        
        variables = list( set( variables ) )
        variables.sort()
        variables.reverse()
        
        values = [ self.Variables[ var ] for var in variables ]
        nvars  = len( variables )
        output = []
        for iv, arg in enumerate( trueargs ):
            for ivar in xrange( nvars ):
                arg = arg.replace( variables[ ivar ], 'values[ %i ][ ievt ]' %ivar )
            output.append( eval( '[ %s for ievt in entries ]' %arg ) )
        cmd = 'output[ 0 ]'
        for i in xrange( 1, len( output ) ):
            cmd += ', output[ %i ]' %i
        return eval( cmd )

    def GetVarNames( self ):
        '''
        Gets the name of the variables in the class
        '''
        return self.Variables.keys()

    def MakeHistogram( self, var, wvar = False, **kwargs ):
        '''
        Makes the histogram of the given variable. A selection can be applied
        introducing < cuts >, as well as the name and the title can be defined in a
        similar way too.
        '''
        cuts    = kwargs.get( 'cuts', False )
        mathmod = kwargs.get( 'mathmod', math )
        
        if wvar:
            vals, wvar = self.GetVarEvents( var, wvar, cuts = cuts, mathmod = mathmod )
        else:
            vals = self.GetVarEvents( var, cuts = cuts, mathmod = mathmod )

        kwargs[ 'name' ]   = kwargs.get( 'name', self.Name + '_' + var )
        kwargs[ 'title' ]  = kwargs.get( 'title', kwargs[ 'name' ] )
        kwargs[ 'xtitle' ] = kwargs.get( 'xtitle', var )
        kwargs[ 'wvar' ]   = kwargs.get( 'wvar', wvar )

        return MakeHistogram( vals, **kwargs )

    def MakeHistogram2D( self, xvar, yvar, wvar = False, **kwargs ):
        '''
        Makes the 2-dimensional histogram of the given variables
        '''
        cuts    = kwargs.get( 'cuts', False )
        mathmod = kwargs.get( 'mathmod', math )
        
        if wvar:
            xvar, yvar, wvar = self.GetVarEvents( xvar, yvar, wvar, cuts = cuts, mathmod = mathmod )
        else:
            xvar, yvar = self.GetVarEvents( xvar, yvar, cuts = cuts, mathmod = mathmod )

        kwargs[ 'name' ]   = kwargs.get( 'name', self.Name + '_' + yvar + '_vs_' + xvar )
        kwargs[ 'title' ]  = kwargs.get( 'title', kwargs[ 'name' ] )
        kwargs[ 'xtitle' ] = kwargs.get( 'xtitle', xvar )
        kwargs[ 'ytitle' ] = kwargs.get( 'ytitle', yvar )
        kwargs[ 'wvar' ]   = kwargs.get( 'wvar', wvar )
            
        return MakeHistogram2D( xvar, yvar, **kwargs )

    def MakeScatterPlot( self, xvar, yvar, xerr = False, yerr = False, **kwargs ):
        '''
        Creates a graph object with the points corresponding to two variables
        '''
        cuts    = kwargs.get( 'cuts', False )
        mathmod = kwargs.get( 'mathmod', math )

        if not xerr and not yerr:
            xvar, yvar = self.GetVarEvents( xvar, yvar, cuts = cuts, mathmod = mathmod )
        elif xerr and yerr:
            xvar, yvar, xerr, yerr = self.GetVarEvents( xvar, yvar, xerr, yerr, cuts = cuts, mathmod = mathmod )
        elif xerr:
            xvar, yvar, xerr = self.GetVarEvents( xvar, yvar, xerr, cuts = cuts, mathmod = mathmod )
        else:
            xvar, yvar, yerr = self.GetVarEvents( xvar, yvar, yerr, cuts = cuts, mathmod = mathmod )

        kwargs[ 'name' ]   = kwargs.get( 'name', yvar + 'vs' + xvar )
        kwargs[ 'title' ]  = kwargs.get( 'title', kwargs[ 'name' ] )
        kwargs[ 'xtitle' ] = kwargs.get( 'xtitle', xvar )
        kwargs[ 'ytitle' ] = kwargs.get( 'ytitle', yvar )
        
        return MakeScatterPlot( xvar, yvar, xerr, yerr, **kwargs )

    def MakeVariable( self, varname, arg, function = False ):
        '''
        Makes another variable using those in the class. There are two different
        ways to do it. The first one consists on specifying the new variable name,
        the name of the variables used by the function ( ordered in a list ) and the
        function itself. The computation of the new variable is going to be performed
        passing the variables to the function as normal entries ( *args, where args is
        the list of values ). The second method consists on specifying only the name
        of the variable and an expression in < arg >. The values will be processed
        then for each entry taking into account the value obtained when evaluating the
        expression.
        '''
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
        '''
        Adds a new event to the manager. Values for all the variables have to be
        provided.
        '''
        for key, values in self.Variables.iteritems():
            values.append( dic[ key ] )
        self.Nentries += 1

    def Print( self, *args, **kwargs ):
        '''
        Prints the information of the class as well as the values for the first 20
        events. If < evts > is introduced as an input, the number of events showed
        would be that specified by the user. If < cut > is specified only will be
        showed the events that statisfy the given cut. If < prec > is given, the
        number of decimal points it sets to this value.
        '''
        
        if not self.Variables:
            print 'ERROR:', self.Name, '=> No variables booked in this manager'
            return
        
        cuts    = kwargs.get( 'cuts', False )
        mathmod = kwargs.get( 'mathmod', math )
        evts    = kwargs.get( 'evts', False )
        prec    = kwargs.get( 'prec', 3 )

        form = '%.' + str( prec ) + 'e'
        if prec:
            prec += 1
        
        ''' If no variables are specified all are printed '''
        if not args:
            args = self.Variables.keys()
        args.sort()
        
        ''' Prints the name of the manager '''
        if self.Name:
            lname = 3*len( self.Name )
            print '\n' + lname*'*' + '\n' + self.Name.center( lname ) + '\n' + lname*'*'
        
        ''' Prints the targets '''
        if self.Targets:
            print '\nFiles attached:'
            for ifile, tlist in self.Targets.iteritems():
                out = ' - ' + ifile.GetName() + ': '
                for tree in tlist:
                    out += tree.GetName() + ', '
                print out[ :-2 ]

        ''' Prints the variables. The variable < maxsize > is the maximum size of the
        numbers in the print '''
        maxsize   = 7 + prec
        vout      = ' '
        shortvars = []
        for var in args:
            vout += var + ', '
            if len( var ) > maxsize:
                shortvars.append( var[ :maxsize ] + '*' )
            else:
                shortvars.append( var.center( maxsize ) )
        print 'Variables:' + vout[ :-2 ]
        vout = '| '
        for var in shortvars:
            vout += var.center( maxsize ) + ' |'
        deco = len( vout )*'='
        print deco + '\n' + vout + '\n' + deco
        
        ''' Prints the values of the variables '''
        if cuts:
            evtlst = self.GetCutList( cuts, mathmod )
        else:
            evtlst = xrange( self.Nentries )

        if evts:
            i = 0
            for ievt in evtlst:
                if i == evts: break
                i += 1
                vout = '| '
                for var in self.GetEventTuple( ievt, *args ):
                    vout += ( form %var ).rjust( maxsize ) + ' |'
                print vout
            print deco + '\n'
        else:
            for ievt in evtlst:
                if ievt and ievt % 20 == 0:
                    if raw_input(
                        '< Introduce q to exit, and any other input to continue printing >: '
                        ) == 'q': break
                vout = '| '
                for var in self.GetEventTuple( ievt, *args ):
                    vout += ( form %var ).rjust( maxsize ) + ' |'
                print vout
            if ievt + 1 == len( evtlst ):
                print deco + '\n'

    def RemoveVariable( self, var ):
        '''
        Removes a booked variable
        '''
        del self.Variables[ var ]

    def Save( self, name, tree_name = False, **kwargs ):
        '''
        Saves the given class values in a TTree. If only < name > is defined, it is
        considered as the tree name, so it is written in the external directory ( to
        be constructed and accesible in the main program ), if two names are provided
        the first one is considered as the file name and the second the tree name. If
        < close > is provided, and if its value is false, this method will return
        the output file. If in < kwargs > the < ftype > key is set to 'txt', then the
        output will be considered as a txt where the columns correspond to each variable in
        alphabetical order. In any case the variables to be stored can be specified using
        the keyword < variables >, providing a list with them.
        '''
        ftype     = kwargs.get( 'ftype', 'root' )
        variables = kwargs.get( 'variables', self.Variables.keys() )
        close     = kwargs.get( 'close', True )
        
        if ftype in ( 'root', 'Root', 'ROOT' ):
            if tree_name:
                ofile = TFile.Open( name, 'RECREATE' )
                print self.Name, '=> Saving tree with name <', tree_name, '> in <', name, '>'
                TreeFromDict( tree_name, self.Variables )
            else:
                print self.Name, '=> Saving tree with name <', name, '> in <', gDirectory.GetName(), '>'
                TreeFromDict( name, self.Variables )
            print self.Name, '=> Written', self.Nentries, 'entries'
            if close: ofile.Close()
            else: return ofile
        elif ftype in ( 'txt', 'TXT' ):
            ofile = open( name, 'wt' )
            print self.Name, '=> Saving txt data in file <', name, '>'
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
    
    def SetName( self, name ):
        '''
        Sets the name of the current manager
        '''
        self.Name = name

    def SubSample( self, name = '', **kwargs ):
        '''
        Returns a copy of this class satisfying the given requirements. A set of cuts can
        be specified. The range of the events to be copied can be specified (as a slice
        object), as well as the variables to be copied. By default the entire class is
        copied.
        '''
        if name == '':
            name = self.Name + '_SubSample'
        if 'cuts' in kwargs:
            mathmod = kwargs.get( 'mathmod', math )
            cmgr    = DataManager( name )
            evtlst  = self.GetCutList( kwargs[ 'cuts' ], mathmod )
            for kw, vlist in self.Variables.iteritems():
                cmgr.Variables[ kw ] = [ vlist[ i ] for i in evtlst ]
        else:
            cmgr = self.Copy( name )
        evts   = kwargs.get( 'evts', slice( 0, self.Nentries ) )
        varset = kwargs.get( 'varset', cmgr.Variables.keys() )
        for v in cmgr.Variables.keys():
            if v in varset:
                cmgr.Variables[ v ] = cmgr.Variables[ v ][ evts ]
            else:
                del cmgr.Variables[ v ]
        cmgr.Nentries = len( next( cmgr.Variables.itervalues() ) )
        return cmgr

#_______________________________________________________________________________
# Creates a new dictionary containing the values of the variables stored on
# a txt file. The file path is specified in < fname >, while the names of the
# variables are given as a list in < tnames >. In < kwargs >, the column index
# to be read has to be given as colidx = [ 1, 3, 5, ... ]. If < tnames > is
# provided, and the first row has the names of the variables, it has
# preference over the < colidx > variable. In the case where the first row
# does not have the names, < tnames > and < columns > must match.
def DictFromTxt( fname, tnames = [], **kwargs ):
    ifile   = open( fname, 'rt' )
    line    = ifile.readline().split()
    columns = kwargs.get( 'colidx', range( len( line ) ) )
    if all( isinstance( line[ i ], str ) for i in columns ):
        if tnames:
            columns = [ columns[ i ] for i in columns if line[ i ] in tnames ]
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
# Creates a manager from the root file in < file_path > and the tree in
# < tree_path >. By default it books all the variables, but they can be provided
# in < **kwargs >, as well as some cuts to be applied.
def ManagerFromTree( name, file_path, tree_path, **kwargs ):
    cuts      = kwargs.get( 'cuts', '' )
    mathmod   = kwargs.get( 'mathmod', math )
    variables = kwargs.get( 'variables', [ '*' ] )
    mgr       = DataManager( name, file_path, [ tree_path ] )
    if variables == '*':
        variables = [ '*' ]
    mgr.BookVariables( *variables )
    if cuts:
        return mgr.SubSample( cuts = cuts, mathmod = mathmod )
    else:
        return mgr

#_______________________________________________________________________________
# Return variables in a tree. If < regexps > are provided, only variables
# matching it will be returned.
def VarsInRootTree( tree = None, fname = '', tpath = '', regexps = [] ):
    if not tree:
        rfile = TFile.Open( fname )
        tree  = rfile.Get( tpath )
    brnames = [ el.GetName()
                for el in tree.GetListOfBranches() ]
    if not tree:
        rfile.Close()
    if regexps != []:
        truenames = []
        for expr in regexps:
            addlst = StringListFilter( brnames, expr )
            if addlst != []:
                truenames += addlst
            else:
                print 'WARNING: No variables found matching expression <', expr, '>'
        return truenames
    else:
        return brnames
