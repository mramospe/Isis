#//////////////////////////////////////////////////////////
#//
#//  Python modules
#//
#// -------------------------------------------------------
#//
#//  AUTHOR: Miguel Ramos Pernas
#//  e-mail: miguel.ramos.pernas@cern.ch
#//
#//  Last update: 28/09/2017
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


from Isis.decorators import DecoArgBase, deco_class_meth, deco_input_args
from Isis.iboost.general import sendErrorMsg, sendWarningMsg
from Isis.iroot import ROOT as rt
from Isis.root_utils import hist_values

from math import sqrt
import numpy as np
import numbers
from scipy.optimize import fsolve
from scipy.special import betainc, binom
from scipy.stats import beta


class DecoEffInput( DecoArgBase ):
    '''
    Decorator to check the input parameters to calculate the efficiency
    '''
    def __init__( self, func ):
        '''
        Call base function
        '''
        DecoArgBase.__init__(self, func)

    @deco_class_meth
    @deco_input_args(float, slc = (1, 2, 3), kvars = ['cl'])
    def __call__( self, N, k, *args, **kwargs ):
        '''
        Check the input numbers and call the stored function
        '''
        if k > N:
            sendErrorMsg('Calculating efficiency with a final number of '\
                         'events greater than the initial')
            return
        
        return self.func(N, k, *args, **kwargs)


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
                sendWarningMsg('Using a TH1 object; input weights ignored')
            
            centers, values, widths = hist_values(arg)

            self.__init__(centers, values)
            
        else:
            if weights == None:
                weights = len(arg)*[1.]

            values, weights = zip(*sorted(zip(arg, weights)))

            values = np.array(values)
            cltve  = np.cumsum(weights)*1./sum(weights)

            ''' Remove duplicates (needed if working with integers) '''
            vset = set(values)
            if len(vset) != len(values):
                for v in vset:
                    
                    places = np.where(values == v)[0]

                    if len(places) > 1:
                        
                        ip = places[0]
                        rm = places[1:]

                        cltve[ip] = cltve[rm[-1]]

                        cltve  = np.delete(cltve, rm)
                        values = np.delete(values, rm)
                
            self._values = values
            self._cltve  = cltve
                
    
    def transform( self, arg ):
        '''
        Return the integral transformated values
        '''
        return np.interp(arg, self._values, self._cltve)

    def detransform( self, arg ):
        '''
        Return the de-transformated values
        '''
        return np.interp(arg, self._cltve, self._values)


@DecoEffInput
def bayes_asym_eff( N, k, cl = 0.683 ):
    '''
    Calculate the bayesian efficiency and uncertainties, forcing the two
    bounds to be as closer as possible. This means that the uncertainties
    only have meaning when they are considered together, since
    integral(mean, mean + s_up) != integral(mean - s_lw, mean).
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


@DecoEffInput
def bayes_sym_eff( N, k, cl = 0.683 ):
    '''
    Symmetric bayesian efficiency and uncertainty. In this case the efficiency
    is set to the mean of the distribution generated by having < k > events in
    < N >. The uncertainties are thus considered to be symmetric. One could make
    use of the method < beta.interval > from scipy, however it has usually small
    asymmetric variations, so for an interval [a, b] => < mean - a > ~ < b - mean >.
    '''
    d    = N - k
    mean = beta.mean(k + 1, d + 1)
    p0   = binomial_eff_uncert(N, k)
    
    fc = lambda x: betainc(k + 1, d + 1, mean + x) - betainc(k + 1, d + 1, mean - x) - cl
    jc = lambda x: beta.pdf(mean + x, k + 1, d + 1) + beta.pdf(mean - x, k + 1, d + 1)
    
    dist = fsolve(fc, p0, fprime = jc)[ 0 ]

    return mean, dist


@deco_input_args(float)
def binomial_eff_uncert( N, k ):
    '''
    Return the binomial efficiency uncertainty of having < k > events in < N >
    '''
    return sqrt(k*(1. - k/N))/N
            

@deco_input_args(float, kvars = ['cl', 'prec'])
def binomial_uncert( N, k, cl = 0.683, prec = 0.01 ):
    '''
    Calculate the bayesian uncertainties associated with an observation of < k >
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


@DecoEffInput
def efficiency( N, k, cl = 0.683 ):
    '''
    Calculate the efficiency and its associated asymmetric uncertainties
    from the initial and final number of events. The confidence level
    can be specified. Returns the efficiency, the symmetric efficiency
    and the lower and upper intervals associated to the given CL.
    '''
    p = k/N
    s_sy, s_lw, s_up = binomial_uncert(N, k, cl)
    
    return p, s_sy, s_lw, s_up


@deco_input_args(float, slc = [], kvars = ['cl', 'prec'])
def poisson_uncert( mean, cl = 0.683, prec = 0.01 ):
    '''
    Calculate the frequentist poisson uncertainties for a given integer value. The
    confidence level may be provided. Also the precision required in the outcoming
    uncertainties.
    '''
    if not isinstance(mean, numbers.Integral):
        mean = int(round(mean))
        sendWarningMsg('Calculating poisson uncertainty of a non-integer value; '\
                       'returning values for closest integer < %i >' %mean)

    s_sy = sqrt(mean)
        
    if mean != 0:
        stp = s_sy
    else:
        stp = 2.

    nsteps = int(2*stp/prec)
    
    pb = (1. - cl)/2.
    
    if mean != 0:
        ''' Adding the value at < mean > is necessary '''
        lw_mean_lst = np.linspace(mean - 2*stp, mean, nsteps)
        lw_probs    = map(lambda x: rt.Math.poisson_cdf_c(mean, x) + rt.Math.poisson_pdf(mean, x),
                       lw_mean_lst)
        mean_lw = np.interp(pb, lw_probs, lw_mean_lst)
        s_lw    = mean - mean_lw
    else:
        s_lw = 0.
    
    up_mean_lst = np.linspace(mean + 2*stp, mean, nsteps)
    up_probs    = map(lambda x: rt.Math.poisson_cdf(mean, x), up_mean_lst)
    mean_up     = np.interp(pb, up_probs, up_mean_lst)

    s_up = mean_up - mean
    
    return s_sy, s_lw, s_up


def rejection( N, k, cl = 0.683 ):
    '''
    Calculate rejection for having < k > elements in < N >. For more details
    see < CalcEfficiency >. Asymmetric uncertainties are swapped.
    '''
    p, s_sy, s_up, s_lw = efficiency(N, k, cl)

    return 1. - p, s_sy, s_lw, s_up


def roc_vals( var, sig, bkg,
               sense    = '>',
               npoints  = 100,
               vmin     = None,
               vmax     = None,
               endpoint = True ):
    '''
    Calculate the ROC values for two samples of signal and background events over
    the variable < var >. Two lists are returned containing the output of the
    < efficiency > and < rejection > methods.
    '''

    nS = sig.runCutEntries(var, sense, npoints, vmin, vmax, endpoint)
    nB = bkg.runCutEntries(var, sense, npoints, vmin, vmax, endpoint)
    
    nS_glob = sig.entries()
    nB_glob = bkg.entries()
    
    eff = map(lambda x: CalcEfficiency(nS_glob, x), nS)
    rej = map(lambda x: CalcRejection(nB_glob, x), nB)

    return eff, rej
