#/////////////////////////////////////////////////////////////////
#//
#//  Python modules
#//
#// --------------------------------------------------------------
#//
#//  AUTHOR: Miguel Ramos Pernas
#//  e-mail: miguel.ramos.pernas@cern.ch
#//
#//  Last update: 24/03/2017
#//
#// --------------------------------------------------------------
#//
#//  Description:
#//
#//  Functions to work with efficiencies and their uncertainties
#//
#// --------------------------------------------------------------
#/////////////////////////////////////////////////////////////////


from Isis.IBoost.PyGeneral import sendErrorMsg
from Isis.Decorators import decoArgBase, decoClassMethod, decoInputArgs
from Isis.Statistics import binomialUncert
from math import sqrt
from scipy.optimize import fsolve
from scipy.special import betainc, binom
from scipy.stats import beta
import numpy as np


@decoInputArgs(float)
def binomialEffUncert( N, k ):
    '''
    Return the binomial efficiency uncertainty of having < k > events in < N >
    '''
    return sqrt(k*(1. - k/N))/N


class decoEffInput( decoArgBase ):
    '''
    Decorator to check the input parameters to calculate the efficiency
    '''
    def __init__( self, func ):
        '''
        Call base function
        '''
        decoArgBase.__init__(self, func)

    @decoClassMethod
    @decoInputArgs(float, slc = slice(1, 3), kvars = None)
    def __call__( self, N, k, *args, **kwargs ):
        '''
        Check the input numbers and call the stored function
        '''
        kvars = kvars or ['cl']
        
        if k > N:
            sendErrorMsg('Calculating efficiency with a final number of '\
                         'events greater than the initial')
            return
        
        return self.func(N, k, *args, **kwargs)
            

@decoEffInput
def calcEfficiency( N, k, cl = 0.683 ):
    '''
    Calculate the efficiency and its associated asymmetric uncertainties
    from the initial and final number of events. The confidence level
    can be specified. Returns the efficiency, the symmetric efficiency
    and the lower and upper intervals associated to the given CL.
    '''
    p = k/N
    s_sy, s_lw, s_up = binomialUncert(N, k, cl)
    
    return p, s_sy, s_lw, s_up


@decoEffInput
def calcBayesAsyEfficiency( N, k, cl = 0.683 ):
    '''
    Calculate the bayesian efficiency and uncertainties. In this case the observed
    efficiency is considered as the true efficiency.
    '''
    p = k/N
    d = N - k
    
    p_unc = beta.std(k + 1, d + 1)
    a0 = abs(p - p_unc)
    b0 = abs(p + p_unc)
    
    def nleq( abr ):
        '''
        Main set of non-linear equations using Lagrange multipliers
        '''
        a, b, r = abr
        
        fa = 1. - r*beta.pdf(a, k + 1, d + 1)
        fb = 1. - r*beta.pdf(b, k + 1, d + 1)
        fr = betainc(k + 1, d + 1, b) - betainc(k + 1, d + 1, a) - cl
        
        return (fa, fb, fr)

    def nleq_jac( abr ):
        '''
        Jacobian to ease the process
        '''
        a, b, r = abr

        apdf = beta.pdf(a, k + 1, d + 1)
        bpdf = beta.pdf(b, k + 1, d + 1)
        
        fa_ja = r*apdf*(d/(1. - a) - k/a)
        fa_jb = 0.
        fa_jr = -apdf

        fb_ja = 0.
        fb_jb = r*bpdf*(d/(1. - b) - k/b)
        fb_jr = -bpdf
        
        fr_ja = -apdf
        fr_jb = bpdf
        fr_jr = 0.

        return ((fa_ja, fa_jb, fa_jr), (fb_ja, fb_jb, fb_jr), (fr_ja, fr_jb, fr_jr))
        
    def nleq_a( ar ):
        '''
        Non-linear equations if the efficiency is one
        '''
        a, r = ar
        
        fa = 1. - r*beta.pdf(a, k + 1, d + 1)
        fr = 1. - betainc(k + 1, d + 1, a) - cl
        
        return (fa, fr)

    def nleq_a_jac( ar ):
        '''
        Jacobian if the efficiency is one
        '''
        a, r = ar

        apdf = beta.pdf(a, k + 1, d + 1)
        
        fa_ja = r*apdf*(d/(1. - a) - k/a)
        fa_jr = -apdf

        fr_ja = -apdf
        fr_jr = 0.

        return ((fa_ja, fa_jr), (fr_ja, fr_jr))

    def nleq_b( br ):
        '''
        Non-linear equations if the efficiency is zero
        '''
        b, r = br
        
        fb = 1. - r*beta.pdf(b, k + 1, d + 1)
        fr = betainc(k + 1, d + 1, b) - cl
        
        return (fb, fr)

    def nleq_b_jac( br ):
        '''
        Jacobian if the efficiency is zero
        '''
        b, r = br

        bpdf = beta.pdf(b, k + 1, d + 1)
        
        fb_jb = r*bpdf*(d/(1. - b) - k/b)
        fb_jr = -bpdf
        
        fr_jb = bpdf
        fr_jr = 0.

        return ((fb_jb, fb_jr), (fr_jb, fr_jr))
    
    '''
    Depending on the value for the efficiency a different set
    of non-linear equations is solved
    '''
    if 1. - b0 < 0:
        l0 = 1./beta.pdf(a0, k + 1, d + 1)
    else:
        l0 = 1./beta.pdf(b0, k + 1, d + 1)
    
    if k == 0:
        a  = 0.
        b0 = 1./N
        b, r = fsolve(nleq_b, (b0, l0), fprime = nleq_b_jac)
    elif k == N:
        b  = 1.
        a0 = (N - 1.)/N
        a, r = fsolve(nleq_a, (a0, l0), fprime = nleq_a_jac)
    else:
        a, b, r = fsolve(nleq, (a0, b0, l0), fprime = nleq_jac)

    s_sy = p_unc
    s_lw = p - a
    s_up = b - p

    return p, s_sy, s_lw, s_up, r
    

@decoEffInput
def calcBayesSymEfficiency( N, k, cl = 0.683 ):
    '''
    Symmetric bayesian efficiency and uncertainty. In this case the efficiency
    is set to the mean of the distribution generated by having < k > events in
    < N >. The uncertainties are thus considered to be symmetric. One could make
    use of the method < beta.interval > from scipy, however it has usually small
    asymmetric variations, so for an interval [a, b] => < mean - a > ~ < b - mean >,
    but not exactly the same.
    '''
    d    = N - k
    mean = beta.mean(k + 1, d + 1)
    p0   = binomialEffUncert(N, k)
    
    fc = lambda x: betainc(k + 1, d + 1, mean + x) - betainc(k + 1, d + 1, mean - x) - cl
    jc = lambda x: beta.pdf(mean + x, k + 1, d + 1) + beta.pdf(mean - x, k + 1, d + 1)
    
    dist = fsolve(fc, p0, fprime = jc)[ 0 ]

    return mean, dist


def calcRejection( N, k, cl = 0.683 ):
    '''
    Calculate rejection for havin < k > elements in < N >. For more details
    see < CalcEfficiency >. Asymmetric uncertainties are swapped.
    '''
    p, s_sy, s_up, s_lw = calcEfficiency(N, k, cl)

    return 1. - p, s_sy, s_lw, s_up


def rocValues( var, sig, bkg,
               sense    = '>',
               npoints  = 100,
               vmin     = None,
               vmax     = None,
               endpoint = True ):
    '''
    Calculate the ROC values for two samples of signal and background events over
    the variable < var >. Two lists are returned containing the output of the
    < calcEfficiency > and < calcRejection > methods.
    '''

    nS = sig.runCutEntries(var, sense, npoints, vmin, vmax, endpoint)
    nB = bkg.runCutEntries(var, sense, npoints, vmin, vmax, endpoint)
    
    nS_glob = sig.getEntries()
    nB_glob = bkg.getEntries()
    
    eff = map(lambda x: CalcEfficiency(nS_glob, x), nS)
    rej = map(lambda x: CalcRejection(nB_glob, x), nB)

    return eff, rej
