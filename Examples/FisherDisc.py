#////////////////////////////////////////////////////
#//  Example of a Fisher discriminant calculation  //
#////////////////////////////////////////////////////

from Isis.DataManagement import *
from Isis.Statistics import *

''' Loads the txt data '''
sigMngr = DataManager( "files/FisherSig.dat", [ "V1", "V2", "V3", "V4" ], fileType = "txt", colIndex = range( 1, 5 ) )
bkgMngr = DataManager( "files/FisherBkg.dat", [ "V1", "V2", "V3", "V4" ], fileType = "txt", colIndex = range( 1, 5 ) )

''' Gets the matrix with all the variables booked in the managers '''
sigMatrix = sigMngr.GetMatrix( '*' )
bkgMatrix = bkgMngr.GetMatrix( '*' )

''' Calculates the projection vectors '''
fisherDisc = FisherDiscriminant( sigMatrix, bkgMatrix )

''' Creates the plots '''
plots = fisherDisc.PlotFisherQuality( nsig = 100, nbkg = 1000, nbins = 100, npoints = 1000 )

''' Applies the fisher to the same sample '''
sigVar = fisherDisc( sigMatrix )
bkgVar = fisherDisc( bkgMatrix )
