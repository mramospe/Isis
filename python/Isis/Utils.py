#/////////////////////////////////////////////////////////////
#//                                                         //
#//  Python modules                                         //
#//                                                         //
#// ------------------------------------------------------- //
#//                                                         //
#//  AUTHOR: Miguel Ramos Pernas                            //
#//  e-mail: miguel.ramos.pernas@cern.ch                    //
#//                                                         //
#//  Last update: 14/07/2016                                //
#//                                                         //
#// ------------------------------------------------------- //
#//                                                         //
#//  Description:                                           //
#//                                                         //
#//  Defines some utils to manage python objects.           //
#//                                                         //
#// ------------------------------------------------------- //
#/////////////////////////////////////////////////////////////


import os, fcntl, math, struct, termios
import __builtin__
from Isis.Algebra import Matrix, SolveLU


#_______________________________________________________________________________
# Calculates the minimum distance between values in an iterable object. With the
# input parameter < allow_zero > one can prevent the function to take into
# account elements in the list with the same value.
def CalcMinDist( lst, allow_zero = True ):
    if allow_zero:
        lst = sorted( lst )
    else:
        lst = sorted( set( lst ) )
    lst = [ abs( lst[ i - 1 ] - lst[ i ] ) for i in xrange( 1, len( lst ) ) ]
    return min( lst )

#_______________________________________________________________________________
# Returns the minimum and maximum values for the combined range for all the
# given lists
def CombinedRange( *args ):
    minlst = min( min( lst ) for lst in args )
    maxlst = max( max( lst ) for lst in args )
    return minlst, maxlst

#_______________________________________________________________________________
# Given a dictionary of lists, it returns a list of dictionaries
def DLtoLD( dic ):
    firstkw = dic.keys()[ 0 ]
    length  = len( dic[ firstkw ] )
    return [ { kw: vals[ i ] for kw, vals in dic.iteritems() }
             for i in xrange( length ) ]

#_______________________________________________________________________________
# Displays the given time in the format [ w, d, h, min, s ]. If one of the
# quantities is zero, it is not displayed.
def FormatTime( itime ):
    conv  = [ 60, 60, 24, 7 ]
    vlist = [ 's', 'min', 'h', 'd', 'w' ]
    vals  = { 's': itime, 'min': 0, 'h': 0, 'd': 0, 'w': 0 }
    for cval, nunit, ounit in zip( conv, vlist[ 1: ], vlist[ :-1 ] ):
        vals[ nunit ], vals[ ounit ] = divmod( int( vals[ ounit ] ), cval )
    vlist.reverse()
    strout = ''
    for kw in vlist:
        val = vals[ kw ]
        if val:
            strout += str( vals[ kw ] ) + kw + ' '
    if strout:
        return strout[ :-1 ]
    else:
        return '0s'

#_______________________________________________________________________________
# This function allows to format a given expression in such a way that takes
# into account the mathematical functions and the logical operators. The module
# containing the mathematical functions can be specified.
def FormatEvalExpr( expr, mathmod = math ):
    expr      = expr.replace( '&&' , ' and ' )
    expr      = expr.replace( '||' , ' or '  )
    expr      = expr.replace( 'abs', 'fabs'  )
    variables = expr
    variables = variables.replace( ' ', '' )
    for el in ( '==', '!=', '<=', '>=', '>', '<',
                'and', 'or', '(', ')',
                '*', '/', '+', '-',
                '!', ',' ):
        variables = variables.replace( el, '|' )
    
    ''' These lines are needed to replace the C negation operator < ! > by < not >'''
    expr = expr.replace( '!=', '%%%' )
    expr = expr.replace( '!', ' not ' )
    expr = expr.replace( '%%%', '!=' )
    
    ''' Splits the elements, so only the variables and the numbers remain '''
    variables = variables.split( '|' )
    while '' in variables:
        variables.remove( '' )

    ''' These lines allow the management of float values given with an < e/E > '''
    for idx, el in enumerate( variables ):
        epos = el.find( 'e' )
        Epos = el.find( 'E' )
        dec1 = ( epos != -1 )
        dec2 = ( Epos != -1 )
        if ( dec1 or dec2 ) and not ( dec1 and dec2 ):
            if dec1:
                numvar = el[ :epos ]
            else:
                numvar = el[ :Epos ]
            try:
                float( numvar )
                variables[ idx ] += '0'
            except:
                pass
    
    truevars = []
    fmlist   = dir( mathmod )
    fblist   = dir( __builtin__ )
    mathmod  = mathmod.__name__ + '.'
    
    ''' This must be done to avoid replacing multiple times the same function by
    < module.function >'''
    idx    = 0
    length = len( variables )
    while idx < length:
        el = variables[ idx ]
        try:
            float( el )
            idx += 1
        except:
            if el in fmlist:
                nc = 0
                while el in variables:
                    variables.remove( el )
                    nc += 1
                length -= nc
                expr = expr.replace( el, mathmod + el )
            else:
                idx += 1
                if el not in fblist:
                    truevars.append( el )
    
    ''' Sorts the list on a reversed way is necessary to prevent missreplacement of
    the variables '''
    truevars.sort()
    truevars.reverse()
    return expr, truevars

#_______________________________________________________________________________
# Given two lists < x > and < y >, performs the inter(extra)polation of the
# < y > value in the given point < x0 > using a polynomial of order < nord >.
# If no order is providen it will use all the points to perform the calculation.
def InferValue( x, y, x0, nord = False ):

    srtlst = sorted( zip( x, y ) )
    x = [ el[ 0 ] for el in srtlst ]
    y = [ el[ 1 ] for el in srtlst ]

    ''' If no order is specified, it will be taken as the number of points. If it is greater
    than the number of points it is set to this number, and a warning message is sent. '''
    lp = len( x )
    if nord:
        if nord < lp:
            nord += 1
        else:
            nord = lp
            print 'WARNING: Order greater than the number of points. Value set to', lp
    else:
        nord = lp

    ''' Gets the index whose element is the first greater than the given point '''
    for i, xi in enumerate( x ):
        if xi >= x0: break

    ''' Stablishes the points to use for the interpolation '''
    if   i + nord/2 >= lp:
        imax = lp; imin = imax - nord
    elif i - nord/2 <= 0:
        imin = 0; imax = nord
    else:
        imin = i - nord/2; imax = imin + nord

    xsol = Matrix( [ x[ imin : imax ] ] )
    ysol = Matrix( [ y[ imin : imax ] ] )

    ''' Because of the degrees of freedom, the polinomial order is one unit smaller than the
    number of points '''
    nord -= 1

    ''' Generates the matrix of equations '''
    X = Matrix( [ [ el**n for n in xrange( nord, -1, -1 ) ] for el in xsol[ 0 ] ] )

    ''' Solves the lineal equation problem '''
    a = reversed( SolveLU( X, ysol.Transpose() ).Transpose()[ 0 ] )

    ''' Evaluates the polinomial at the point given '''
    y0 = 0.
    for n, el in enumerate( a ):
        y0 += el * x0**n
    return y0

#_______________________________________________________________________________
# Joins dictionaries with different keys into one. If some of them have the
# same key, this one is not considered and not added to the final dictionary.
def JoinDicts( *args ):
    rdict = {}
    for dic in args:
        for key in dic:
            if key in rdict:
                del rdict[ key ]
                print 'WARNING: Key <', key, '> already in dictionary. Not considered.'
            else:
                rdict[ key ] = dic[ key ]
    return rdict

#_______________________________________________________________________________
# If the input is a list, it gets the length of the maximum string located
# inside it. If it is a dictionary, it gets the maximum value of the strings
# associated to the keys of it ( it has to be a dictionary of strings ).
def LargestString( lstdic ):
    maxlen = 0
    if isinstance( lstdic, list ):
        for el in lstdic:
            newlen = len( el )
            if newlen > maxlen: maxlen = newlen
    elif isinstance( lstdic, dict ):
        for kw in lstdic:
            newlen = len( lstdic[ kw ] )
            if newlen > maxlen: maxlen = newlen
    else:
        print 'The input parameter is not a list nor a dictionary, returned 0'
    return maxlen

#_______________________________________________________________________________
# Given a list of dictionaries, it returns a dictionary of lists
def LDtoDL( lst ):
    keys = lst[ 0 ].keys()
    return { kw: [ el[ kw ] for el in lst ] for kw in keys }

#_______________________________________________________________________________
# This function generates a sublist from that given, with its values between
# < vmin > and < vmax >. If < nbr > is set to True, no value equal or greater
# than that of < vmax > will be considered. If set to False, the values that are
# equal to it will also be considered. By default the output list is given
# sorted, and this behaviour is managed trough the < sort > variable.
def MakeSubList( lst, vmin = None, vmax = None, **kwargs ):
    if 'sort' in kwargs: sort = kwargs[ 'sort' ]
    else: sort = True
    if 'nbr' in kwargs: nbr = kwargs[ 'nbr' ]
    else: nbr = True
    order = zip( lst, xrange( len( lst ) ) )
    order.sort()
    cplst = [ fst for fst, sec in order ]
    order = [ sec for fst, sec in order ]
    lgth = len( cplst )
    imin, imax = 0, lgth
    if vmin != None:
        i = 0
        while i < lgth and cplst[ i ] < vmin:
            i += 1
        imin = i
    if vmax != None:
        i = -1
        while i < lgth and cplst[ i ] > vmax:
            i -= 1
        if nbr and cplst[ i ] == vmax:
            i -= 1
        if i != -1:
            imax = i + 1
    if sort:
        return cplst[ imin:imax ]
    else:
        order = order[ imin:imax ]
        order.sort()
        return cplst.__class__( lst[ el ] for el in order )

#_______________________________________________________________________________
# This function merges the values in various dictionaries into one
def MergeDicts( *args ):
    kvars = args[ 0 ].keys()
    for dic in args[ 1: ]:
        keys = dic.keys()
        for key in kvars:
            if key not in keys:
                kvars.remove( key )
    rdic = {}
    for key in kvars:
        rdic[ key ] = []
        for dic in args:
            rdic[ key ] += dic[ key ]
    return rdic

#_______________________________________________________________________________
# Returns the minimum and maximum values for the shared range among lists. If
# there is no shared range < False > is returned.
def SharedRange( *args ):
    minlst = max( min( lst ) for lst in args )
    maxlst = min( max( lst ) for lst in args )
    if maxlst > minlst:
        return minlst, maxlst
    else:
        return False

#_______________________________________________________________________________
# Given a list and a pattern ( with elements separated by '*' symbols ) it
# returns another list with those that satisfy it.
def StringListFilter( lst, pattern ):
    pattern = pattern.split( '*' )
    checkstart = not pattern[  0 ] == ''
    checkend   = not pattern[ -1 ] == ''
    while '' in pattern:
        del pattern[ pattern.index( '' ) ]
    output = []
    for el in lst:
        add = True
        if checkstart:
            if not el.startswith( pattern[ 0 ] ):
                add = False
        if checkend:
            if not el.endswith( pattern[ -1 ] ):
                add = False
        if add:
            pos = 0
            for pt in pattern:
                newpos = el.find( pt )
                if newpos == -1 or newpos < pos:
                    add = False
                    break
                pos = newpos
            if add:
                output.append( el )
    return output

#_______________________________________________________________________________
# This is a generator of numbers represented as strings. It allows to perform
# iterations over numbers from < start > to < end >, filling the left side of
# them with as much zeros as necessary to make all the numbers in the row have
# the same size.
class StrNumGenerator:

    def __init__( self, start, end = False ):
        ''' If only one number is specified the start will be taken as zero. Only positive
        values are allowed. '''

        if not end:
            start, end = 0, start
        elif end < start:
            print 'ERROR: The starting number has to be greater than the ending'
        if end < 0 or start < 0:
            print 'ERROR: Input parameters have to be both positive'
        self.CurrIter  = start
        self.MaxIter   = end
        self.MaxStrLen = len( str( end ) )

    def __iter__( self ):
        ''' On the iterations it returns itself '''
        return self

    def next( self ):
        ''' Moves one position forward the iterator '''
        if self.CurrIter == self.MaxIter:
            raise StopIteration
        else:
            citer  = str( self.CurrIter )
            lciter = len( citer ) + 1
            self.CurrIter += 1
            return ( self.MaxStrLen - lciter )*'0' + citer

#_______________________________________________________________________________
# Returns the values of the height and width of the terminal
def TerminalSize():
    def gwinsz( fd ):
        try:
            cr = struct.unpack( 'hh', fcntl.ioctl( fd, termios.TIOCGWINSZ, '1234' ) )
        except:
            return
        return cr
    cr = gwinsz( 0 ) or gwinsz( 1 ) or gwinsz( 2 )
    if not cr:
        try:
            fd = os.open( os.ctermid(), os.O_RDONLY )
            cr = gwinsz( fd )
            os.close( fd )
        except:
            pass
    if not cr:
        cr = ( os.environ.get( 'LINES', 25 ), os.environ.get( 'COLUMNS', 80 ) )
    return int( cr[ 0 ] ), int( cr[ 1 ] )
