#//////////////////////////////////////////////////////////
#//                                                      //
#//  Python modules                                      //
#//                                                      //
#// ---------------------------------------------------- //
#//                                                      //
#//  AUTHOR: Miguel Ramos Pernas                         //
#//  e-mail: miguel.ramos.pernas@cern.ch                 //
#//                                                      //
#//  Last update: 16/11/2015                             //
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


from ROOT import gStyle, TCanvas, TH1D, TGraph
import numpy
from math import sqrt


#_______________________________________________________________________________
# This class allows to generate the linear Fisher discriminant variable given
# a signal and a background samples.
class FisherDiscriminant:

    def __init__( self, SigSample, BkgSample ):
        ''' This class works with a signal and a background samples given as a
        list of lists. Each one corresponds to one variable.'''
        self.SigSample = numpy.matrix( SigSample )
        self.BkgSample = numpy.matrix( BkgSample )
        cmatrixSig = numpy.matrix( CovMatrix( self.SigSample.A ) )
        cmatrixBkg = numpy.matrix( CovMatrix( self.BkgSample.A ) )
        invscm     = numpy.linalg.inv( cmatrixSig + cmatrixBkg )
        meansSig   = numpy.array( [ Mean( numpy.array( row ) ) for row in self.SigSample.A ] )
        meansBkg   = numpy.array( [ Mean( numpy.array( row ) ) for row in self.BkgSample.A ] )
        self.ProjVect = invscm.dot( meansSig - meansBkg )

    def __eval__( self, sample ):
        ''' Applies the fisher discriminant to a given sample '''
        fisherVar  = [ self.ProjVect.dot( sample.A[ i ] ) for i in range( len( sample ) ) ]
        return fisherVar

    def Apply( self, sample ):
        ''' Applies the fisher discriminant to a given sample '''
        fisherVar  = [ self.ProjVect.dot( sample.A[ i ] ) for i in range( len( sample ) ) ]
        return fisherVar

    def GetTrainFisherValues( self ):
        ''' Returns the values for the fisher discriminant of the two training samples '''
        SigSample  = self.SigSample.T
        BkgSample  = self.BkgSample.T
        fisherSig  = [ float(self.ProjVect.dot( SigSample.A[ i ] ).A) for i in range( len( SigSample ) ) ]
        fisherBkg  = [ float(self.ProjVect.dot( BkgSample.A[ i ] ).A) for i in range( len( BkgSample ) ) ]
        return fisherSig, fisherBkg

    def PlotFisherQuality( self, **kwargs ):
        ''' Plots the histograms of the fisher discriminant values for the two training
        samples '''
        if "nbins" in kwargs: nbins = kwargs[ "nbins" ]
        else: nbins = 100
        if "npoints" in kwargs: npoints = kwargs[ "npoints" ]
        else: npoints = 100
        if "nsig" in kwargs: nsig = kwargs[ "nsig" ]
        else: nsig = 1000
        if "nbkg" in kwargs: nbkg = kwargs[ "nbkg" ]
        else: nbkg = 1000
        if "offset" in kwargs: offset = kwargs[ "offset" ]
        else: offset = 0

        ''' Gets the fisher discriminant values for the samples '''
        fisherSig, fisherBkg = self.GetTrainFisherValues()
        fisherTot  = fisherSig + fisherBkg
        minv, maxv = min( fisherTot ) - offset, max( fisherTot ) + offset

        ''' Makes the histograms with the aforementioned variables '''
        histA = TH1D( "Signal", "", nbins, minv, maxv )
        histB = TH1D( "Background", "", nbins, minv, maxv )
        for el in fisherSig: histA.Fill( el )
        for el in fisherBkg: histB.Fill( el )
        histA.Scale( nsig*1./histA.GetEntries() )
        histB.Scale( nbkg*1./histB.GetEntries() )
        histA.SetLineColor( 2 )
        histB.SetLineColor( 4 )

        ''' Makes the ROC, significance/cut and efficiency/cut graphs '''
        step = ( maxv - minv )*1./( npoints - 1 )
        fisherSig.sort()
        fisherBkg.sort()
        la , lb  = len( fisherSig ), len( fisherBkg )
        roc, sig, eff, rej = TGraph(), TGraph(), TGraph(), TGraph()
        for i in range( npoints ):
            cut    = minv + i*step
            nA, nB = 0, 0
            for el in reversed( fisherSig ):
                if el > cut: nA += 1
                else: break
            for el in reversed( fisherBkg ):
                if el > cut: nB += 1
                else: break
            sigeff = nA*1./la
            bkgeff = nB*1./lb
            bkgrej = 1 - bkgeff
            roc.SetPoint( i, sigeff, bkgrej )
            if ( sigeff + bkgeff ) != 0:
                sig.SetPoint( sig.GetN(), cut, sigeff*nsig*1./sqrt( sigeff*nsig + bkgeff*nbkg ) )
            eff.SetPoint( i, cut, sigeff )
            rej.SetPoint( i, cut, bkgrej )

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
        gStyle.SetOptStat( 0 )
        canvas.cd( 1 )
        if histA.GetMaximum() > histB.GetMaximum():
            histA.GetXaxis().SetTitle( "Fisher linear discriminant" )
            histA.Draw()
            histB.Draw( "SAME" )
        else:
            histB.GetXaxis().SetTitle( "Fisher linear discriminant" )
            histB.Draw()
            histA.Draw( "SAME" )
        canvas.cd( 2 ); eff.Draw( "APC" ); rej.Draw( "SAMEPC" );
        canvas.cd( 3 ); roc.Draw( "APC" )
        canvas.cd( 4 ); sig.Draw( "APC" )

        canvas.cd( 1 ).BuildLegend( 0.15, 0.75, 0.35, 0.85 )
        canvas.cd( 2 ).BuildLegend( 0.15, 0.75, 0.45, 0.85 )

        ''' Prints the maximum significance point and the cut value '''
        lst = sig.GetY()
        maxv, maxi = max( [ ( lst[ i ], i ) for i in range( sig.GetN() ) ] )

        print "----------------------------"
        print "Fisher discriminant analysis"
        print "----------------------------"
        print "Number of Background events:", nbkg
        print "Number of Signal events:    ", nsig
        print "Maximum significance:", maxv, "when cutting at", minv + maxi*step
        print "----------------------------"

        return { "canvas" : canvas,
                 "histA"  : histA,
                 "histB"  : histB,
                 "eff"    : eff,
                 "rej"    : rej,
                 "roc"    : roc,
                 "sig"    : sig }

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
