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
# Calculates the least common multiple of two numbers. If no number is found it
# returns one.
def LeastComMult( na, nb ):
    cnt = 2
    while ( na % cnt or nb % cnt ) and cnt <= na and cnt <= nb:
        cnt += 1
    if cnt > na or cnt > nb:
        return 1
    else:
        return cnt

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
