#/////////////////////////////////////////////////////////////
#//
#//  Python modules
#//
#// ----------------------------------------------------------
#//
#//  AUTHOR: Miguel Ramos Pernas
#//  e-mail: miguel.ramos.pernas@cern.ch
#//
#//  Last update: 13/09/2017
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


from Isis.iboost.general import sendErrorMsg
import math, inspect, numpy, re


# Global variable storing the conversion from python conditionals
# to numpy functions
numpy_conditionals = {' and ': 'numpy.logical_and',
                      ' or ' : 'numpy.logical_or',
                      '^'    : 'numpy.logical_xor',
                      'not ' : 'numpy.logical_not'}


def check_parentheses( expr ):
    '''
    Check the open and close parentheses in the given expression
    '''
    it = ParIter()
    for s in expr:
        it.signal(s)
    return it.signal()

        
class ParIter:
    '''
    Dummy class to iterate over items inside two parentheses
    '''
    def __init__( self, idx = 0, obj = 0 ):
        '''
        The index refering to the nesting level must be given
        '''
        self.counter   = idx
        self.objective = obj
        
    def restart( self, idx = 1 ):
        '''
        Build the class again
        '''
        self.__init__(idx)

    def signal( self, char = None ):
        '''
        Check the given character and change the counter accordingly. Return
        True if the objective is reached, False otherwise.
        '''
        if char == '(':
            self.counter += 1
        elif char == ')':
            self.counter -= 1
        
        d = (self.counter == self.objective)
        
        return d

    
def parse_eval_expr( expr, mathmod = None ):
    '''
    Function to parse an expression and substitute the members in
    "mathmod" by "mathmomd.<some function>".
    '''
    mathmod = mathmod or math
        
    pexpr = translate_cexpr(expr)
    mname = mathmod.__name__ + '.'
    memb  = numpy.array(inspect.getmembers(mathmod)).T[0]

    # Check the parentheses. Otherwise it may end up on an infinite loop.
    if not check_parentheses(expr):
        sendErrorMsg('Parentheses mismatch in expression "{}"'.format(expr))
    
    plst = []
    for f in memb:
        p = pexpr.rfind(f)
        while p != -1:
            if not pexpr[p - 1].isalnum():
                # There is a space before the name
                ip = pexpr.find('(', p)
                if pexpr[p + len(f):ip].strip() == '':
                    # No alphanumeric characters between the name
                    # and the parenthesis
                    plst.append((p, f))
                        
            p = pexpr.rfind(f, 0, p)
    
    for p, f in plst:
        r = pexpr[p:].replace(f, mname + f, 1)
        pexpr = pexpr[:p] + r
        
    return pexpr


def numpy_parse_eval_expr( expr, mathmod = None ):
    '''
    Parse the given expression on a numpy-friendly way, so numpy functions
    are applied correctly when one evaluates it
    '''
    mathmod = mathmod or numpy
    
    expr = parse_eval_expr(expr, mathmod)
    
    comp = {}

    pexpr = expr
    while len(pexpr):

        op = pexpr.find('(') + 1
        if op == 0:
            break
        else:
            it = ParIter(1, 0)
            for cp, s in enumerate(pexpr[op:]):
                if it.signal(s):
                    app = pexpr[op:op + cp]
                    comp[app] = numpy_parse_eval_expr(app)
                    pexpr = pexpr[op + cp + 1:]
                    break
    
    for name, el in sorted(comp.iteritems()):
        expr = expr.replace(name, el)
        if el != name:
            expr = expr.replace(name, el)
    
    # The numpy functions need a list as an argument
    poslst = []
    pos = expr.find('numpy.')
    while pos != -1:

        npos = pos + 6
        
        p = expr.find('(', npos)
        f = expr[npos:p]
        
        func = getattr(numpy, f)
        
        if inspect.isfunction(func):
            # Only append if it is a function (not a "ufunc")
            poslst.append(pos)
            
        pos = expr.find('numpy.', npos)
        
    for p in reversed(poslst):
        par = expr.find('(', p)
        it  = ParIter(1, 0)
        for i, s in enumerate(expr[par + 1:]):
            if it.signal(s):
                break
        i += par + 1
        expr = '{}[{}], axis = 0{}'.format( expr[:par + 1],
                                            expr[par + 1:i],
                                            expr[i:] )

    return numpy_process_conditionals(expr)


def numpy_process_conditionals( expr ):
    '''
    Process one expression within this class
    '''
    pos = [ (expr.find(' and ') , ' and '),
            (expr.find(' or ')  , ' or '),
            (expr.find('^')     , '^') ]

    expr = expr.strip()
    
    # Not is an special case, since it can be preceded by
    # another conditional
    not_pos = [m.start() for m in re.finditer('not ', expr)]
    pos.append((not_pos[-1] if not_pos else -1, 'not '))
    
    for p, c in sorted(pos):
        if p != -1:
            
            left_expr  = expr[:p]
            right_expr = expr[p + len(c):]
            proc_expr  = numpy_process_conditionals(right_expr)
            
            if c == 'not ':
                return '{}{}({})'.format( left_expr,
                                          numpy_conditionals[c],
                                          proc_expr )
            else:
                return '{}({}, {})'.format( numpy_conditionals[c],
                                            left_expr,
                                            proc_expr )
    return expr


def translate_cexpr( expr ):
    '''
    Translates a C expression into a python expression
    '''
    expr = expr.replace('&&' , ' and ')
    expr = expr.replace('||' , ' or ')
    expr = expr.replace('abs', 'fabs')
    expr = expr.replace('!=', '%%%')
    expr = expr.replace('!', ' not ')
    expr = expr.replace('%%%', '!=')
    return expr
