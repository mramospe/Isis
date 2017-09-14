#
# CLs mu-scan
#
# author: Miguel Ramos Pernas
# email:  miguel.ramos.pernas@cern.ch
#

from Isis.iboost.analysis import ( CLsFactory, CLsFluctuator,
                                   CLsHypothesis, CLsPrior, CLsResult )
from Isis.statistics import poisson_uncert

import numpy as np
import ROOT as rt

from CLsMuScanPlotUtils import *

# Style
rt.gStyle.SetOptStat(0)
rt.gStyle.SetLineWidth(2)

# Enable/disable showing the plots
rt.gROOT.SetBatch(True)

# -------------------------------------------------------
# Configuration to be launched
class Config:

    # Background effect
    bkgEff = np.array([1.5, 1, 0.5], dtype = float)

    # Signal effect
    sigEff = np.array([2, 4, 2], dtype = float)

    # Observation
    obs = np.array([1, 1, 1], dtype = float)

    # Number of points for the scan
    npoints = 25
    
    # Output file
    output = rt.TFile('CLsMuScanOutput.root', 'RECREATE')

    # Minimum and maximum of the signal strength
    mumin = 0.
    mumax = 1.

    # Number of bins for the histogram
    nbins = 50

    # Number of events to be generated
    ngen = int(1e5)
# -------------------------------------------------------

class AsymmFluct(CLsFluctuator):
    '''
    Fluctuator for the hypotheses
    '''
    def __init__( self, means):
        '''
        Get the mean and calculate the asymmetric errors to prevent
        calculating them many times
        '''
        CLsFluctuator.__init__(self)
        self.means = means
        self.ssum  = []
        self.sdif  = []
        
        for m in self.means:
            sig_sy, sig_lo, sig_hi = poisson_uncert(int(m))
            self.ssum.append( sig_hi + sig_lo )
            self.sdif.append( sig_hi - sig_lo )

        self.rndm = rt.TRandom3()

    def fluctuate( self, pos, mean ):
        '''
        Override abstract class method. In this case, "mean" could not be used.
        '''
        r = self.rndm.Uniform()
        s = self.ssum[pos]
        d = self.sdif[pos]
        return mean*(1 + 0.5*r*s + 0.5*r**2*d)


class FlatPrior(CLsPrior):
    '''
    Basic flat prior for the CLs
    '''
    def __init__( self ):
        '''
        Empty constructor
        '''
        CLsPrior.__init__( self )

    def evaluate( self, pos, mean, value ):
        '''
        Return the identity
        '''
        return 1.
        

# Initialize CLs classes
bkgHyp  = CLsHypothesis(Config.bkgEff)
sigHyp  = CLsHypothesis()
factory = CLsFactory(bkgHyp, sigHyp)

# To store the status at each step
snapshots = []
clsValues = CLsStepList()
muValues  = []

# Build the background fluctuators and prior for each bin
bkgFluct = AsymmFluct(Config.bkgEff)
bkgHyp.setFluctuator(bkgFluct)
bkgPrior = FlatPrior()
bkgHyp.setPrior(bkgPrior)

# Set the signal prior
sigPrior = FlatPrior()
sigHyp.setPrior(sigPrior)

# Do the scan and save snapshots in folder
folder = Config.output.mkdir('SnapShots')
folder.cd()
for i, mu in enumerate(np.linspace(Config.mumax, Config.mumin, Config.npoints, endpoint = False)[::-1]):

    print '*** Start step %i (%i)' %(i, Config.npoints)

    print '-- Signal strength: %f' %mu
    
    sig = np.array([b + mu*s for b, s in zip(Config.bkgEff, Config.sigEff)], dtype = float)
    sigHyp.setHyp(sig)

    sigFluct = AsymmFluct(sig)
    sigHyp.setFluctuator(sigFluct)
    
    print '-- Expected sig/bkg: %s / %s' %(sigHyp.getHyp(), bkgHyp.getHyp())

    print '-- Generating events'
    factory.generate(Config.ngen)

    print '-- Calculating CLs'
    res_med = factory.calculate(bkgHyp.testStatFromProb(0.5))
    res_1sl = factory.calculate(bkgHyp.testStatFromProb(0.317))
    res_1sr = factory.calculate(bkgHyp.testStatFromProb(0.683))
    res_2sl = factory.calculate(bkgHyp.testStatFromProb(0.046))
    res_2sr = factory.calculate(bkgHyp.testStatFromProb(0.954))

    if len(Config.obs):
        res_obs = factory.calculate(Config.obs)
    
    print '-- Test Stat.    %f' %res_med.TestStat
    print '-- CLb:          %f' %res_med.CLb
    print '-- CLsb:         %f' %res_med.CLsb
    print '-- CLs:          %f' %res_med.CLs
    print '-- Significance: %s' %res_med.Significance
    if len(Config.obs):
        print '-- CLs for obs:  %f' %res_obs.CLs

    
    out = makeSnapShot('SnapShot_' + str(i), i, mu, bkgHyp, sigHyp, res_med.TestStat, nbins = Config.nbins)
    snapshots.append(out)

    reslst = [res_med, res_1sl, res_1sr, res_2sl, res_2sr]
    if len(Config.obs):
        reslst.append(res_obs)
    
    clsValues.append(*[res.CLs for res in reslst])
    muValues.append(mu)

    for obj in out:
        if isinstance(obj, rt.TLine):
            obj.Write('Line_' + str(i))
        else:
            if obj.GetName() != '':
                obj.Write()
            else:
                obj.Write('Format_' + str(i))

# Make CLs plot and save
clsScanPlots = makeCLsPlot(muValues, clsValues)
Config.output.cd()
for obj in clsScanPlots:
    il = 0
    if isinstance(obj, rt.TLine):
        obj.Write('Line_' + str(il))
        il += 1
    else:
        obj.Write()
