#//////////////////////////////////////////////////////////
#//                                                      //
#//  Python modules                                      //
#//                                                      //
#// ---------------------------------------------------- //
#//                                                      //
#//  AUTHOR: Miguel Ramos Pernas                         //
#//  e-mail: miguel.ramos.pernas@cern.ch                 //
#//                                                      //
#//  Last update: 21/04/2016                             //
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


from ROOT import gStyle, TCanvas, TH1, TH1D, TGraph
from Isis.Algebra import Matrix, Inv
from Isis.Utils import CalcMinDist
from math import sqrt


#_______________________________________________________________________________
# This class allows to generate the linear Fisher discriminant variable given
# a signal and a background samples.
class FisherDiscriminant:

    def __init__( self, SigSample, BkgSample, EvtsInRows = True ):
        ''' This class works with a signal and a background sample given as a
        list of lists. They can be provided with the events disposed as rows
        < EvtsInRows = True > or in columns ( more common ) < EvtsInRows = False > '''
        if EvtsInRows:
            self.SigSample = Matrix( SigSample ).Transpose()
            self.BkgSample = Matrix( BkgSample ).Transpose()
        else:
            self.SigSample = Matrix( SigSample )
            self.BkgSample = Matrix( BkgSample )
        cmatrixSig     = Matrix( CovMatrix( self.SigSample ) )
        cmatrixBkg     = Matrix( CovMatrix( self.BkgSample ) )
        invscm         = Inv( cmatrixSig + cmatrixBkg )
        meansSig       = Matrix( [ [ Mean( row ) for row in self.SigSample ] ] )
        meansBkg       = Matrix( [ [ Mean( row ) for row in self.BkgSample ] ] )
        self.ProjVect  = invscm.Dot( ( meansSig - meansBkg ).Transpose() ).Transpose()[ 0 ]

    def __call__( self, sample ):
        ''' Applies the fisher discriminant to a given sample '''
        return self.Apply( sample )

    def Apply( self, sample ):
        ''' Applies the fisher discriminant to a given sample '''
        fisherVar  = [ self.ProjVect.Dot( sample[ i ] ) for i in xrange( len( sample ) ) ]
        return fisherVar

    def GetTrainFisherValues( self ):
        ''' Returns the values for the fisher discriminant of the two training samples '''
        SigSample  = self.SigSample.Transpose()
        BkgSample  = self.BkgSample.Transpose()
        fisherSig  = self.Apply( SigSample )
        fisherBkg  = self.Apply( BkgSample )
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
        for i in xrange( npoints ):
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
        maxv, maxi = max( [ ( lst[ i ], i ) for i in xrange( sig.GetN() ) ] )

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
    covmatrix = Matrix( [ nvars*[ 0. ] for i in xrange( nvars ) ] )
    for i in xrange( nvars ):
        for j in xrange( nvars ):
            covmatrix[ i ][ j ] = Covariance( data[ i ], data[ j ] )
    return covmatrix

#_______________________________________________________________________________
# Returns the two Kolmogorov-Smirnov factors. The input parameters can be
# iterable objects or TH1 histograms. The < smpRef > variable will be taken
# as the "reference" and < smpObs > as the distribution to check if matches. If
# a list or similar class is provided, by default the analysis will be unbinned,
# controlling it with the < binned > option.
def KolmogorovSmirnovTest( smpRef, smpObs, **kwargs ):
    if "binned" in kwargs: binned = kwargs[ "binned" ]
    else: binned = False
    if all( issubclass( smp.__class__, TH1 ) for smp in ( smpRef, smpObs ) ):
        ''' If the classes are histograms it works using the bins contents '''
        nbins = smpRef.GetNbinsX()
        if nbins != smpObs.GetNbinsX():
            print "ERROR: The number of bins is different for both samples"
            return
        elif smpRef.GetXaxis().GetXmin() != smpObs.GetXaxis().GetXmin():
            print "ERROR: The minimum values for the axis of the histograms do not match"
            return
        elif smpRef.GetXaxis().GetXmax() != smpObs.GetXaxis().GetXmax():
            print "ERROR: The maximum values for the axis of the histograms do not match"
            return
    else:
        if binned:
            ''' This is the task performed when one wants to consider binned lists '''
            if "nbins" in kwargs: nbins = kwargs[ "nbins" ]
            else: nbins = 100
            if "vmin" in kwargs: vmin = kwargs[ "vmin" ]
            else: vmin = min( smpRef + smpObs )
            if "vmax" in kwargs: vmax = kwargs[ "vmax" ]
            else: vmax = max( smpRef + smpObs ) + CalcMinDist( smpRef + smpObs )*1./2
            smpRef = MakeHistogram( 'ref', smpRef, nbins = nbins, vmin = vmin, vmax = vmax )
            smpObs = MakeHistogram( 'obs', smpObs, nbins = nbins, vmin = vmin, vmax = vmax )
        else:
            ''' This is what is performed when one works with unbinned distributions '''
            if "npoints" in kwargs: npoints = kwargs[ "npoints" ]
            else: npoints = 100
            smpRef  = list( smpRef )
            smpObs  = list( smpObs )
            mainlst = smpRef + smpObs
            vmax    = max( mainlst )
            vmin    = min( mainlst )
            step    = float( vmax - min( mainlst ) )/npoints
            points  = [ vmin + i*step for i in xrange( 1, npoints + 1 ) ]
            points[ -1 ] += step
            cRef, cObs = 1./len( smpRef ), 1./len( smpObs )
            smpRef.sort()
            smpObs.sort()
            Dp, Dm = npoints*[ 0. ], npoints*[ 0. ]
            for ip, p in enumerate( points ):
                nRef, nObs = 0, 0
                i = 0
                while smpRef[ i ] < p:
                    i += 1
                    nRef += cRef
                smpRef = smpRef[ i: ]
                i = 0
                while smpObs[ i ] < p:
                    i += 1
                    nObs += cObs
                smpObs = smpObs[ i: ]
                Dp[ ip ] = abs( nObs - nRef )
                Dm[ ip ] = abs( nRef - nObs )
            return max( Dp ), max( Dm )
    ''' Calculates the sum of weights and creates a list with the content of all the bins '''
    nRef, nObs = [ smp.GetSumOfWeights() for smp in ( smpRef, smpObs ) ]
    cRef = [ smpRef.GetBinContent( i )/nRef for i in xrange( 1, nbins + 1 ) ]
    cObs = [ smpObs.GetBinContent( i )/nObs for i in xrange( 1, nbins + 1 ) ]
    ''' Calculates the cumulated values '''
    for i in xrange( 1, nbins ):
        cRef[ i ] += cRef[ i - 1 ]
        cObs[ i ] += cObs[ i - 1 ]
    ''' Calculates the Kolmogorov-Smirnov parameters '''
    Dp, Dm = nbins*[ 0. ], nbins*[ 0. ]
    for ib in xrange( nbins ):
        Dp[ ib ] = abs( cObs[ ib ] - cRef[ ib ] )
        Dm[ ib ] = abs( cRef[ ib ] - cObs[ ib ] )
    return max( Dp ), max( Dm )

#_______________________________________________________________________________
# Calculates the linear correlation coefficient between two lists of values
def LinearCorrCoeff( lst1, lst2 ):
    return Covariance( lst1, lst2 )*1./sqrt( StdDev2( lst1 )*StdDev2( lst2 ) )

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
