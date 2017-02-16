#//////////////////////////////////////////
#//  Example/test of the Algebra module  //
#//////////////////////////////////////////

from Isis.Algebra import *

print "----------------------------------------------"
print "This is an example to test some of the Algebra"
print "module features"

print "-------------------"
print "Defines two vectors"
v1 = LongVector( [ 1, 2, 3  ] )
v2 = LongVector( [ 10, 5, 6 ] )
print "v1:", v1
print "v2:", v2
print "v1/v2:", v1/v2
print "v1*v2:", v1*v2
print "v1.Dot( v2 ):", v1.Dot( v2 )

print "----------------"
print "Defines a matrix"
M = Matrix( [
        [ 11,  9, 24, 2 ],
        [  1,  5,  2, 6 ],
        [  3, 17, 18, 1 ],
        [  2,  5,  7, 1 ]
        ])
print "M:", M

print "Calculates the L, U, P and Q matrices"
L, U, P, Q = LU( M )
print "L:", L
print "U:", U
print "P:", P
print "Q:", Q
print "P.Dot( L.Dot( U.Dot( Q ) ) ):", P.Dot( L.Dot( U.Dot( Q ) ) )

print "--------------------------------"
print "Solves a linear equation problem"
M = Matrix( [ 
        [ 1, 3, -1 ],
        [ 2, 4, 1 ],
        [ -2, 1, 0 ]
        ])
print "M:", M
b = Matrix( [ [ 8, 8, -5 ] ] )
print "b:", b
print "Equation: M*x = b"
print "The solution should be: [3, 1, -2]"
print "Solution:", SolveLU( M, b.Transpose() )

print "--------------------------------------------"
print "Now we are going to calculate the inverse"
print "and the determinant of a matrix"
M = Matrix( [
        [ 0, 0, 1 ],
        [ 1, 1, 0 ],
        [ 1, 0, 0 ]
        ])
Mpiv = PivoteMatrices( M )

print "Initial matrix:", M
print "Inverse matrix:", Inv( M )
print "Product of both:", M.Dot( Inv( M ) )
print "The determinant should be -1"
print "Determinant:", Det( M )

print "---------------"
print "Another example"
M = Matrix( [
        [ 1, 2, 3 ],
        [ 4, 5, 6 ],
        [ 7, 8, 9 ]
        ] )
print "Initial matrix:", M
print "The determinant should be 0"
print "Determinant:", Det( M )
try:
    print "Inverse matrix:", Inv( M )
except:
    print "The inverse can not be computed since determinant is zero"

