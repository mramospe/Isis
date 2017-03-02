#//////////////////////////////////////////////////////////
#//
#//  Python modules
#//
#// -------------------------------------------------------
#//
#//  AUTHOR: Miguel Ramos Pernas
#//  e-mail: miguel.ramos.pernas@cern.ch
#//
#//  Last update: 02/03/2017
#//
#// -------------------------------------------------------
#//
#//  Description:
#//
#//  Defines different statistical functions and class
#//  to manage data stored in lists.
#//
#// -------------------------------------------------------
#//////////////////////////////////////////////////////////


import ROOT as rt

from Isis.Algebra import Matrix, Inv
from Isis.Decorators import DecoInputArgs
from Isis.IBoost.PyGeneral import SendErrorMsg, SendInfoMsg, SendWarningMsg
from Isis.RootUtils import ExtractHistValues
from Isis.Utils import CalcMinDist

from math import sqrt
import numpy as np
import numbers
from scipy.special import betainc
from scipy.stats import beta


@DecoInputArgs(float, kvars = ['cl', 'prec'])
def BinomialUncert( N, k, cl = 0.683, prec = 0.01 ):
    '''
    Calculate the frequentist uncertainties associated with an observation of < k >
    events in < N >. The confidence level and precision for the results may be
    provided.
    '''
    p = k/N

    d = N - k
    
    s_sy = beta.std(k + 1, d + 1)
    
    nsteps = int(1./prec)

    vmin = p - 2*s_sy
    vmax = p + 2*s_sy

    if vmin < 0:
        vmin = 0

    lw_lst = np.linspace(vmin, p, nsteps)
    up_lst = np.linspace(vmax, p, nsteps)

    lw_probs = map(lambda x: betainc(k + 1, d + 1, x), lw_lst)
    up_probs = map(lambda x: 1. - betainc(k + 1, d + 1, x), up_lst)
    
    pb = (1. - cl)/2.

    p_lw = np.interp(pb, lw_probs, lw_lst)
    p_up = np.interp(pb, up_probs, up_lst)

    s_lw = p - p_lw
    s_up = p_up - p

    return s_sy, s_lw, s_up


def Covariance( lst1, lst2 ):
    '''
    Calculates the covariance between two lists ( with the same size )
    '''
    m1 = Mean( lst1 )
    m2 = Mean( lst2 )
    ex = [ ( v1 - m1 )*( v2 - m2 ) for v1, v2 in zip( lst1, lst2 ) ]
    return Mean( ex )


def CovMatrix( data ):
    '''
    Calculates the covariance matrix for a given set of data. The indexes of the
    data set have to correspond to the values of one of the variables.
    '''
    nvars     = len( data )
    covmatrix = Matrix( [ nvars*[ 0. ] for i in xrange( nvars ) ] )
    for i in xrange( nvars ):
        for j in xrange( nvars ):
            covmatrix[ i ][ j ] = Covariance( data[ i ], data[ j ] )
    return covmatrix


class FisherDiscriminant:
    '''
    This class allows to generate the linear Fisher discriminant variable given
    a signal and a background samples.
    '''
    def __init__( self, SigSample, BkgSample, EvtsInRows = True ):
        '''
        This class works with a signal and a background sample given as a
        list of lists. They can be provided with the events disposed as rows
        < EvtsInRows = True > or in columns ( more common ) < EvtsInRows = False >
        '''
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
        '''
        Returns the values for the fisher discriminant of the two training samples
        '''
        SigSample  = self.SigSample.Transpose()
        BkgSample  = self.BkgSample.Transpose()
        fisherSig  = self.Apply( SigSample )
        fisherBkg  = self.Apply( BkgSample )
        return fisherSig, fisherBkg

    def PlotFisherQuality( self, **kwargs ):
        '''
        Plots the histograms of the fisher discriminant values for the two training
        samples
        '''
        nbins   = kwargs.get( 'nbins', 100 )
        npoints = kwargs.get( 'npoints', 100 )
        nsig    = kwargs.get( 'nsig', 1000 )
        nbkg    = kwargs.get( 'nbkg', 1000 )
        offset  = kwargs.get( 'offset', 0 )

        ''' Gets the fisher discriminant values for the samples '''
        fisherSig, fisherBkg = self.GetTrainFisherValues()
        fisherTot  = fisherSig + fisherBkg
        minv, maxv = min( fisherTot ) - offset, max( fisherTot ) + offset

        ''' Makes the histograms with the aforementioned variables '''
        histA = rt.TH1D( 'Signal', '', nbins, minv, maxv )
        histB = rt.TH1D( 'Background', '', nbins, minv, maxv )
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
        roc, sig, eff, rej = rt.TGraph(), rt.TGraph(), rt.TGraph(), rt.TGraph()
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
                sig.SetPoint( sig.GetN(), cut,
                              sigeff*nsig*1./sqrt( sigeff*nsig + bkgeff*nbkg ) )
            eff.SetPoint( i, cut, sigeff )
            rej.SetPoint( i, cut, bkgrej )

        ''' Defines the name, title, line style and marker style of the graphs '''
        roc.SetNameTitle( 'ROC', '' )
        roc.GetXaxis().SetTitle( 'Background rejection' )
        roc.GetYaxis().SetTitle( 'Signal efficiency' )
        roc.SetLineColor( 4 ); roc.SetMarkerStyle( 8 )
        sig.SetNameTitle( 'Significance', '' )
        sig.GetYaxis().SetTitle( 'S/#sqrt{S + B}' )
        sig.GetXaxis().SetTitle( 'FD cut' )
        sig.SetLineColor( 4 ); sig.SetMarkerStyle( 8 )
        eff.SetNameTitle( 'Signal efficiency', '' )
        eff.GetXaxis().SetTitle( 'FD cut' )
        eff.SetLineColor( 4 ); eff.SetMarkerStyle( 8 )
        rej.SetNameTitle( 'Background rejection', '' )
        rej.GetXaxis().SetTitle( 'FD cut' )
        rej.SetLineColor( 2 ); rej.SetMarkerStyle( 8 )

        ''' Draws all the plots in a canvas and returns them '''
        canvas = rt.TCanvas()
        canvas.Divide( 2, 2 )
        rt.gStyle.SetOptStat( 0 )
        canvas.cd( 1 )
        if histA.GetMaximum() > histB.GetMaximum():
            histA.GetXaxis().SetTitle( 'Fisher linear discriminant' )
            histA.Draw()
            histB.Draw( 'SAME' )
        else:
            histB.GetXaxis().SetTitle( 'Fisher linear discriminant' )
            histB.Draw()
            histA.Draw( 'SAME' )
        canvas.cd( 2 ); eff.Draw( 'APC' ); rej.Draw( 'SAMEPC' );
        canvas.cd( 3 ); roc.Draw( 'APC' )
        canvas.cd( 4 ); sig.Draw( 'APC' )

        canvas.cd( 1 ).BuildLegend( 0.15, 0.75, 0.35, 0.85 )
        canvas.cd( 2 ).BuildLegend( 0.15, 0.75, 0.45, 0.85 )

        ''' Prints the maximum significance point and the cut value '''
        lst = sig.GetY()
        maxv, maxi = max( [ ( lst[ i ], i ) for i in xrange( sig.GetN() ) ] )

        print '----------------------------'
        print 'Fisher discriminant analysis'
        print '----------------------------'
        print 'Number of Background events:', nbkg
        print 'Number of Signal events:    ', nsig
        print 'Maximum significance:', maxv, 'when cutting at', minv + maxi*step
        print '----------------------------'

        return { 'canvas' : canvas,
                 'histA'  : histA,
                 'histB'  : histB,
                 'eff'    : eff,
                 'rej'    : rej,
                 'roc'    : roc,
                 'sig'    : sig }


class IntegralTransformer:
    '''
    Class to do integral transformations. The transformation is performed so the
    distribution of the given argument is flat in [0, 1]. The de-transformation
    process is also possible.
    '''
    def __init__( self, arg, weights = None ):
        '''
        The argument must be an array-like or a TH1 object. The weights may only be
        provided for the first case.
        '''
        if isinstance( arg, rt.TH1 ):

            if weights:
                SendWarningMsg('Using a TH1 object; input weights ignored')
            
            centers, values, widths = ExtractHistValues(arg)

            self.__init__(centers, values)
            
        else:
            if weights == None:
                weights = len(arg)*[1.]

            values, weights = zip(*sorted(zip(arg, weights)))

            self.Values = np.array(values)
            self.Cltve  = np.cumsum(weights)*1./sum(weights)
    
    def Transform( self, arg ):
        '''
        Return the integral transformated value(s)
        '''
        return np.interp(arg, self.Values, self.Cltve)

    def DeTransform( self, arg ):
        '''
        Return the de-transformated value(s)
        '''
        return np.interp(arg, self.Cltve, self.Values)


def LinearCorrCoeff( lst1, lst2 ):
    '''
    Calculates the linear correlation coefficient between two lists of values
    '''
    return Covariance( lst1, lst2 )*1./sqrt( StdDev2( lst1 )*StdDev2( lst2 ) )


def Mean( lst ):
    '''
    Calculates the mean of the values in a list of values
    '''
    return sum( lst )*1./len( lst )


def MeanStdDev2( lst ):
    '''
    Calculates the squared standard deviation of the mean of a list of values
    '''
    return StdDev2( lst )*1./len( lst )


def MeanStdDev( lst ):
    '''
    Calculates the standard deviation of the mean of a list of values
    '''
    return sqrt( MeanStdDev2( lst ) )


def Mode( lst ):
    '''
    Calculates the mode of a list of values
    '''
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
        SendWarningMsg('A number of < %i > modes exist in the input list' %nmodes)
    return oldmax


@DecoInputArgs(float, slc = [], kvars = ['cl', 'prec'])
def PoissonUncert( mean, cl = 0.683, prec = 0.01 ):
    '''
    Calculate the frequentist poisson uncertainties for a given integer value. The
    confidence level may be provided. Also the precision required in the outcoming
    uncertainties.
    '''
    if not isinstance(mean, numbers.Integral):
        mean = int(round(mean))
        SendWarningMsg('Calculating poisson uncertainty of a non-integer value; '\
                       'returning values for closest integer < %i >' %mean)
    
    s_sy = sqrt(mean)

    nsteps = int(2*s_sy/prec)
    lw_mean_lst = np.linspace(mean - 2*s_sy, mean, nsteps)
    up_mean_lst = np.linspace(mean + 2*s_sy, mean, nsteps)

    ''' Adding the value at < mean > is necessary '''
    lw_probs = map(lambda x: rt.Math.poisson_cdf_c(mean, x) + rt.Math.poisson_pdf(mean, x),
                   lw_mean_lst)
    up_probs = map(lambda x: rt.Math.poisson_cdf(mean, x), up_mean_lst)

    pb = (1. - cl)/2.

    mean_lw = np.interp(pb, lw_probs, lw_mean_lst)
    mean_up = np.interp(pb, up_probs, up_mean_lst)

    s_lw = mean - mean_lw
    s_up = mean_up - mean

    return s_sy, s_lw, s_up


def StatMomentum( lst, n ):
    '''
    Gets the nth momentum of the distribution of the list
    '''
    mean = Mean( lst )
    res  = 0
    for el in lst:
        res += ( el - mean )**n
    return res*1./len( lst )
    

def StdDev2( lst ):
    '''
    Calculates the squared standard deviation of a list of values
    '''
    mean = Mean( lst )
    res  = 0
    for el in lst:
        res += ( el - mean )**2
    return res*1./len( lst )


def StdDev( lst ):
    '''
    Calculates the standard deviation of a list of values
    '''
    return sqrt( StdDev2( lst ) )
