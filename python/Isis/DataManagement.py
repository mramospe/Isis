#/////////////////////////////////////////////////////////////
#//
#//  Python modules
#//
#// ----------------------------------------------------------
#//
#//  AUTHOR: Miguel Ramos Pernas
#//  e-mail: miguel.ramos.pernas@cern.ch
#//
#//  Last update: 28/12/2016
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
from Isis.IBoost.RootTree import DictFromTree, ListFromTree, TreeFromDict, TreeFromList
from Isis.Algebra import LongVector, Matrix
from Isis.PlotTools import MakeHistogram, MakeHistogram2D, MakeScatterPlot
from Isis.Utils import FormatEvalExpr, JoinDicts, MergeDicts, StringListFilter


#_______________________________________________________________________________
# Class to manage data, specially designed to work together with Root files
# and trees
class DataManager( dict ):

    def __init__( self, name = '', path = '', tree = 'DecayTree', variables = [ '*' ], colid = [], ftype = 'root' ):
        '''
        The constructor provides the possibility of loading data from root or
        txt files, or from dictionary-like classes:
        
        - name:  Name of this class.
        - path:  If it is a string it is the path to the input file. If not
        it is considered to be dictionary-like.
        - ftype: Type for the input file.
        - variables: List of variables to be booked.
        - colid: In case of loading a txt file, it refers to the columns to be
        added (see function < DictFromTxt >.
        
        All the constructors finally call the constructor given a
        dictionary-like class.
        '''

        self.Iterator = 0
        self.Name     = name
        
        if isinstance( path, str ):
            if ftype:
                if ftype == 'root':
                    self.__init__( name, DictFromTree( path, tree, variables ) )
                elif ftype == 'txt':
                    self.__init__( name, DictFromTxt( path, variables, colid ) )
                else:
                    print 'ERROR:', self.Name, '=> Unknown input file type <', ftype, '>'
            else:
                print 'ERROR:', self.Name, '=> The input file type must be specified'
        else:
            for kw, lst in path.iteritems():
                self[ kw ] = list( lst )
        
        wrong = ( len( set( len( lst ) for lst in self.itervalues() ) ) != 1 )
        if wrong:
            print 'ERROR:', self.Name, '=> The lists stored in the manager have different lengths'

    def __add__( self, other ):
        '''
        Allows merging two objects of this class
        '''
        mgr = DataManager( self.Name + '__' + other.Name )
        true_vars = [ var for var in self if var in other ]
        for var in true_vars:
            mgr[ var ] = self[ var ] + other[ var ]
        no_booked = set( self.keys() + other.keys() ).difference( true_vars )
        if no_booked:
            print 'WARNING:', mgr.Name, '=> The following variables are not booked:', no_booked
        return mgr

    def __iadd__( self, other ):
        '''
        Allows adding another manager variables to this class
        '''
        if len( self ):
            return self + other
        else:
            self = other.Copy()

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
        return len( next( self.itervalues() ) )

    def next( self ):
        '''
        Sets the new value for the iteration. If it reaches the limit the exception
        is raised.
        '''
        if self.Iterator == len( self ):
            raise StopIteration
        else:
            self.Iterator += 1
            return self.GetEventDict( self.Iterator - 1 )
            
    def Copy( self, name = '' ):
        '''
        Returns a copy of this class
        '''
        if not name:
            name = self.Name + '_copy'
        return DataManager( name, self )
    
    def GetCutList( self, cut, mathmod = math ):
        '''
        This method allows to obtain a list with the events that satisfy the cuts given
        '''
        cut, variables = FormatEvalExpr( cut, mathmod )
        varstoadd = [ v for v in variables if v not in self ]
        if varstoadd:
            print 'ERROR: Need to load additional variables to apply the cuts:', varstoadd
            return
        values = [ self[ var ] for var in variables ]
        for ivar in xrange( len( variables ) ):
            cut = cut.replace( variables[ ivar ], 'values[ %i ][ ievt ]' %ivar )
        return eval( '[ ievt for ievt in xrange( len( self ) ) if ' + cut + ' ]' )

    def GetEntries( self, selection = False, mathmod = math ):
        '''
        Gets the number of entries of the class. If a cut selection is given, it is
        calculated the number of events that satisfy those cuts.
        '''
        if selection:
            return len( self.GetCutList( selection ), mathmod )
        else:
            return len( self )

    def GetEventDict( self, ievt ):
        '''
        Returns a dictionary with the values of the variables at the given entry
        '''
        return dict( ( var, values[ ievt ] ) for var, values in self.iteritems() )

    def GetEventTuple( self, ievt, *args ):
        '''
        Gets the event at position ievt. Allows to get only the variables in < args >
        in the order specified.
        '''
        if len( args ):
            return tuple( [ self[ var ][ ievt ] for var in args ] )
        else:
            return tuple( [ values[ ievt ] for var, values in self.iteritems() ] )

    def GetMatrix( self, variables = '*', trans = True ):
        '''
        Returns a list with the values from < variables > for each event. If < trans >
        is set to true, then it returns a list of lists with the values for each variable.
        '''
        if variables == '*':
            variables = self.keys()
            variables.sort()
        matrix = Matrix( [ self[ var ] for var in variables ] )
        if trans:
            return matrix.Transpose()
        else:
            return matrix

    def GetNvars( self ):
        '''
        Gets the number of variables in the class
        '''
        return len( self.keys() )

    def GetVarEvents( self, variables, cuts = False, mathmod = math ):
        '''
        Return a list of lists with the values associated with < variables >. If
        an element in < variables > is a variable, it gets the list of values for
        it. If it is an expression, it returns a list with the corresponding values.
        '''
        
        if cuts:
            entries = self.GetCutList( cuts, mathmod )
        else:
            entries = xrange( len( self ) )
        
        fvars    = []
        truevars = []
        for v in variables:
            if v in self:
                fvars.append( v )
            else:
                v, newv = FormatEvalExpr( v, mathmod )
                fvars += newv 
            ''' The module is not imported, so the name must change '''
            truevars.append( v.replace( mathmod.__name__, 'mathmod' ) )
        
        fvars = list( set( fvars ) )
        fvars.sort()
        fvars.reverse()
        
        values = [ self[ var ] for var in fvars ]
        nvars  = len( fvars )
        output = []
        for iv, arg in enumerate( truevars ):
            for ivar in xrange( nvars ):
                arg = arg.replace( fvars[ ivar ], 'values[ %i ][ ievt ]' %ivar )
            output.append( eval( '[ %s for ievt in entries ]' %arg ) )
        cmd = 'output[ 0 ]'
        for i in xrange( 1, len( output ) ):
            cmd += ', output[ %i ]' %i
        return eval( cmd )

    def MakeHistogram( self, var, wvar = False, **kwargs ):
        '''
        Makes the histogram of the given variable. A selection can be applied
        introducing < cuts >, as well as the name and the title can be defined in a
        similar way too.
        '''
        if 'cuts' in kwargs:
            cuts = kwargs.pop( 'cuts' )
        else:
            cuts = False
        if 'mathmod' in kwargs:
            mathmod = kwargs.pop( 'mathmod' )
        else:
            mathmod = math
        
        if wvar:
            vals, wvar = self.GetVarEvents( [ var, wvar ], cuts = cuts, mathmod = mathmod )
        else:
            vals = self.GetVarEvents( [ var ], cuts = cuts, mathmod = mathmod )

        kwargs[ 'name' ]   = kwargs.get( 'name', self.Name + '_' + var )
        kwargs[ 'title' ]  = kwargs.get( 'title', kwargs[ 'name' ] )
        kwargs[ 'xtitle' ] = kwargs.get( 'xtitle', var )
        kwargs[ 'wvar' ]   = kwargs.get( 'wvar', wvar )
        
        return MakeHistogram( vals, **kwargs )

    def MakeHistogram2D( self, xvar, yvar, wvar = False, **kwargs ):
        '''
        Makes the 2-dimensional histogram of the given variables
        '''
        if 'cuts' in kwargs:
            cuts = kwargs.pop( 'cuts' )
        else:
            cuts = False
        if 'mathmod' in kwargs:
            mathmod = kwargs.pop( 'mathmod' )
        else:
            mathmod = math
        
        if wvar:
            xvar, yvar, wvar = self.GetVarEvents( [ xvar, yvar, wvar ], cuts = cuts, mathmod = mathmod )
        else:
            xvar, yvar = self.GetVarEvents( [ xvar, yvar ], cuts = cuts, mathmod = mathmod )

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
            xvar, yvar = self.GetVarEvents( [ xvar, yvar ], cuts = cuts, mathmod = mathmod )
        elif xerr and yerr:
            xvar, yvar, xerr, yerr = self.GetVarEvents( [ xvar, yvar, xerr, yerr ], cuts = cuts, mathmod = mathmod )
        elif xerr:
            xvar, yvar, xerr = self.GetVarEvents( [ xvar, yvar, xerr ], cuts = cuts, mathmod = mathmod )
        else:
            xvar, yvar, yerr = self.GetVarEvents( [ xvar, yvar, yerr ], cuts = cuts, mathmod = mathmod )

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
            new_variable = len( self )*[ 0. ]
            var_tensor   = [ self[ vname ] for vname in arg ]
            lvars        = xrange( len( arg ) )
            for ievt in xrange( len( self ) ):
                values               = [ var_tensor[ ivar ][ ievt ] for ivar in lvars ]
                new_variable[ ievt ] = function( *values )
            self[ varname ] = new_variable
        else:
            self[ varname ] = self.GetVarEvents( arg )

    def NewEvent( self, dic ):
        '''
        Adds a new event to the manager. Values for all the variables have to be
        provided.
        '''
        for key, values in self.iteritems():
            values.append( dic[ key ] )

    def Print( self, variables = [], cuts = '', mathmod = math, evts = -1, prec = 3 ):
        '''
        Prints the information of the class as well as the values for the first 20
        events. If < evts > is introduced as an input, the number of events showed
        would be that specified by the user. If < cut > is specified only will be
        showed the events that statisfy the given cut. If < prec > is given, the
        number of decimal points it sets to this value.
        '''
        
        if not self:
            print 'ERROR:', self.Name, '=> No variables booked in this manager'
            return
        
        form = '%.' + str( prec ) + 'e'
        if prec:
            prec += 1
        
        ''' If no variables are specified all are printed '''
        if variables == []:
            variables = self.keys()
            variables.sort()
        
        ''' Prints the name of the manager '''
        if self.Name:
            lname = 3*len( self.Name )
            print '\n' + lname*'*' + '\n' + self.Name.center( lname ) + '\n' + lname*'*'
        
        '''
        Prints the variables. The variable < maxsize > is the maximum size of the
        numbers in the print
        '''
        maxsize   = 7 + prec
        vout      = ' '
        shortvars = []
        for var in variables:
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
        if cuts != '':
            evtlst = self.GetCutList( cuts, mathmod )
        else:
            evtlst = xrange( len( self ) )

        if evts != -1:
            i = 0
            for ievt in evtlst:
                if i == evts: break
                i += 1
                vout = '| '
                for var in self.GetEventTuple( ievt, *variables ):
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
                for var in self.GetEventTuple( ievt, *variables ):
                    vout += ( form %var ).rjust( maxsize ) + ' |'
                print vout
            if ievt + 1 == len( evtlst ):
                print deco + '\n'

    def Save( self, name = '', tree_name = False, ftype = 'root', variables = [], close = True ):
        '''
        Saves the given class values in a TTree. If < name > is not provided, the
        tree will be written in the external directory (to be constructed and
        accesible in the main program). If < close > is provided, and if its value
        is false, this method will return the output file. If < ftype > is set to
        'txt', then the output will be considered as a txt where the columns
        correspond to each variable in alphabetical order. In any case the
        variables to be stored can be specified using the keyword < variables >,
        providing a list with them.
        '''
        if variables == []:
            variables = self.keys()
        if ftype in ( 'root', 'Root', 'ROOT' ):
            if name != '':
                ofile = TFile.Open( name, 'RECREATE' )
            else:
                ofile = False
                name  = gDirectory.GetName()
            print self.Name, '=> Saving tree with name <', tree_name, '> in <', name, '>'
            TreeFromDict( self, name = tree_name, variables = variables )
            print self.Name, '=> Written', len( self ), 'entries'
            if ofile and close:
                ofile.Close()
            else:
                return ofile
        elif ftype in ( 'txt', 'TXT' ):
            ofile = open( name, 'wt' )
            print self.Name, '=> Saving txt data in file <', name, '>'
            varvalues = [ self[ var ] for var in variables ]
            out = ''
            for var in variables:
                out += var + '\t'
            ofile.write( out[ :-1 ] + '\n' )
            for ievt in xrange( len( self ) ):
                out = ''
                for var in varvalues:
                    out += str( var[ ievt ] ) + '\t'
                ofile.write( out[ :-1 ] + '\n' )
            if close:
                ofile.close()
            else:
                return ofile
    
    def SubSample( self, name = '', cuts = '', mathmod = math, evts = -1, varset = '*' ):
        '''
        Returns a copy of this class satisfying the given requirements. A set
        of cuts can be specified. The range of the events to be copied can be
        specified, as well as the variables to be copied. By default the
        entire class is copied.
        '''
        if evts == -1:
            evts = xrange( 0, len( self ) )
        if varset == '*':
            varset = self.keys()
        if name == '':
            name = self.Name + '_SubSample'
        if 'cuts' != '':
            evtlst = self.GetCutList( cuts, mathmod )
        else:
            evtlst = evts
            
        cmgr = DataManager( name )
        for kw in varset:
            vlist = self[ kw ]
            cmgr.Variables[ kw ] = [ vlist[ i ] for i in evtlst if i in evts ]
        cmgr.Nentries = len( next( cmgr.Variables.itervalues() ) )
        
        return cmgr

#_______________________________________________________________________________
# Creates a new dictionary containing the values of the variables stored on
# a txt file. The file path is specified in < fname >, while the names of the
# variables are given as a list in < tnames >. As a keyword argument, the column
# index to be read has to be given as colid = [ 1, 3, 4, ... ]. If < tnames >
# is provided, and the first row has the names of the variables, it has
# preference over the < colid > variable. In the case where the first row
# does not have the names, < tnames > and < colid > must match.
def DictFromTxt( fname, tnames = [], colid = [] ):
    ifile   = open( fname, 'rt' )
    line    = ifile.readline().split()
    if colid == []:
        colid = range( len( line ) )
    if all( isinstance( line[ i ], str ) for i in colid ):
        if tnames and tnames != [ '*' ]:
            colid = [ colid[ i ] for i in colid if line[ i ] in tnames ]
        else:
            tnames = [ line[ i ] for i in colid ]
        line = ifile.readline().split()
    elif any( isinstance( line[ i ], str ) for i in colid ):
        print 'ERROR: The first line of the input file has not the correct format'
        return
    else:
        if not tnames:
            print 'ERROR: The names of the variables in the colid have to be specified'
            return
        elif len( tnames ) != len( colid ):
            print 'ERROR: The names of the variables and the column index must match'
            return
    convfuncs, varvalues = [], []
    for index, icol in enumerate( colid ):
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
        for index, icol in enumerate( colid ):
            varvalues[ index ].append( convfuncs[ index ]( line[ icol ] ) )
    ifile.close()
    return { name: varvalues[ index ] for index, name in enumerate( tnames ) }

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
