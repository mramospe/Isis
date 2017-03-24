#/////////////////////////////////////////////////////////////
#//
#//  Python modules
#//
#// ----------------------------------------------------------
#//
#//  AUTHOR: Miguel Ramos Pernas
#//  e-mail: miguel.ramos.pernas@cern.ch
#//
#//  Last update: 24/03/2017
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


import math
import numpy as np

import ROOT as rt

from Isis.IBoost.PyGeneral import sendErrorMsg, sendWarningMsg
from Isis.IBoost.RootTree import treeToDict, treeToList, dictToTree, listToTree
from Isis.Utils import formatEvalExpr, joinDicts, mergeDicts, stringListFilter


class DataMgr( dict ):
    '''
    Class to manage data, specially designed to work together with Root files
    and trees
    '''
    def __init__( self, name = '', path = None, tree = 'DecayTree', variables = None, colid = None, ftype = 'root' ):
        '''
        The constructor provides the possibility of loading data from root or
        txt files, or from dictionary-like classes:
        
        - name:  Name of this class.
        - path:  If it is a string it is the path to the input file. If not
        it is considered to be dictionary-like.
        - ftype: Type for the input file.
        - variables: List of variables to be booked.
        - colid: In case of loading a txt file, it refers to the columns to be
        added (see function < txtToDict >.
        
        All the constructors finally call the constructor given a
        dictionary-like class.
        '''
        path = path or {}
        variables = variables or ['*']
        colid = None or []
        
        if isinstance( path, str ):
            if ftype:
                if ftype == 'root':
                    self.__init__( name, treeToDict( path, tree, variables ) )
                elif ftype == 'txt':
                    self.__init__( name, txtToDict( path, variables, colid ) )
                else:
                    sendErrorMsg('%s => Unknown input file type < %s >' %(name, ftype))
            else:
                sendErrorMsg('%s => The input file type must be specified' %name)
        else:
            for kw, lst in path.iteritems():
                self[ kw ] = list( lst )
                
            self._iter = 0
            self.name  = name
        
            wrong = ( len( set( len( lst ) for lst in self.itervalues() ) ) > 1 )
            if wrong:
                sendErrorMsg('%s => The lists stored in the manager '\
                             'have different lengths' %self.name)

    def __add__( self, other ):
        '''
        Allows merging two objects of this class
        '''
        mgr = DataMgr( self.name + '__' + other.name )
        
        true_vars = set(self.keys()).intersection(other.keys())
        for var in true_vars:
            mgr[ var ] = self[ var ] + other[ var ]
        
        no_booked = set(self.keys() + other.keys()).difference( true_vars )
        if no_booked:
            sendWarningMsg('%s => The following variables are not '\
                           'booked: %s' %(mgr.name, no_booked))
        
        return mgr

    def __iadd__( self, other ):
        '''
        Allows adding another manager variables to this class
        '''
        if len( self ):
            return self + other
        else:
            self = other.copy()

    def __iter__( self ):
        '''
        Definition of the iterator
        '''
        self._iter = 0
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
        if self._iter == len( self ):
            raise StopIteration
        else:
            self._iter += 1
            return self.getEventDict( self._iter - 1 )
            
    def copy( self, name = '' ):
        '''
        Returns a copy of this class
        '''
        if not name:
            name = self.name + '_copy'
        return DataMgr( name, self )
    
    def getCutList( self, cut, mathmod = math ):
        '''
        This method allows to obtain a list with the events that satisfy the cuts given
        '''
        cut, variables = formatEvalExpr( cut, mathmod )
        varstoadd = [ v for v in variables if v not in self ]
        if varstoadd:
            sendErrorMsg('Need to load additional variables to apply the cuts: %s'
                         %varstoadd)
            return
        values = [ self[ var ] for var in variables ]
        for ivar in xrange( len( variables ) ):
            cut = cut.replace( variables[ ivar ], 'values[ %i ][ ievt ]' %ivar )
        return eval( '[ ievt for ievt in xrange( len( self ) ) if ' + cut + ' ]' )

    def getEntries( self, selection = False, mathmod = math ):
        '''
        Gets the number of entries of the class. If a cut selection is given, it is
        calculated the number of events that satisfy those cuts.
        '''
        if self.keys():
            if selection:
                return len(self.getCutList(selection, mathmod))
            else:
                return len(next(self.itervalues()))
        else:
            sendErrorMsg('Attempting to get entries from an empty data manager')

    def getEventDict( self, ievt ):
        '''
        Returns a dictionary with the values of the variables at the given entry
        '''
        return dict( ( var, values[ ievt ] ) for var, values in self.iteritems() )

    def getEventTuple( self, ievt, *args ):
        '''
        Gets the event at position ievt. Allows to get only the variables in < args >
        in the order specified.
        '''
        if len( args ):
            return tuple( [ self[ var ][ ievt ] for var in args ] )
        else:
            return tuple( [ values[ ievt ] for var, values in self.iteritems() ] )

    def getNvars( self ):
        '''
        Gets the number of variables in the class
        '''
        return len(self.keys())

    def varEvents( self, variables, cuts = False, mathmod = math ):
        '''
        Return a list of lists with the values associated with < variables >. If
        an element in < variables > is a variable, it gets the list of values for
        it. If it is an expression, it returns a list with the corresponding values.
        '''
        
        if cuts:
            entries = self.getCutList( cuts, mathmod )
        else:
            entries = xrange( len( self ) )
        
        fvars    = []
        truevars = []
        for v in variables:
            if v in self:
                fvars.append( v )
            else:
                v, newv = formatEvalExpr( v, mathmod )
                fvars += newv 
            ''' The module is not imported, so the name must change '''
            truevars.append( v.replace( mathmod.__name__, 'mathmod' ) )
        
        fvars = list( set( fvars ) )
        fvars.sort()
        fvars.reverse()
        
        values = [ self[ var ] for var in fvars ]
        output = []
        for arg in truevars:
            for jv, var in enumerate( fvars ):
                arg = arg.replace( var, 'values[ %i ][ ievt ]' %jv )
            output.append( eval( '[ %s for ievt in entries ]' %arg ) )
        cmd = 'output[ 0 ]'
        for i in xrange( 1, len( output ) ):
            cmd += ', output[ %i ]' %i
        
        return eval( cmd )

    def makeVariable( self, varname, arg, function = False ):
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
            self[ varname ] = self.varEvents( arg )

    def newEvent( self, dic ):
        '''
        Adds a new event to the manager. Values for all the variables have to be
        provided.
        '''
        for key, values in self.iteritems():
            values.append( dic[ key ] )

    def display( self, variables = None, cuts = '', mathmod = math, evts = -1, prec = 3 ):
        '''
        Prints the information of the class as well as the values for the first 20
        events. If < evts > is introduced as an input, the number of events showed
        would be that specified by the user. If < cut > is specified only will be
        showed the events that statisfy the given cut. If < prec > is given, the
        number of decimal points it sets to this value.
        '''
        variables = variables or []
        
        if not self:
            sendErrorMsg('%s => No variables booked in this manager' %self.name)
            return
        
        form = '%.' + str( prec ) + 'e'
        if prec:
            prec += 1
        
        ''' If no variables are specified all are printed '''
        if variables == []:
            variables = self.keys()
            variables.sort()
        
        ''' Prints the name of the manager '''
        if self.name:
            lname = 3*len( self.name )
            print '\n' + lname*'*' + '\n' + self.name.center( lname ) + '\n' + lname*'*'
        
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
            evtlst = self.getCutList( cuts, mathmod )
        else:
            evtlst = xrange( len( self ) )

        if evts != -1:
            i = 0
            for ievt in evtlst:
                if i == evts: break
                i += 1
                vout = '| '
                for var in self.getEventTuple( ievt, *variables ):
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
                for var in self.getEventTuple( ievt, *variables ):
                    vout += ( form %var ).rjust( maxsize ) + ' |'
                print vout
            if ievt + 1 == len( evtlst ):
                print deco + '\n'

    def runCutEntries( self, var,
                       sense    = '>',
                       npoints  = 100,
                       vmin     = None,
                       vmax     = None,
                       endpoint = True ):
        '''
        Return a list with the numbers of elements satisfying a given cut, running
        from < vmin > to < vmax > in < npoints >.
        '''
        if sense not in ('>', '>=', '<', '<='):
            sendErrorMsg('Unable to parse < %s > as a sense-like symbol')
            return

        values = self[var]
        
        if vmin == None:
            vmin = min(values)
        if vmax == None:
            vmax = max(values)

        cuts = np.linspace(vmin, vmax, npoints, endpoint = endpoint)
        
        var += sense

        points = []
        for ic in cuts:
            ct = var + str(ic)
            points.append(self.getEntries(ct))

        return points
    
    def save( self, name = '', tree_name = False, ftype = 'root', variables = None, close = True ):
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
        variables = variables or self.keys()
        if ftype in ( 'root', 'Root', 'ROOT' ):
            if name != '':
                ofile = rt.TFile.Open( name, 'RECREATE' )
            else:
                ofile = False
                name  = rt.gDirectory.GetName()
            print self.name, '=> Saving tree with name <', tree_name, '> in <', name, '>'
            dictToTree( self, name = tree_name, variables = variables )
            print self.name, '=> Written', len( self ), 'entries'
            if ofile and close:
                ofile.Close()
            else:
                return ofile
        elif ftype in ( 'txt', 'TXT' ):
            ofile = open( name, 'wt' )
            print self.name, '=> Saving txt data in file <', name, '>'
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
    
    def subSample( self, name = '', cuts = '', mathmod = math, evts = -1, varset = '*' ):
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
            name = self.name + '_SubSample'
        if 'cuts' != '':
            evtlst = self.getCutList( cuts, mathmod )
        else:
            evtlst = evts
            
        cmgr = DataMgr( name )
        for kw in varset:
            vlist = self[ kw ]
            cmgr[ kw ] = [ vlist[ i ] for i in evtlst if i in evts ]
        cmgr.Nentries = len( next( cmgr.itervalues() ) )
        
        return cmgr


def txtToDict( fname, tnames = None, colid = None ):
    '''
    Creates a new dictionary containing the values of the variables stored on
    a txt file. The file path is specified in < fname >, while the names of the
    variables are given as a list in < tnames >. As a keyword argument, the column
    index to be read has to be given as colid = [ 1, 3, 4, ... ]. If < tnames >
    is provided, and the first row has the names of the variables, it has
    preference over the < colid > variable. In the case where the first row
    does not have the names, < tnames > and < colid > must match.
    '''
    tnames = tnames or []
    colid  = colid or []
    
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
        sendErrorMsg('The first line of the input file has not the correct format')
        return
    else:
        if not tnames:
            sendErrorMsg('The names of the variables in the colid have to be specified')
            return
        elif len( tnames ) != len( colid ):
            sendErrorMsg('The names of the variables and the column index must match')
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
                sendErrorMsg('Format for column < %s > not recognised' %i)
        varvalues.append( [ convfuncs[ -1 ]( value ) ] )
    for line in ifile:
        line = line.split()
        for index, icol in enumerate( colid ):
            varvalues[ index ].append( convfuncs[ index ]( line[ icol ] ) )
    ifile.close()
    return { name: varvalues[ index ] for index, name in enumerate( tnames ) }


def varsInRootTree( tree = None, fname = '', tpath = '', regexps = None ):
    '''
    Return variables in a tree. If < regexps > are provided, only variables
    matching it will be returned.
    '''
    regexps = regexps or []
    
    if not tree:
        rfile = rt.TFile.Open( fname )
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
                sendWarningMsg('No variables found matching '\
                               'expression < %s >' %expr)
        return truenames
    else:
        return brnames
