#/////////////////////////////////////////////////////////////
#//                                                         //
#//  Python modules                                         //
#//                                                         //
#// ------------------------------------------------------- //
#//                                                         //
#//  AUTHOR: Miguel Ramos Pernas                            //
#//  e-mail: miguel.ramos.pernas@cern.ch                    //
#//                                                         //
#//  Last update: 08/02/2016                                //
#//                                                         //
#// ------------------------------------------------------- //
#//                                                         //
#//  Description:                                           //
#//                                                         //
#//  Defines some utils to manage python objects.           //
#//                                                         //
#// ------------------------------------------------------- //
#/////////////////////////////////////////////////////////////


import math
from Isis.Algebra import Matrix, SolveLU


#_______________________________________________________________________________
# Displays the given time in the format [ w, d, h, min, s ]. If one of the
# quantities is zero, it is not displayed.
def FormatTime( itime ):
    conv  = [ 60, 60, 24, 7 ]
    vlist = [ "s", "min", "h", "d", "w" ]
    vals  = { "s": itime, "min": 0, "h": 0, "d": 0, "w": 0 }
    for cval, nunit, ounit in zip( conv, vlist[ 1: ], vlist[ :-1 ] ):
        vals[ nunit ], vals[ ounit ] = divmod( int( vals[ ounit ] ), cval )
    vlist.reverse()
    strout = ""
    for kw in vlist:
        val = vals[ kw ]
        if val:
            strout += str( vals[ kw ] ) + kw + " "
    if strout:
        return strout[ :-1 ]
    else:
        return "0s"

#_______________________________________________________________________________
# This function allows to format a given expression in such a way that takes
# into account the mathematical functions and the logical operators. The module
# containing the mathematical functions can be specified.
def FormatEvalExpr( expr, mathmod = math ):
    expr      = expr.replace( "&&" , "and"  )
    expr      = expr.replace( "||" , "or"   )
    expr      = expr.replace( "abs", "fabs" )
    variables = expr
    variables = variables.replace( ' ', '' )
    for el in ( '==', '!=', '<=', '>=', '>', '<',
                'and', 'or', '(', ')',
                "*", "/" ):
        variables = variables.replace( el, '|' )
    ''' This lines allow the management of float values given with an < e/E > '''
    if variables[ 0 ] in ( '+', '-' ):
        variables = '|' + variables[ 1: ]
    i, n = 1, len( variables )
    while ( i != n ):
        el = variables[ i ]
        if el in ( "+", "-" ):
            el = variables[ i - 1 ]
            if el != "e" and el != "E":
                variables = variables[ :i ] + '|' + variables[ i + 1: ]
            else:
                i += 1
        else:
            i += 1
    ''' Splits the elements, so only the variables and the numbers remain '''
    variables = variables.split( '|' )
    while '' in variables:
        variables.remove( '' )
    truevars = []
    flist    = dir( mathmod )
    mathmod  = mathmod.__name__ + "."
    for el in variables:
        try:
            float( el )
        except:
            if el in flist:
                expr = expr.replace( el, mathmod + el )
            else:
                truevars.append( el )
    ''' Sorting the list on a reversed way is necessary to prevent missreplacement of
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
            print "WARNING: Order greater than the number of points. Value set to", lp
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
                print "Key <", key, "> already in dictionary. Not considered."
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
        print "The input parameter is not a list nor a dictionary, returned 0"
    return maxlen

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
# Given a list and a pattern ( with elements separated by "*" symbols ) it
# returns another list with those that satisfy it.
def StringListFilter( lst, pattern ):
    pattern = pattern.split( "*" )
    checkstart, checkend = False, False
    while "" in pattern:
        pos = pattern.index( "" )
        if pos:
            del pattern[ pos ]
            checkstart = True
        else:
            del pattern[ pos ]
            checkend   = True
    if checkstart and checkend:
        checkstart, checkend = False, False
    output = []
    for el in lst:
        add = True
        if checkstart:
            if not el.startswith( pattern[ 0 ] ):
                add = False
        elif checkend:
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
            print "ERROR: The starting number has to be greater than the ending"
        if end < 0 or start < 0:
            print "ERROR: Input parameters have to be both positive"
        end   -= 1
        order  = len( str( end ) )
        num, vlist = start, []
        while num > 1:
            num, rem = divmod( num, 10 )
            vlist.append( rem )
        while len( vlist ) < order:
            vlist.append( 0 )
        vlist[ 0 ] -= 1
        self.CurrIter = 0
        self.List     = vlist
        self.MaxIter  = end - start

    def __iter__( self ):
        ''' On the iterations it returns itself '''
        return self

    def next( self ):
        ''' Moves one position forward the iterator '''
        if self.CurrIter > self.MaxIter:
            raise StopIteration
        else:
            self.CurrIter  += 1
            self.List[ 0 ] += 1
            i = 0
            while self.List[ i ] == 10:
                self.List[ i ] = 0
                i += 1
                self.List[ i ] += 1
            return "".join( [ str( el ) for el in reversed( self.List ) ] )
