#/////////////////////////////////////////////////////////////
#//
#//  Python modules
#//
#// ----------------------------------------------------------
#//
#//  AUTHOR: Miguel Ramos Pernas
#//  e-mail: miguel.ramos.pernas@cern.ch
#//
#//  Last update: 07/12/2015
#//
#// ----------------------------------------------------------
#//
#//  Description:
#//
#//  Defines classes and functions to perform algebraic
#//  calculations.
#//
#// ----------------------------------------------------------
#/////////////////////////////////////////////////////////////


#_______________________________________________________________________________
# Definition of the class to almacenate a large number of algebraic values. It
# inherits from the < list > python class, with the algebraic methods being
# modified.
class LongVector( list ):
    
    def __init__( self, obj = [] ):
        ''' To construct this class an iterable object has to be provided '''
        if obj:
            for el in obj:
                self.append( el )

    def __add__( self, obj ):
        ''' Computes the sum of two objects. If a LongVector class is provided,
        the sum element by element will be computed '''
        if isinstance( obj, LongVector ):
            return LongVector( [ el1 + el2 for el1, el2 in zip( self, obj ) ] )
        else:
            return LongVector( [ el + obj for el in self ] )

    def __div__( self, obj ):
        ''' Computes the division between this class and an object. If a
        LongVector class is provided, it will compute it element by element '''
        if isinstance( obj, LongVector ):
            return LongVector( [ el1/float( el2 ) for el1, el2 in zip( self, obj ) ] )
        else:
            return LongVector( [ el/float( obj ) for el in self ] )

    def __iadd__( self, obj ):
        ''' Computes the __add__ method setting the result to itself '''
        if isinstance( obj, LongVector ):
            for i in xrange( self.Nrows() ):
                self[ i ] += obj[ i ]
        else:
            for i in xrange( self.Nrows() ):
                self[ i ] += obj

    def __isub__( self, obj ):
        ''' Computes the __sub__ method setting the result to itself '''
        if isinstance( obj, LongVector ):
            for i in xrange( self.Nrows() ):
                self[ i ] -= obj[ i ]
        else:
            for i in xrange( self.Nrows() ):
                self[ i ] -= obj

    def __mul__( self, obj ):
        ''' Computes the multiplication between two objects. If a LongVector
        class is provided, it will compute it element by element '''
        if isinstance( obj, LongVector ):
            return LongVector( [ el1*el2 for el1, el2 in zip( self, obj ) ] )
        else:
            return LongVector( [ el*obj for el in self ] )

    def __rdiv__( self, obj ):
        ''' See __div__ '''
        return self.__div__( obj )

    def __rmul__( self, obj ):
        ''' See __mul__ '''
        return self.__mul__( obj )

    def __sub__( self, obj ):
        ''' Computes the substraction of two objects. If a LongVector class is provided,
        the substraction element by element will be computed '''
        if isinstance( obj, LongVector ):
            return LongVector( [ el1 - el2 for el1, el2 in zip( self, obj ) ] )
        else:
            return LongVector( [ el - obj for el in self ] )

    def Dot( self, obj ):
        ''' Computes the dot product between two LongVector classes '''
        res = 0
        for el1, el2 in zip( self, obj ):
            res += el1*el2
        return res

    def Mod2( self ):
        ''' Computes the squared module of the LongVector '''
        res = 0
        for el in self:
            res += el*el
        return res

    def Mod( self ):
        ''' Computes the module of the LongVector '''
        return sqrt( self.Mod2() )

#_______________________________________________________________________________
# Defines the Matrix class, which inherits from LongVector. In order to solve
# algebraic equations all objects has to be of this class, since the Dot product
# defined here is different from that of LongVector class.
class Matrix( LongVector ):

    def __init__( self, obj = False, **kwargs ):
        ''' If another Matrix is provided, it creates a copy of it. On the other
        hand a set of LongVectors can be given, appending a copy of them to the
        class. By default the different vectors are considered as rows in the
        matrix. '''
        if   isinstance( obj, Matrix ):
            for el in obj:
                self.append( el )
        elif isinstance( obj, list ):
            for el in obj: 
                self.append( LongVector( el ) )

    def __add__( self, obj ):
        ''' Computes the sum of two objects. If a Matrix class is provided,
        the sum element by element will be computed '''
        if isinstance( obj, Matrix ):
            return Matrix( [ el1 + el2 for el1, el2 in zip( self, obj ) ] )
        else:
            return Matrix( [ el + obj for el in self ] )

    def __div__( self, obj ):
        ''' Computes the division between this class and an object. If a
        Matrix class is provided, it will compute it element by element '''
        if isinstance( obj, Matrix ):
            return Matrix( [ el1/float( el2 ) for el1, el2 in zip( self, obj ) ] )
        else:
            return Matrix( [ el/float( obj ) for el in self ] )

    def __iadd__( self, obj ):
        ''' Computes the __add__ method setting the result to itself '''
        if isinstance( obj, Matrix ):
            for i in xrange( self.Nrows() ):
                self[ i ] += obj[ i ]
        else:
            for i in xrange( self.Nrows() ):
                self[ i ] += obj

    def __isub__( self, obj ):
        ''' Computes the __sub__ method setting the result to itself '''
        if isinstance( obj, Matrix ):
            for i in xrange( self.Nrows() ):
                self[ i ] -= obj[ i ]
        else:
            for i in xrange( self.Nrows() ):
                self[ i ] -= obj

    def __mul__( self, obj ):
        ''' Computes the multiplication between two objects. If a Matrix
        class is provided, it will compute it element by element '''
        if isinstance( obj, Matrix ):
            return Matrix( [ el1*el2 for el1, el2 in zip( self, obj ) ] )
        else:
            return Matrix( [ el*obj for el in self ] )

    def __rdiv__( self, obj ):
        ''' See __div__ '''
        return self.__div__( obj )

    def __rmul__( self, obj ):
        ''' See __mul__ '''
        return self.__mul__( obj )

    def __sub__( self, obj ):
        ''' Computes the substraction of two objects. If a Matrix class is provided,
        the substraction element by element will be computed '''
        if isinstance( obj, Matrix ):
            return Matrix( [ el1 - el2 for el1, el2 in zip( self, obj ) ] )
        else:
            return Matrix( [ el - obj for el in self ] )

    def Diagonal( self ):
        ''' Returns a row Matrix with the elements in the diagonal of this one '''
        return Matrix( [ [ self[ i ][ i ] for i in xrange( len( self ) ) ] ] )

    def Dimensions( self ):
        ''' Returns the dimensions of the matrix '''
        return len( self ), len( self[ 0 ] )

    def Dot( self, obj ):
        ''' Performs the dot product between matrices. Note that this method can
        not be used to perform multiplications by a LongVector. In that case one
        has to convert it to a matrix class '''
        nrows  = self.Nrows()
        ncols  = len( obj[ 0 ] )
        matrix = Zeros( nrows, ncols )
        rarr   = range( nrows )
        carr   = range( ncols )
        karr   = range( len( obj ) )
        for ir in rarr:
            for ic in carr:
                for ik in karr:
                    matrix[ ir ][ ic ] += self[ ir ][ ik ]*obj[ ik ][ ic ]
        return matrix

    def GetCol( self, index ):
        ''' Returns a Matrix with the elements of the column at < index > '''
        return Matrix( [ [ vec[ index ] ] for vec in self ] )

    def GetColList( self, index ):
        ''' Returns a list with the elements of the column at the given index '''
        return [ vec[ index ] for vec in self ]

    def GetMaximumPos( self ):
        ''' Gets the position of the maximum value in the matrix '''
        rmax    = 0
        cmax    = 0
        maximum = max( self[ 0 ] )
        for irow in xrange( len( self[ 1: ] ) ):
            for icol in xrange( len( self[ 0 ] ) ):
                newmax = self[ irow ][ icol ]
                if newmax > maximum:
                    maximum = newmax
                    rmax    = irow
                    cmax    = icol
        return irow, icol

    def GetMinimumPos( self ):
        ''' Gets the position of the minimum value in the matrix '''
        rmin    = 0
        cmin    = 0
        minimum = min( self[ 0 ] )
        for irow in xrange( len( self[ 1: ] ) ):
            for icol in xrange( len( self[ 0 ] ) ):
                newmin = self[ irow ][ icol ]
                if newmin < minimum:
                    minimum = newmin
                    rmin    = irow
                    cmin    = icol
        return irow, icol

    def GetRow( self, index ):
        ''' Returns a copy of the Matrix at place < index > '''
        return Matrix( self[ index ] )

    def GetRowList( self, index ):
        ''' Returns a list with the elements of the row at the given index '''
        return list( self[ index ] )
    
    def Ncols( self ):
        ''' Returns the number of columns of the matrix '''
        return len( self[ 0 ] )

    def Nrows( self ):
        ''' Returns the number of rows of the matrix '''
        return len( self )

    def ReplaceRow( self, index, row ):
        ''' Replaces the row at the given index '''
        self[ index ] = row[ 0 ]

    def ReplaceCol( self, index, column ):
        ''' Replaces the column at the given index '''
        for ir, el in enumerate( self ):
            el[ index ] = column[ 0 ][ ir ]

    def SwapCols( self, index1, index2 ):
        ''' Swaps the content of two columns in the matrix given their index '''
        column1 = self.GetCol( index1 ).Transpose()
        column2 = self.GetCol( index2 ).Transpose()
        self.ReplaceCol( index1, column2 )
        self.ReplaceCol( index2, column1 )

    def SwapRows( self, index1, index2 ):
        ''' Swaps the content of two rows in the matrix given their index '''
        self[ index1 ], self[ index2 ] = self[ index2 ], self[ index1 ]

    def Trace( self ):
        ''' Calculates the trace of the matrix '''
        return sum( self[ i ][ i ] for i in range( len( self ) ) )

    def Transpose( self ):
        ''' Returns the transpose matrix to this one '''
        tmatrix = Matrix()
        for tup in zip( *self ):
            tmatrix.append( LongVector( tup ) )
        return tmatrix

#_______________________________________________________________________________
# Calculates the determinant of a matrix using the LU factorization. If the LU
# factorization fails means that the determinant is zero.
def Det( matrix ):
    dim = len( matrix )
    try:
        L, U, P, Q = LU( matrix )
    except:
        return 0.
    ldet, udet = 1, 1
    for i in xrange( dim ):
        ldet *= L[ i ][ i ]
        udet *= U[ i ][ i ]
    if P.Swaps + Q.Swaps % 2:
        return -ldet*udet
    else:
        return +ldet*udet

#_______________________________________________________________________________
# Performs the backward substitution given the upper triangular matrix and the
# vector of independent terms. < b > has to be given as a column Matrix.
def BackwardSubs( B, b ):
    dim = len( B )
    x   = Matrix( dim*[ [ 0. ] ] )
    for i in xrange( dim - 1, -1, -1 ):
        x[ i ][ 0 ] = ( b[ i ][ 0 ] -
                        sum( B[ i ][ k ]*x[ k ][ 0 ] for k in xrange( i, dim ) ) )/float( B[ i ][ i ] )
    return x
        
#_______________________________________________________________________________
# Solves by forward substitution given a lower triangular matrix and the vector
# of independent terms. < b > has to be given as a column Matrix.
def ForwardSubs( B, b ):
    dim = len( B )
    y   = Matrix( dim*[ [ 0. ] ] )
    for i in xrange( dim ):
        y[ i ][ 0 ] = ( b[ i ][ 0 ] -
                        sum( B[ i ][ j ]*y[ j ][ 0 ] for j in xrange( i ) ) )/float( B[ i ][ i ] )
    return y

#_______________________________________________________________________________
# Calculates the inverse of the matrix using the LU decomposition. The
# solution for the inverse of L and U is obtained by forward and backward
# substitution with the columns of the identity matrix.
def Inv( matrix ):
    L, U, P, Q = LU( matrix )
    dim  = len( matrix )
    I    = Identity( dim )
    Linv = Zeros( dim, dim )
    Uinv = Zeros( dim, dim )
    for i in xrange( dim ):
        icol      = I.GetCol( i )
        Linv[ i ] = ForwardSubs( L, icol ).Transpose()[ 0 ]
        Uinv[ i ] = BackwardSubs( U, icol ).Transpose()[ 0 ]
    Linv = Linv.Transpose()
    Uinv = Uinv.Transpose()
    ''' Since L and U are triangular, it is satisfied ( Uinv.Dot( Linv ) =
    Linv.Dot( Uinv ) '''
    return Q.Dot( Uinv.Dot( Linv.Dot( P ) ) )

#_______________________________________________________________________________
# Performs the LU decomposition, returning L, U and the two pivote matrices
def LU( matrix ):
    dim     = len( matrix )
    L       = Zeros( dim, dim )
    U       = Zeros( dim, dim )
    A, P, Q = PivoteMatrices( matrix )
    for j in xrange( dim ):
        L[ j ][ j ] = 1.
        for i in xrange( j + 1 ):
            s = sum( U[ k ][ j ]*L[ i ][ k ] for k in xrange( i ) )
            U[ i ][ j ] = A[ i ][ j ] - s
        for i in xrange( j, dim ):
            s = sum( U[ k ][ j ]*L[ i ][ k ] for k in xrange( j ) )
            L[ i ][ j ] = ( A[ i ][ j ] - s )/float( U[ j ][ j ] )
    return L, U, P, Q
    
#_______________________________________________________________________________
# Returns the pivoted matrix together with the row and column permutation
# matrices. The procedure consists on finding on each row-column combination,
# a number different from zero to place it in the diagonal. The only way to
# ensure that is will be possible to put elements different from zero in the
# diagonal is to see if in that case the submatrix generated at the end has at
# least one number different from zero in the row-column combination. Only n - 1
# iterations are needed since the last term is automatically ordered with the
# previous one.
def PivoteMatrices( matrix ):
    dim = len( matrix )
    P   = Identity( dim ); P.Swaps = 0
    Q   = Identity( dim ); Q.Swaps = 0
    A   = Matrix( matrix )
    for n in xrange( dim - 1 ):
        stit  = n + 1
        j, nf = n, True
        while nf and j < dim:
            dec = True
            ''' Here it checks if the elment at j is different from zero '''
            if abs( A[ j ][ n ] ) > 0.:
                r = n
                ''' Starts to loop over all the elements of the row at a position different
                from that of j. If all the elements are zero at that row, it breaks and looks
                if swapping columns is allowed. '''
                while dec and r < dim:
                    if r != j:
                        rlist = matrix.GetRowList( r )[ n: ]
                        if max( map( abs, rlist ) ) == 0.:
                            dec = False
                    r += 1
                if dec:
                    c = stit
                    while dec and c < dim:
                        clist = matrix.GetColList( c )[ n: ]
                        del clist[ j - n ]
                        if max( map( abs, clist ) ) == 0.:
                            dec = False
                        else:
                            c += 1
                    if dec:
                        nf = False
                        if j != n:
                            A.SwapRows( n, j )
                            P.SwapRows( n, j ); P.Swaps += 1
            ''' If a stable configuration is found checking the rows, the loop over the columns
            is not made '''
            if nf:
                ''' Here it checks if the element at column j is different from zero '''
                if abs( A[ n ][ j ] ) > 0.:
                    c = n
                    ''' Here makes the loop over the elements of the column at a position different
                    from that of j. If all elements are zero at that column, a new j value is taken '''
                    while dec and c < dim:
                        if c != j:
                            clist = matrix.GetColList( c )[ n: ]
                            if max( map( abs, clist ) ) == 0.:
                                dec = False
                        c += 1
                    if dec:
                        r = stit
                        while dec and r < dim:
                            rlist = matrix.GetRowList( r )[ n: ]
                            del rlist[ j - n ]
                            if max( map( abs, rlist ) ) == 0.:
                                dec = False
                            else:
                                r += 1
                        if dec:
                            nf = False
                            if j != n:
                                A.SwapCols( n, j )
                                Q.SwapCols( n, j ); Q.Swaps += 1
            j += 1
    ''' Returns the pivoted matrix ( A ), with the row ( P ) and column ( Q ) permutation matrices '''
    return A, P, Q

#_______________________________________________________________________________
# Solves a system of linear equations using the LU method
def SolveLU( matrix, idpt ):
    matrix     = Matrix( matrix )
    L, U, P, Q = LU( matrix )
    idpt = P.Dot( idpt )
    Y    = ForwardSubs( L, idpt )
    return Q.Dot( BackwardSubs( U, Y ) )

#_______________________________________________________________________________
# Generates an identity matrix with dimensions dim x dim
def Identity( dim ):
    return Matrix( [ [ float( i == j ) for i in xrange( dim ) ]
                      for j in xrange( dim ) ] )

#_______________________________________________________________________________
# Generates a matrix filled by zeros of dimensions nrows x ncols
def Zeros( nrows, ncols ):
    return Matrix( [ [ 0. for i in xrange( ncols ) ]
                     for j in xrange( nrows ) ] )
