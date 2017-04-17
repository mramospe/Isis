#////////////////////////////////////////////////////////////////
#//
#//  Python modules
#//
#// -------------------------------------------------------------
#//
#//  AUTHOR: Miguel Ramos Pernas
#//  e-mail: miguel.ramos.pernas@cern.ch
#//
#//  Last update: 17/04/2017
#//
#// -------------------------------------------------------------
#//
#//  Description:
#//
#//  This module implements different mathematical functions.
#//
#// -------------------------------------------------------------
#////////////////////////////////////////////////////////////////


from math import sqrt, floor


def decompose( number ):
    '''
    Decomposes the given number in the smallest multiples. The result is given in
    a list, which is automatically ordered from the minimum multiple to the
    maximum.
    '''
    cnt, result = 1, []
    while number != 1:
        cnt += 1
        if number % cnt == 0:
            number /= cnt
            result.append( cnt )
            cnt = 1
    return result


def greaterComDiv( *args ):
    '''
    Calculates the greater common divisor of a set of numbers
    '''
    cnt = min( args )
    while any( el % cnt for el in args ):
        cnt -= 1
    return cnt


def isPrime( number ):
    '''
    Checks if the given number is a prime number
    '''
    cnt = 2
    while number % cnt and cnt < number:
        cnt += 1
    if cnt == number:
        return True
    else:
        return False


def isSquare( number ):
    '''
    Checks if the given number is a pure square number
    '''
    cnt, number = 0, int( number )
    while cnt*cnt < number:
        cnt += 1
    if cnt*cnt == number:
        return True
    else:
        return False


def leastComMult( *args ):
    '''
    Calculates the least common multiple of a set of numbers
    '''
    cnt = max( args )
    while any( cnt % el for el in args ):
        cnt += 1
    return cnt


def nearestSquare( number ):
    '''
    Calculates the nearest perfect square number to the one given
    '''
    nfloor  = floor( sqrt( number ) )
    lowsqrt = nfloor**2
    upsqrt  = ( nfloor + 1 )**2
    if number - lowsqrt < upsqrt - number:
        return int(round(lowsqrt))
    else:
        return int(round(upsqrt))


def sumQuad( *values ):
    '''
    Sum the input values in quadrature
    '''
    return sum(v**2 for v in values)
