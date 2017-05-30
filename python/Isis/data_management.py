#/////////////////////////////////////////////////////////////
#//
#//  Python modules
#//
#// ----------------------------------------------------------
#//
#//  AUTHOR: Miguel Ramos Pernas
#//  e-mail: miguel.ramos.pernas@cern.ch
#//
#//  Last update: 30/05/2017
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


import collections
import math
import numpy as np

import ROOT as rt

from Isis.iboost.general import sendErrorMsg, sendWarningMsg
from Isis.iboost.rootio import treeToDict, treeToList, dictToTree, listToTree
from Isis.utils import joinDicts, mergeDicts, stringListFilter
from Isis.expressions import NumpyEvalExpr


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
                    sendErrorMsg('{} => Unknown input file type < {} >'.format(name, ftype))
            else:
                sendErrorMsg('{} => The input file type must be specified'.format(name))
        else:
            for kw, lst in path.iteritems():
                self[ kw ] = np.array(lst)
                
            self._iter = 0
            self.name  = name
        
            wrong = ( len( set( len( lst ) for lst in self.itervalues() ) ) > 1 )
            if wrong:
                sendErrorMsg('{} => The lists stored in the manager '\
                             'have different lengths'.format(self.name))

    def __add__( self, other ):
        '''
        Allows merging two objects of this class
        '''
        mgr = DataMgr(self.name + '__' + other.name)
        
        true_vars = set(self.keys()).intersection(other.keys())
        for var in true_vars:
            mgr[ var ] = np.concatenate((self[var], other[var]))
        
        no_booked = set(self.keys() + other.keys()).difference(true_vars)
        if no_booked:
            sendWarningMsg('{} => The following variables are not '\
                           'booked: {}'.format(mgr.name, no_booked))
        
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
        return len(next(self.itervalues()))

    def next( self ):
        '''
        Sets the new value for the iteration. If it reaches the limit the exception
        is raised.
        '''
        if self._iter == len(self):
            raise StopIteration
        else:
            self._iter += 1
            return self.getEventDict(self._iter - 1)
            
    def copy( self, name = '' ):
        '''
        Returns a copy of this class
        '''
        if not name:
            name = self.name + '_copy'
        return DataMgr(name, self)

    def cutMask( self, cut, mathmod = np ):
        '''
        Return the mask associated with the events passing the given cut
        '''
        eval_expr = NumpyEvalExpr(cut, mathmod)
        cut       = eval_expr.expr

        if cut:
            variables = eval_expr.variables
            varstoadd = [v for v in variables if v not in self]
        
            if varstoadd:
                sendErrorMsg('Need to load additional variables to '\
                             'apply the cuts: {}'.format(varstoadd))
        
            values = [self[var] for var in variables]
        
            for i, var in enumerate(variables):
                cut = cut.replace(var, 'values[{}]'.format(i))

            return eval(cut)

        else:
            return np.ones(len(self))
    
    def cutIdxs( self, cut, mathmod = np ):
        '''
        This method allows to obtain a list with the events that satisfy the cuts given
        '''
        mask = self.cutMask(cut, mathmod)
        return np.nonzero(mask)[0]

    def entries( self, selection = False, mathmod = np ):
        '''
        Gets the number of entries of the class. If a cut selection is given, it is
        calculated the number of events that satisfy those cuts.
        '''
        if self.keys():
            if selection:
                return np.count_nonzero(self.cutMask(selection, mathmod))
            else:
                return len(self)
        else:
            sendErrorMsg('Attempting to get entries from an empty data manager')

    def getEventDict( self, ievt ):
        '''
        Returns a dictionary with the values of the variables at the given entry
        '''
        return dict((var, values[ ievt ]) for var, values in self.iteritems())

    def getEventTuple( self, ievt, *args ):
        '''
        Gets the event at position ievt. Allows to get only the variables in < args >
        in the order specified.
        '''
        if len( args ):
            return tuple([self[var][ievt] for var in args])
        else:
            return tuple([values[ievt] for var, values in self.iteritems()])

    def getNvars( self ):
        '''
        Gets the number of variables in the class
        '''
        return len(self.keys())

    def varEvents( self, variables, cuts = False, mathmod = np ):
        '''
        Return a list of lists with the values associated with < variables >. If
        an element in < variables > is a variable, it gets the list of values for
        it. If it is an expression, it returns a list with the corresponding values.
        '''
        
        if cuts:
            entries = self.cutMask(cuts, mathmod)
        else:
            entries = np.ones(self.entries(), dtype = bool)
            
        fvars    = []
        truevars = []
        for v in variables:
            if v in self:
                fvars.append( v )
            else:
                sendErrorMsg('Unknown variable < %s >')
        
        fvars = list(set(fvars))
        fvars.sort()
        fvars.reverse()
        
        values = [self[var][entries] for var in fvars]
        
        return values

    def makeVariable( self, varname, arg, mathmod = np, function = False ):
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
            var_tensor = [self[v] for v in arg]
            lvars      = xrange(len(arg))
            new_var    = function(*var_tensor)
            self[varname] = np.array(new_var)
        else:
            eval_expr = NumpyEvalExpr(arg, mathmod = mathmod)
            expr      = eval_expr.expr
            variables = eval_expr.variables

            for var in variables:
                expr = expr.replace(var, "self['{}']".format(var))

            self[varname] = eval(expr)

    def newEvent( self, dic ):
        '''
        Adds a new event to the manager. Values for all the variables have to be
        provided.
        '''
        for key, values in self.iteritems():
            values.append(dic[key])

    def display( self, variables = None, cuts = '', mathmod = np, evts = None, prec = 3 ):
        '''
        Prints the information of the class as well as the values for the first 20
        events. If < evts > is introduced as an input, the number of events showed
        would be that specified by the user. If < cut > is specified only will be
        showed the events that statisfy the given cut. If < prec > is given, the
        number of decimal points it sets to this value.
        '''
        variables = variables or []
        
        if not self:
            sendErrorMsg('{} => No variables booked in this manager'.format(self.name))
            return
        
        ''' If no variables are specified all are printed '''
        if variables == []:
            variables = self.keys()
            variables.sort()
        
        ''' Prints the name of the manager '''
        if self.name:
            ln  = 3*len(self.name)
            out = '\n{0}\n{1:^{2}}\n{0}'
            sep = ln*'*'
            print out.format(sep, self.name, ln)
        
        '''
        Prints the variables. The variable < maxsize > is the maximum size of the
        numbers in the print
        '''
        maxsize   = 8 + prec
        shortvars = []
        for var in variables:
            if len(var) > maxsize:
                var = var[:maxsize] + '*'
            shortvars.append('{0:^{1}}'.format(var, maxsize))
            
        print '{}: {}'.format('Variables', ', '.join(variables))
        
        vout = '| {} |'.format(' | '.join(shortvars))
        deco = len(vout)*'='
        print '{0}\n{1}\n{0}'.format(deco, vout)
        
        ''' Prints the values of the variables '''
        if cuts != '':
            evtlst = self.cutIdxs(cuts, mathmod)
        else:
            evtlst = np.arange(len(self))

        form = '{:' + str(maxsize) + '.' + str(prec) + 'e}'

        buildLine = lambda ievt: '| {} |'.format(
            ' | '.join(form.format(v)
                       for v in self.getEventTuple(ievt, *variables)))
        
        if evts != None:
            for ievt in evtlst[:evts]:
                print buildLine(ievt)
            print '{}\n'.format(deco)
        else:
            for ievt in evtlst:
                if ievt and ievt % 20 == 0:
                    if raw_input(
                        '< Introduce q to exit, and any other input to continue printing >: '
                        ) == 'q': break
                print buildLine(ievt)
            if ievt + 1 == len(evtlst):
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
            sendErrorMsg('Unable to parse < {} > as a sense-like symbol'.format(sense))
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
            points.append(self.entries(ct))

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
        if ftype in ('root', 'Root', 'ROOT'):
            if name != '':
                ofile = rt.TFile.Open(name, 'RECREATE')
            else:
                ofile = False
                name  = rt.gDirectory.GetName()
            print self.name, '=> Saving tree with name <', tree_name, '> in <', name, '>'
            dictToTree(self, name = tree_name, variables = variables)
            print self.name, '=> Written', len(self), 'entries'
            if ofile and close:
                ofile.Close()
            else:
                return ofile
        elif ftype in ('txt', 'TXT'):
            ofile = open(name, 'wt')
            print self.name, '=> Saving txt data in file <', name, '>'
            varvalues = [ self[ var ] for var in variables ]
            out = ''
            for var in variables:
                out += var + '\t'
            ofile.write(out[ :-1 ] + '\n')
            for ievt in xrange(len(self)):
                out = ''
                for var in varvalues:
                    out += str(var[ ievt ]) + '\t'
                ofile.write(out[ :-1 ] + '\n')
            if close:
                ofile.close()
            else:
                return ofile
    
    def subSample( self, name = '', cuts = '', mathmod = np, evts = None, varset = '*' ):
        '''
        Returns a copy of this class satisfying the given requirements. A set
        of cuts can be specified. The range of the events to be copied can be
        specified, as well as the variables to be copied. By default the
        entire class is copied.
        ''' 
        if varset == '*':
            varset = self.keys()
        if name == '':
            name = self.name + '_SubSample'
        
        if evts != None:
            if isinstance(evts, collections.Iterable):
                evts = np.array(evts)
            else:
                evts = np.arange(evts)
        else:
            evts = np.arange(len(self))
            
        if 'cuts' != '':
            cut_lst = self.cutIdxs(cuts, mathmod)
        else:
            cut_lst = np.arange(len(self))

        evtlst = np.intersect1d(evts, cut_lst)
        
        cmgr = DataMgr(name)
        for v in varset:
            cmgr[v] = self[v][evtlst]

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
    
    ifile   = open(fname, 'rt')
    line    = ifile.readline().split()
    if colid == []:
        colid = range(len(line))
    if all( isinstance(line[i], str) for i in colid):
        if tnames and tnames != [ '*' ]:
            colid = [colid[i] for i in colid if line[i] in tnames]
        else:
            tnames = [line[i] for i in colid]
        line = ifile.readline().split()
    elif any(isinstance(line[i], str) for i in colid):
        sendErrorMsg('The first line of the input file has not the correct format')
        return
    else:
        if not tnames:
            sendErrorMsg('The names of the variables in the colid have to be specified')
            return
        elif len( tnames ) != len(colid):
            sendErrorMsg('The names of the variables and the column index must match')
            return
    convfuncs, varvalues = [], []
    for index, icol in enumerate(colid):
        value = line[ icol ]
        try:
            int( value )
            convfuncs.append(int)
            isint = True
        except:
            try:
                float(value)
                convfuncs.append(float)
            except:
                sendErrorMsg('Format for column < {} > not recognised'.format(i))
        varvalues.append([convfuncs[-1](value)])
    for line in ifile:
        line = line.split()
        for index, icol in enumerate(colid):
            varvalues[index].append(convfuncs[index](line[icol]))
    ifile.close()
    return {name: varvalues[index] for index, name in enumerate(tnames)}


def varsInRootTree( tree = None, fname = '', tpath = '', regexps = None ):
    '''
    Return variables in a tree. If < regexps > are provided, only variables
    matching it will be returned.
    '''
    regexps = regexps or []
    
    if not tree:
        rfile = rt.TFile.Open(fname)
        tree  = rfile.Get(tpath)
    brnames = [el.GetName() for el in tree.GetListOfBranches()]
    if not tree:
        rfile.Close()
    if regexps != []:
        truenames = []
        for expr in regexps:
            addlst = StringListFilter(brnames, expr)
            if addlst != []:
                truenames += addlst
            else:
                sendWarningMsg('No variables found matching '\
                               'expression < {} >'.format(expr))
        return truenames
    else:
        return brnames
