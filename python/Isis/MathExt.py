#////////////////////////////////////////////////////////////////
#//                                                            //
#//  Python modules                                            //
#//                                                            //
#// ---------------------------------------------------------- //
#//                                                            //
#//  AUTHOR: Miguel Ramos Pernas                               //
#//  e-mail: miguel.ramos.pernas@cern.ch                       //
#//                                                            //
#//  Last update: 13/12/2015                                   //
#//                                                            //
#// ---------------------------------------------------------- //
#//                                                            //
#//  Description:                                              //
#//                                                            //
#//  This module implements different mathematical functions.  //
#//                                                            //
#// ---------------------------------------------------------- //
#////////////////////////////////////////////////////////////////


from math import sqrt


#_______________________________________________________________________________
# Decomposes the given number in the smallest multiples. The result is given in
# a list, which is automatically ordered from the minimum multiple to the
# maximum.
def Decompose( number ):
    cnt, result = 1, []
    while number != 1:
        cnt += 1
        if number % cnt == 0:
            number /= cnt
            result.append( cnt )
            cnt = 1
    return result

#_______________________________________________________________________________
# Calculates the greater common divisor of a set of numbers
def GreaterComDiv( *args ):
    cnt = min( args )
    while any( el % cnt for el in args ):
        cnt -= 1
    return cnt

#_______________________________________________________________________________
# Checks if the given number is a prime number
def IsPrime( number ):
    cnt = 2
    while number % cnt and cnt < number:
        cnt += 1
    if cnt == number:
        return True
    else:
        return False

#_______________________________________________________________________________
# Checks if the given number is a pure square number
def IsSquare( number ):
    cnt, number = 0, int( number )
    while cnt*cnt < number:
        cnt += 1
    if cnt*cnt == number:
        return True
    else:
        return False

#_______________________________________________________________________________
# Calculates the least common multiple of a set of numbers
def LeastComMult( *args ):
    cnt = max( args )
    while any( cnt % el for el in args ):
        cnt += 1
    return cnt
