#////////////////////////////////////////////////////////////////
#//                                                            //
#//  Python modules                                            //
#//                                                            //
#// ---------------------------------------------------------- //
#//                                                            //
#//  AUTHOR: Miguel Ramos Pernas                               //
#//  e-mail: miguel.ramos.pernas@cern.ch                       //
#//                                                            //
#//  Last update: 11/12/2015                                   //
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
# Calculates the greater common divisor of a number
def GreaterComDiv( number ):
    cnt = number - 1
    while number % cnt:
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
# Calculates the least common multiple of a set of numbers. If no number is
# found it returns one.
def LeastComMult( *args ):
    cnt = 2
    while any( el % cnt for el in args ) and all( cnt <= el for el in args ):
        cnt += 1
    if any( el < cnt for el in args ):
        return 1
    else:
        return cnt
