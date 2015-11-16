from ROOT import *
from numpy import append


class CLsClass:

    def __init__( self, name, SigEvts, BkgEvts, n , nbins):
        ''' Given a name, the signal events, the background events ( both as lists ),
        the number of iterations ( to get the <t> distribution ) and the number of bins
        in the histograms, gets the distribution of the signal and background events in
        the <t> variable.
        '''

        self.Length = len( SigEvts )
        self.SigEvts = SigEvts
        self.BkgEvts = BkgEvts

        self.SigHTest = []
        self.BkgHTest = []  
        for i in range( n ):
            sig_al = []
            bkg_al = []
            for m in range( self.Length ):
                sig_al += [gRandom.Poisson( self.SigEvts[m] )]
                bkg_al += [gRandom.Poisson( self.BkgEvts[m] )]

            self.BkgHTest += [self.t( bkg_al )]
            self.SigHTest += [self.t( sig_al )]

        self.BkgHTest.sort()
        self.SigHTest.sort()

        xmin  = min( append( self.SigHTest, self.BkgHTest ) )
        xmax  = max( append( self.SigHTest, self.BkgHTest ) )
        xmean = ( xmax - xmin ) / 2
        xmin -= xmean;
        xmax += xmean;

        self.BkgHist = TH1D( "BkgHist_" + name, "BkgHist_" + name, nbins, xmin, xmax )
        self.SigHist = TH1D( "SigHist_" + name, "SigHist_" + name, nbins, xmin, xmax )

        for l in self.BkgHTest:
            self.BkgHist.Fill( l )
        for l in self.SigHTest:
            self.SigHist.Fill( l )


    def GetAlpha( self, t ):
        ''' Gets the p-value for the background hypothesis '''
        return self.GetPValue( self.BkgHTest, t, "bkg" )
        

    def GetBeta( self, t ):
        ''' Gets the p-value for the signal hypothesis '''
        return self.GetPValue( self.SigHTest, t, "sig" )

    
    def GetCLs( self, obs ):
        ''' Gets the value of the CLs given the observed data '''
        return self.GetBeta( self.t( obs ) )/( 1 - self.GetAlpha( self.t( obs ) ) )


    def GetPValue( self, lst , t0, tp):
        ''' Gets the p-value given its <t> distribution, the variable for the
        observed data and the type of the distribution bkg/sig '''
        evts = 0
        n    = len( lst )
        if tp == "sig":
            for i in range( n ):
                if lst[i] < t0:
                    evts += 1
        elif tp == "bkg":
            for i in range( n ):
                if lst[i] > t0:
                    evts += 1
        return evts/float(n)

    def GetQCLs( self, q ):
        ''' Gets the CLs for a given percentage of deviation from the background
        hypothesis '''
        tq = self.BkgHTest[int( q*len( self.BkgHTest ) )]
        return self.GetBeta( tq )/( 1 - self.GetAlpha( tq ) )

    def GetROC( self ):
        ''' Computes the ROC curve '''
        tlist    = self.BkgHTest + self.SigHTest
        new_list = []
        ROC      = TGraph()
        j = 0
        for i in range( len( tlist ) ):
            if all( k < tlist[i] for k in new_list ):
                ROC.SetPoint( j,
                              self.GetAlpha( tlist[i] ),
                              1 - self.GetBeta( tlist[i] ) )
                j += 1
                new_list += [tlist[i]]
        return ROC

    def t(self, x):
        ''' Poissonian definition of the <t> variable '''
        res = 1.
        for i in range( self.Length ):
            res *= float( self.BkgEvts[i]**x[i] )/self.SigEvts[i]**x[i]*TMath.Exp(
                - self.BkgEvts[i] + self.SigEvts[i] )
        return -2*TMath.Log( res )
