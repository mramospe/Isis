#/////////////////////////////////////////////////////////////
#//
#//  Python modules
#//
#// ----------------------------------------------------------
#//
#//  AUTHOR: Miguel Ramos Pernas
#//  e-mail: miguel.ramos.pernas@cern.ch
#//
#//  Last update: 20/04/2017
#//
#// ----------------------------------------------------------
#//
#//  Description:
#//
#//  Classes and functions to parse expressions and modify
#//  them
#//
#// ----------------------------------------------------------
#/////////////////////////////////////////////////////////////


import __builtin__
import math
import numpy


class EvalExpr:

    def __init__( self, expr, mathmod = math ):
        '''
        This class allows to format a given expression in such a way that takes
        into account the mathematical functions and the logical operators. The module
        containing the mathematical functions can be specified.
        '''

        self.expr      = ''
        self.variables = []
        self.consts    = []
        
        proc_expr = translateCExpr( expr )
        variables = proc_expr
        for el in ( '==', '!=', '<=', '>=', '>', '<',
                    'and', 'or', 'not', '(', ')',
                    '*', '/', '+', '-',
                    '!', ',' ):
            variables = variables.replace( el, '|' )
        variables = variables.replace( ' ', '' )
    
        ''' Splits the elements, so only the variables and the numbers remain '''
        variables = variables.split( '|' )
        while '' in variables:
            variables.remove( '' )

        '''
        Iterates over the expression to find the variables and the constants in it. The use
        of a < while > loop becomes necessary to avoid replacing multiple times the same
        function by < module.function >.
        '''
    
        truevars = []
        fmlist   = dir( mathmod )
        fblist   = dir( __builtin__ )
        mathmod  = mathmod.__name__ + '.'
    
        idx    = 0
        length = len( variables )

        while idx < length:
        
            el = variables[ idx ]

            isfloat = False
        
            ''' These lines allow the management of float values given with an < e/E > '''
            epos = el.find( 'e' )
            Epos = el.find( 'E' )
            dec1 = ( epos != -1 )
            dec2 = ( Epos != -1 )
            if ( dec1 or dec2 ) and not ( dec1 and dec2 ):
                try:
                    float( el )
                    isfloat = True
                except:
                    ''' The number may not be a float '''
                    if dec1:
                        left, right = el[ :epos ], el[ epos + 1: ]
                    else:
                        left, right = el[ :Epos ], el[ Epos + 1: ]
                    try:
                        float( left )
                        if right:
                            sendErrorMsg('Unable to parse expression < %s >' %el)
                            return
                        else:
                            try:
                                '''
                                The < 0 > is added since it could find a
                                string like < 1e >
                                '''
                                float( variables[ idx + 1 ] + '0' )
                            except:
                                sendErrorMsg('Unable to parse expression; error '\
                                             'in floating constant')
                                return
                        isfloat = True
                    except:
                        isfloat = False
            else:
                ''' Here it determines if the element is a number or a variable '''
                try:
                    float( el )
                    isfloat = True
                except:
                    it = el[ 0 ]
                    if it.isdigit() or it == '.':
                        sendErrorMsg('Unable to parse expression < %s >' %el)
                        return
                    else:
                        isfloat = False

            '''
            If it is a float it continues the loop. If it is an expression, tries
            to find it in the builtin and math modules.
            '''
            if isfloat:
                self.consts.append(el)
                idx += 1
            else:
                if el in fmlist:
                    nc = 0
                    while el in variables:
                        variables.remove( el )
                        nc += 1
                    length -= nc
                    proc_expr = proc_expr.replace( el, mathmod + el )
                else:
                    idx += 1
                    if el not in fblist:
                        truevars.append( el )
    
        '''
        Sorting the list on a reversed way is necessary to prevent missreplacement of
        the variables
        '''
        truevars.sort()
        truevars.reverse()

        self.variables = truevars
        self.expr = proc_expr


class NumpyEvalExpr(EvalExpr):
    '''
    Transform an expression to be evaluated using numpy ndarray objects
    '''

    changes = {' and ': 'np.logical_and',
               ' or ' : 'np.logical_or',
               '^'    : 'np.logical_xor',
               'not ' : 'np.logical_not'}
    
    def __init__( self, expr, mathmod = numpy ):
        '''
        Build this class from an expression and the math module to be
        used. The constructor of the base class is called to do the first
        processing of the expression. Then replacements are done to
        introduce the numpy logical expressions in it.
        '''
        EvalExpr.__init__(self, expr, mathmod)
        
        self.comp = {}

        expr = self.expr

        '''
        Create new expressions from those between parentheses. Needed
        since no simple-string replacement is possible here.
        '''
        while len(expr):

            op = expr.find('(') + 1
            if op == 0:
                break
            else:
                counter = 1
                for cp, s in enumerate(expr[op:]):
                    if s == '(':
                        counter += 1
                    elif s == ')':
                        counter -= 1
                    if counter == 0:
                        app = expr[op:op + cp]
                        self.comp[app] = NumpyEvalExpr(app)
                        expr = expr[op + cp + 1:]
                        break

        ''' Reset the expression '''
        expr = self.expr
                    
        for name, el in sorted(self.comp.iteritems()):
            
            expr = expr.replace(name, el.expr)
            
            if el.expr != name:
                while name in expr:
                    expr = expr.replace(name, el.expr)
                    
        self.expr = self._process_expr(expr)
        
    def _process_expr( self, expr ):
        '''
        Process one expression within this class
        '''
        pos = { expr.find(' and ') : ' and ',
                expr.find(' or ')  : ' or ',
                expr.find('^')     : '^'
        }

        ''' Not is an special case, since it can be preceded by another conditional '''
        pos_not = expr.find('not ')
        while pos_not not in (-1, 0) and expr[pos_not - 1] != 0:
            pos_not = expr.find('not ', pos_not + 1)
        pos[pos_not] = 'not '
        
        for p, c in sorted(pos.iteritems()):
            if p != -1:
                
                right_expr = expr[p + len(c):]
                proc_expr  = self._process_expr(right_expr)
                
                if c == 'not ':
                    expr = '%s(%s)' %(NumpyEvalExpr.changes[c],
                                      proc_expr)
                else:
                    left_expr  = expr[:p]
                    expr = '%s(%s, %s)' %(NumpyEvalExpr.changes[c],
                                          left_expr, proc_expr)
                return expr
            
        return expr


def translateCExpr( expr ):
    '''
    Translates a C expression into a python expression
    '''
    expr = expr.replace( '&&' , ' and ' )
    expr = expr.replace( '||' , ' or '  )
    expr = expr.replace( 'abs', 'fabs'  )
    expr = expr.replace( '!=', '%%%' )
    expr = expr.replace( '!', ' not ' )
    expr = expr.replace( '%%%', '!=' )
    return expr
