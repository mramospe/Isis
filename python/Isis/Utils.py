#/////////////////////////////////////////////////////////////
#//                                                         //
#//  Python modules                                         //
#//                                                         //
#// ------------------------------------------------------- //
#//                                                         //
#//  AUTHOR: Miguel Ramos Pernas                            //
#//  e-mail: miguel.ramos.pernas@cern.ch                    //
#//                                                         //
#//  Last update: 13/01/2016                                //
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
# into account the mathematical functions and the logical operators. It will
# also check that the variables obtained are contained in the list given.
def FormatEvalExpr( expr, vlist ):
    expr      = expr.replace( "&&" , "and"  )
    expr      = expr.replace( "||" , "or"   )
    expr      = expr.replace( "abs", "fabs" )
    variables = expr
    for el in [ '==', '!=', '<=', '>=', '>', '<',
                'and', 'or', '(', ')',
                "+", "-", "*", "/" ]:
        variables = variables.replace( el, '|' )
    variables = variables.replace( ' ', '' )
    variables = variables.split( '|' )
    while '' in variables:
        variables.remove( '' )
    truevars = []
    flist    = dir( math )
    for el in variables:
        if el not in flist:
            try:
                float( el )
            except:
                if el in vlist:
                    truevars.append( el )
                else:
                    print "ERROR: Variable <", el, "> does not exist"
        else:
            expr = expr.replace( el, "math." + el )
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
