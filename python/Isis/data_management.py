#/////////////////////////////////////////////////////////////
#//
#//  Python modules
#//
#// ----------------------------------------------------------
#//
#//  AUTHOR: Miguel Ramos Pernas
#//  e-mail: miguel.ramos.pernas@cern.ch
#//
#//  Last update: 20/09/2017
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

import iroot as rt

from Isis.iboost.general import sendErrorMsg, sendWarningMsg
from Isis.iboost.rootio import treeToArray, arrayToTree
from Isis.utils import string_list_filter
from Isis.expressions import numpy_parse_eval_expr


class DataMgr( pandas.DataFrame ):
    '''
    Class to manage data, specially designed to work together with Root files
    and trees
    '''
    def __init__( self, name = '<unnamed>', **kwargs ):
        '''
        Constructor mainly inherited from pandas.DataFrame. Also a name
        is stored.
        '''
        pandas.DataFrame.__init__(self, **kwargs)
        
        self.name = name

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
        mgr = DataMgr(data = m, name = new_name)
        
        return mgr
    
    def __iadd__( self, other ):
        '''
        Allows adding another manager variables to this class
        '''
        if len(self):
            self = self + other
        else:
            self = other.copy()
            
    def copy( self, name = None, **kwargs ):
        '''
        Returns a copy of this class
        '''
        name = name or self.name + '_copy'

        m = pandas.DataFrame.copy(self, **kwargs)
        
        m.name = name
        
        return m
    
    def evaluate( self, expr, mathmod = None ):
        '''
        Evaluate the given expression
        '''
        mathmod = mathmod or numpy
        
        pexpr = numpy_parse_eval_expr(expr, mathmod)

        rc = list(reversed(sorted(self.columns)))
        
        for i, v in enumerate(rc):
            pexpr = pexpr.replace(v, 'self["{{{}}}"]'.format(i))
        pexpr = pexpr.format(*rc)

        try:
            return eval(pexpr)
        except SyntaxError:
            sendErrorMsg('Unable to evaluate expression "{}", which '\
                         'transforms into "{}"'.format(expr, pexpr))
            
    def cutidxs( self, cut, mathmod = None ):
        '''
        This method allows to obtain a list with the events that satisfy
        the cuts given
        '''
        if cut.strip():
            mask = self.evaluate(cut, mathmod)
            return numpy.nonzero(mask)[0]
        else:
            return numpy.array(self.index)

    def entries( self, selection = False, mathmod = None ):
        '''
        Gets the number of entries of the class. If a cut selection is
        given, it is calculated the number of events that satisfy those cuts.
        '''
        if self.keys():
            if selection:
                return numpy.count_nonzero(self.cut_mask(selection, mathmod))
            else:
                return len(self)
        else:
            sendErrorMsg('Attempting to get entries from an empty data manager')

    def make_var( self, name, arg, function = False, mathmod = None ):
        '''
        Makes another variable using those in the class. There are two
        different ways to do it. The first one consists on specifying the
        new variable name, the name of the variables used by the function
        (ordered in a list) and the function itself. The computation of
        the new variable is going to be performed passing the variables to
        the function as normal entries ( *args, where args is the list of
        values ). The second method consists on specifying only the name
        of the variable and an expression in < arg >. The values will be
        processed then for each entry taking into account the value
        obtained when evaluating the expression.
        '''
        if function:
            v = numpy.array(function(self[arg]))
        else:
            v = self.evaluate(arg, mathmod)
        self[name] = v

    def nvars( self ):
        '''
        Gets the number of variables in the class
        '''
        return len(self.columns)

    def display( self, variables = None, cuts = '', mathmod = None, evts = None, prec = 3 ):
        '''
        Prints the information of the class as well as the values for the
        first 20 events. If < evts > is introduced as an input, the number
        of events showed would be that specified by the user. If < cut > is
        specified only will be showed the events that statisfy the given
        cut. If < prec > is given, the number of decimal points it sets to
        this value.
        '''
        variables = variables or []
        
        if not self.columns:
            sendErrorMsg('{} => No variables booked in this manager'.format(self.name))
            return
        
        # If no variables are specified all are printed
        if variables == []:
            variables = list(self.columns)
            variables.sort()
        
        # Prints the name of the manager
        if self.name:
            ln  = 3*len(self.name)
            out = '\n{0}\n{1:^{2}}\n{0}'
            sep = ln*'*'
            print out.format(sep, self.name, ln)
        
        # Prints the variables. The variable < maxsize > is the maximum size
        # of the numbers in the print
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
        
        # Prints the values of the variables
        evtlst = self.cutidxs(cuts, mathmod)

        form = '{:' + str(maxsize) + '.' + str(prec) + 'e}'

        buildLine = lambda ievt: '| {} |'.format(
            ' | '.join(form.format(v) for v in self.iloc[ievt][variables])
        )
        
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
        
    @staticmethod
    def from_root( path, tname, columns = None, name = '<unnamed>', cuts = '', regex = False ):
        '''
        Create a DataMgr instance from a root file
        '''
        cols = columns or ['*']
        
        v = treeToArray(path, tname, cols, cuts, regex)
        
        return DataMgr(data = v, columns = cols, name = name)
                       
    def run_cut( self, var,
                 sense    = '>',
                 npoints  = 100,
                 vmin     = None,
                 vmax     = None,
                 endpoint = True ):
        '''
        Return a list with the numbers of elements satisfying a given cut,
        running from < vmin > to < vmax > in < npoints >.
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
        
        points = np.array(npoints*[0])
        for ic in cuts:
            ct = var + str(ic)
            points[ic] = self.entries(ct)
        
        return points
        
    def subsample( self, cuts = '', mathmod = None, evts = None, columns = None, name = None ):
        '''
        Returns a copy of this class satisfying the given requirements. A set
        of cuts can be specified. The range of the events to be copied can be
        specified, as well as the variables to be copied. By default the
        entire class is copied.
        '''
        columns = columns or self.columns
        name    = name or self.name + '_subsample'
        
        if evts != None:
            if isinstance(evts, collections.Iterable):
                evts = numpy.array(evts)
            else:
                evts = self.index[:evts]
        else:
            evts = self.index
            
        cut_lst = self.cutidxs(cuts, mathmod)
        
        evtlst = numpy.intersect1d(evts, cut_lst)
        
        cmgr = DataMgr(data = self[columns].iloc[evtlst], name = name)
        
        return cmgr

    def to_root( self, name = '', tname = False, columns = None, mode = 'recreate', use_regex = False ):
        '''
        Save this instance in a Root file
        '''
        columns = columns or list(self.columns)
        
        if name != '':
            ofile = rt.TFile.Open(name, mode)
        else:
            ofile = False
            name  = rt.gDirectory.GetName()
            
        print self.name, '=> Saving tree with name <', tname, '> in <', name, '>'

        v = self.to_records(index = False)
        
        arrayToTree(v, name = tname, variables = columns, use_regex = use_regex)
        print self.name, '=> Written', len(self), 'entries'
        
        if ofile:
            ofile.Close()


def vars_in_root_tree( tree = None, fname = '', tpath = '', regexps = None ):
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
            addlst = string_list_filter(brnames, expr)
            if addlst != []:
                truenames += addlst
            else:
                sendWarningMsg('No variables found matching '\
                               'expression < {} >'.format(expr))
        return truenames
    else:
        return brnames
