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
#//  Defines different statistical functions and class   //
#//  to manage data stored in lists.                     //
#//                                                      //
#// ---------------------------------------------------- //
#//////////////////////////////////////////////////////////


from ROOT import TCanvas, TH1D, TGraph
import numpy
from math import sqrt


#_______________________________________________________________________________
# --- CLASSES ---

#_______________________________________________________________________________
# Fisher discriminant class
class FisherDiscriminant:

    #_______________________________________________________________________________
    # Constructor
    def __init__( self, sampleA, sampleB ):

        self.SampleA = numpy.matrix( sampleA )
        self.SampleB = numpy.matrix( sampleB )

        cmatrix1 = numpy.matrix( CovMatrix( self.SampleA.A ) )
        cmatrix2 = numpy.matrix( CovMatrix( self.SampleB.A ) )
        invscm   = numpy.linalg.inv( cmatrix1 + cmatrix2 )
        means1   = numpy.array( [ Mean( numpy.array( row ) ) for row in self.SampleA.A ] )
        means2   = numpy.array( [ Mean( numpy.array( row ) ) for row in self.SampleB.A ] )

        self.ProjVect = invscm.dot( means1 - means2 )

    #_______________________________________________________________________________
    # Applies the fisher discriminant to a given sample
    def __eval__( self, sample ):
        fisherVar  = [ self.ProjVect.dot( sample.A[ i ] ) for i in range( len( sample ) ) ]
        return fisherVar

    #_______________________________________________________________________________
    # Applies the fisher discriminant to a given sample
    def Apply( self, sample ):
        fisherVar  = [ self.ProjVect.dot( sample.A[ i ] ) for i in range( len( sample ) ) ]
        return fisherVar

    #_______________________________________________________________________________
    # Returns the values for the fisher discriminant of the two training samples
    def GetTrainFisherValues( self ):
        sampleA  = self.SampleA.T
        sampleB  = self.SampleB.T
        fisherA  = [ float(self.ProjVect.dot( sampleA.A[ i ] ).A) for i in range( len( sampleA ) ) ]
        fisherB  = [ float(self.ProjVect.dot( sampleB.A[ i ] ).A) for i in range( len( sampleB ) ) ]
        return fisherA, fisherB

    #_______________________________________________________________________________
    # Plots the histograms of the fisher discriminant values for the two training
    # samples
    def PlotFisherQuality( self, **kwargs ):
        if "nbins" in kwargs: nbins = kwargs[ "nbins" ]
        else: nbins = 100
        if "npoints" in kwargs: npoints = kwargs[ "npoints" ]
        else: npoints = 100
        if "offset" in kwargs: offset = kwargs[ "offset" ]
        else: offset = 0

        ''' Gets the fisher discriminant values for the samples '''
        fisherA, fisherB = self.GetTrainFisherValues()
        fisherTot        = fisherA + fisherB
        minv, maxv       = min( fisherTot ) - offset, max( fisherTot ) + offset

        ''' Makes the histograms with the aforementioned variables '''
        histA = TH1D( "Signal", "", nbins, minv, maxv )
        histB = TH1D( "Background", "", nbins, minv, maxv )
        for el in fisherA: histA.Fill( el )
        for el in fisherB: histB.Fill( el )
        histA.SetLineColor( 2 )
        histB.SetLineColor( 4 )

        ''' Makes the ROC, significance/cut and efficiency/cut graphs '''
        step = ( maxv - minv )*1./( npoints - 1 )
        fisherA.sort()
        fisherB.sort()
        la , lb  = len( fisherA ), len( fisherB )
        roc, sig, eff, rej = TGraph(), TGraph(), TGraph(), TGraph()
        for i in range( npoints ):
            cut    = minv + i*step
            nA, nB = 0, 0
            for el in reversed( fisherA ):
                if el > cut: nA += 1
                else: break
            for el in reversed( fisherB ):
                if el > cut: nB += 1
                else: break
            roc.SetPoint( i, nA*1./la, 1 - nB*1./lb )
            if nA == 0 and nB == 0:
                sig.SetPoint( i, cut, 1. )
            else:
                sig.SetPoint( i, cut, nA*1./sqrt( nA + nB ) )
            eff.SetPoint( i, cut, nA*1./la )
            rej.SetPoint( i, cut, 1 - nB*1./lb )

        ''' Defines the name, title, line style and marker style of the graphs '''
        roc.SetNameTitle( "ROC", "" )
        roc.GetXaxis().SetTitle( "Background rejection" )
        roc.GetYaxis().SetTitle( "Signal efficiency" )
        roc.SetLineColor( 4 ); roc.SetMarkerStyle( 8 )
        sig.SetNameTitle( "Significance", "" )
        sig.GetYaxis().SetTitle( "S/#sqrt{S + B}" )
        sig.GetXaxis().SetTitle( "FD cut" )
        sig.SetLineColor( 4 ); sig.SetMarkerStyle( 8 )
        eff.SetNameTitle( "Signal efficiency", "" )
        eff.GetXaxis().SetTitle( "FD cut" )
        eff.SetLineColor( 4 ); eff.SetMarkerStyle( 8 )
        rej.SetNameTitle( "Background rejection", "" )
        rej.GetXaxis().SetTitle( "FD cut" )
        rej.SetLineColor( 2 ); rej.SetMarkerStyle( 8 )

        ''' Draws all the plots in a canvas and returns them '''
        canvas = TCanvas()
        canvas.Divide( 2, 2 )      
        canvas.cd( 1 )
        if histA.GetMaximum() > histB.GetMaximum():
            histA.GetXaxis().SetTitle( "Fisher linear discriminant" )
            histA.DrawNormalized()
            histB.DrawNormalized( "SAME" )
        else:
            histB.GetXaxis().SetTitle( "Fisher linear discriminant" )
            histB.DrawNormalized()
            histA.DrawNormalized( "SAME" )
        canvas.cd( 2 ); eff.Draw( "APC" ); rej.Draw( "SAMEPC" );
        canvas.cd( 3 ); roc.Draw( "APC" )
        canvas.cd( 4 ); sig.Draw( "APC" )

        canvas.cd( 1 ).BuildLegend( 0.15, 0.75, 0.35, 0.85 )
        canvas.cd( 2 ).BuildLegend( 0.15, 0.75, 0.45, 0.85 )

        return { "canvas" : canvas,
                 "histA"  : histA,
                 "histB"  : histB,
                 "eff"    : eff,
                 "rej"    : rej,
                 "roc"    : roc,
                 "sig"    : sig }

#_______________________________________________________________________________
# --- FUNCTIONS ---

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
    nvars     = len( data )
    covmatrix = [ nvars*[ 0. ] for i in range( nvars ) ]
    for i in range( nvars ):
        for j in range( nvars ):
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
