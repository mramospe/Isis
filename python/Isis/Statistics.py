#//////////////////////////////////////////////////////////
#//                                                      //
#//  Python modules                                      //
#//                                                      //
#// ---------------------------------------------------- //
#//                                                      //
#//  AUTHOR: Miguel Ramos Pernas                         //
#//  e-mail: miguel.ramos.pernas@cern.ch                 //
#//                                                      //
#//  Last update: 27/10/2015                             //
#//                                                      //
#// ---------------------------------------------------- //
#//                                                      //
#//  Description:                                        //
#//                                                      //
#//  Defines different statistical functions to manage   //
#//  data stored in lists.                               //
#//                                                      //
#// ---------------------------------------------------- //
#//////////////////////////////////////////////////////////


from math import sqrt


#_______________________________________________________________________________
# Calculates the covariance between two lists ( with the same size )
def Covariance( lst1, lst2 ):
    m1 = Mean( lst1 )
    m2 = Mean( lst2 )
    ex = [ ( v1 - m1 )*( v2 - m2 ) for v1, v2 in zip( lst1, lst2 ) ]
    return Mean( ex )

#_______________________________________________________________________________
# Calculates the covariance matrix for a given set of data. The indexes of the
# data set have to correspond to the values of one of the variables.
def CovMatrix( data ):
    length    = len( lst1 )
    covmatrix = length*[ length*[ 0. ] ]
    for i in range( length ):
        for j in range( length ):
            covmatrix[ i ][ j ] = Covariance( data[ i ], data[ j ] )
    return covmatrix

#_______________________________________________________________________________
# Calculates the linear correlation coefficient between two lists of values
def LinearCorrCoef( lst1, lst2 ):
    return Covariance( lst1, lst2 )*1./( StdDev( lst1 )*StdDev( lst2 ) )

#_______________________________________________________________________________
# Calculates the mean of the values in a list of values
def Mean( lst ):
    return sum( lst )*1./len( lst )

#_______________________________________________________________________________
# Calculates the squared standard deviation of the mean of a list of values
def MeanStdDev2( lst ):
    return StdDev2( lst )*1./len( lst )

#_______________________________________________________________________________
# Calculates the standard deviation of the mean of a list of values
def MeanStdDev( lst ):
    return sqrt( MeanStdDev2( lst ) )

#_______________________________________________________________________________
# Calculates the mode of a list of values
def Mode( lst ):
    vdict = {}
    for el in lst:
        if el not in vdict:
            vdict[ el ] = 1
        else:
            vdict[ el ] += 1
    oldmax = 0
    for el in vdict:
        newmax = vdict[ el ]
        if newmax > oldmax:
            oldmax = newmax
    nmodes = 0
    for el in vdict:
        if vdict[ el ] == oldmax:
            nmodes += 1
    if nmodes > 1:
        print "WARNING: A number of", nmodes, "exist in the input list"
    return oldmax

#_______________________________________________________________________________
# Gets the nth momentum of the distribution of the list
def StatMomentum( lst, n ):
    mean = Mean( lst )
    res  = 0
    for el in lst:
        res += ( el - mean )**n
    return res*1./len( lst )
    
#_______________________________________________________________________________
# Calculates the squared standard deviation of a list of values
def StdDev2( lst ):
    mean = Mean( lst )
    res  = 0
    for el in lst:
        res += ( el - mean )**2
    return res*1./len( lst )

#_______________________________________________________________________________
# Calculates the standard deviation of a list of values
def StdDev( lst ):
    return sqrt( StdDev2( lst ) )
