#/////////////////////////////////////////////////////////////
#//
#//  Python modules
#//
#// ----------------------------------------------------------
#//
#//  AUTHOR: Miguel Ramos Pernas
#//  e-mail: miguel.ramos.pernas@cern.ch
#//
#//  Last update: 12/09/2017
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
import numpy
import pandas

import ROOT as rt

from Isis.iboost.general import sendErrorMsg, sendWarningMsg
from Isis.iboost.rootio import treeToDict, treeToList, dictToTree, listToTree
from Isis.utils import joinDicts, mergeDicts, stringListFilter
from Isis.expressions import NumpyEvalExpr


class DataMgr( pandas.DataFrame ):
    '''
    Class to manage data, specially designed to work together with Root files
    and trees
    '''
    def __init__( self, data = None, index = None, columns = None, dtype = None, copy = False, name = '<unnamed>' ):
        '''
        Constructor mainly inherited from pandas.DataFrame. Also a name is stored.
        '''
        pandas.DataFrame.__init__(self, data, index, columns, dtype, copy)
            
        self.name  = name

    def __add__( self, other ):
        '''
        Allows merging two objects of this class
        '''
        new_name  = self.name + '__' + other.name
        
        no_booked = set(self.columns).symmetric_difference(other.columns)
        if no_booked:
            sendWarningMsg('{} => The following variables are not being '\
                           'booked: {}'.format(mgr.name, no_booked))
            
        v   = list(set(self.columns).intersection(other.columns))
        m   = pandas.DataFrame.__add__(self[v], other[v])
        mgr = DataMgr(m, name = new_name)
        
        return mgr
    
    def __iadd__( self, other ):
        '''
        Allows adding another manager variables to this class
        '''
        if len(self):
            self = self + other
        else:
            self = other.copy()
            
    def copy( self, name = '', deep = True ):
        '''
        Returns a copy of this class
        '''
        if not name:
            name = self.name + '_copy'
        return DataMgr(self, name = name, copy = deep)
    
    def cutMask( self, cut, mathmod = None ):
        '''
        Return the mask associated with the events passing the given cut
        '''
        eval_expr = NumpyEvalExpr(cut, mathmod)
        cut       = eval_expr.expr
        
        if cut:
            variables = eval_expr.variables
            varstoadd = [v for v in variables if v not in self.columns]
        
            if varstoadd:
                sendErrorMsg('Need to load additional variables to '\
                             'apply the cuts: {}'.format(varstoadd))
        
            values = [self[var] for var in variables]
            
            for i, var in enumerate(variables):
                cut = cut.replace(var, 'values[{}]'.format(i))

            return eval(cut)

        else:
            return numpy.ones(len(self))
    
    def cutIdxs( self, cut, mathmod = None ):
        '''
        This method allows to obtain a list with the events that satisfy the cuts given
        '''
        mask = self.cutMask(cut, mathmod)
        return numpy.nonzero(mask)[0]

    def entries( self, selection = False, mathmod = None ):
        '''
        Gets the number of entries of the class. If a cut selection is given, it is
        calculated the number of events that satisfy those cuts.
        '''
        if self.keys():
            if selection:
                return numpy.count_nonzero(self.cutMask(selection, mathmod))
            else:
                return len(self)
        else:
            sendErrorMsg('Attempting to get entries from an empty data manager')

    def nvars( self ):
        '''
        Gets the number of variables in the class
        '''
        return len(self.columns())

    def varEvents( self, variables, cuts = False, mathmod = None ):
        '''
        Return a list of lists with the values associated with < variables >. If
        an element in < variables > is a variable, it gets the list of values for
        it. If it is an expression, it returns a list with the corresponding values.
        '''
        
        if cuts:
            entries = self.cutMask(cuts, mathmod)
        else:
            entries = numpy.ones(self.entries(), dtype = bool)
            
        fvars = []
        for v in variables:
            if v in self.columns:
                fvars.append(v)
            else:
                sendErrorMsg('Unknown variable < {} >'.format(v))
        
        fvars = list(set(fvars))
        fvars.sort()
        fvars.reverse()
        
        values = self[fvars][entries]
        
        return values

    def makeVariable( self, varname, arg, mathmod = None, function = False ):
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
            var_tensor    = self[arg]
            new_var       = function(var_tensor)
            self[varname] = numpy.array(new_var)
        else:
            eval_expr = NumpyEvalExpr(arg, mathmod = mathmod)
            expr      = eval_expr.expr
            variables = eval_expr.variables
            
            for var in variables:
                expr = expr.replace(var, "self['{}']".format(var))
                
            self[varname] = eval(expr)

    def display( self, variables = None, cuts = '', mathmod = None, evts = None, prec = 3 ):
        '''
        Prints the information of the class as well as the values for the first 20
        events. If < evts > is introduced as an input, the number of events showed
        would be that specified by the user. If < cut > is specified only will be
        showed the events that statisfy the given cut. If < prec > is given, the
        number of decimal points it sets to this value.
        '''
        variables = variables or []
        
        if not self.columns:
            sendErrorMsg('{} => No variables booked in this manager'.format(self.name))
            return
        
        ''' If no variables are specified all are printed '''
        if variables == []:
            variables = list(self.columns)
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
            evtlst = numpy.arange(len(self))

        form = '{:' + str(maxsize) + '.' + str(prec) + 'e}'

        buildLine = lambda ievt: '| {} |'.format(
            ' | '.join(form.format(v) for v in self.iloc[ievt][variables]
        
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

        cuts = numpy.linspace(vmin, vmax, npoints, endpoint = endpoint)
        
        var += sense

        points = []
        for ic in cuts:
            ct = var + str(ic)
            points.append(self.entries(ct))

        return points
    
    def from_root( path, tname, columns = None, name = '<unnamed>' ):
        '''
        Create a DataMgr instance from a root file
        '''
        cols = columns or ['*']
        
        d = treeToDict(path, tname, cols)
        
        if not columns:
            cols = sorted(d.keys())
            
        v = numpy.array(list(dic[v] for v in cols))
        
        return DataMgr(v, variables = cols, name = name)
        
    def subSample( self, name = None, cuts = '', mathmod = None, evts = None, columns = None ):
        '''
        Returns a copy of this class satisfying the given requirements. A set
        of cuts can be specified. The range of the events to be copied can be
        specified, as well as the variables to be copied. By default the
        entire class is copied.
        '''
        columns = columns or self.columns
        name    = name or self.name + '_SubSample'
        
        if evts != None:
            if isinstance(evts, collections.Iterable):
                evts = numpy.array(evts)
            else:
                evts = numpy.arange(evts)
        else:
            evts = numpy.arange(len(self))
            
        if 'cuts' != '':
            cut_lst = self.cutIdxs(cuts, mathmod)
        else:
            cut_lst = numpy.arange(len(self))

        evtlst = numpy.intersect1d(evts, cut_lst)
        
        cmgr = DataMgr(self[columns][evtlst], name = name)
        
        return cmgr

    def to_root( self, name = '', tname = False, columns = None, mode = 'recreate' ):
        '''
        Save this instance in a Root file
        '''
        columns = columns or self.columns
        
        if name != '':
            ofile = rt.TFile.Open(name, mode)
        else:
            ofile = False
            name  = rt.gDirectory.GetName()
            
        print self.name, '=> Saving tree with name <', tree_name, '> in <', name, '>'
        dictToTree(self.to_dict('series'), name = tree_name, variables = columns)
        print self.name, '=> Written', len(self), 'entries'
        
        if ofile:
            ofile.Close()


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
            addlst = stringListFilter(brnames, expr)
            if addlst != []:
                truenames += addlst
            else:
                sendWarningMsg('No variables found matching '\
                               'expression < {} >'.format(expr))
        return truenames
    else:
        return brnames
