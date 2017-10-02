#//////////////////////////////////////////////////////////
#//
#//  Python modules
#//
#// -------------------------------------------------------
#//
#//  AUTHOR: Miguel Ramos Pernas
#//  e-mail: miguel.ramos.pernas@cern.ch
#//
#//  Last update: 02/10/2017
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


from Isis.decorators import deco_input_args
from Isis.iboost.general import sendWarningMsg
from Isis.iroot import ROOT as rt
from Isis.root_utils import hist_values

from math import sqrt
import numpy as np
import scipy.stats as st
from scipy.interpolate import interp1d


class config:
    '''
    Define default values of the functions in this module
    '''
    cl   = st.chi2(1).cdf(1) # 1 sigma
    disc = False
    nsig = 5
    prec = 0.01


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
        return interp1d(self._values, self._cltve)(arg)

    def detransform( self, arg ):
        '''
        Return the de-transformated values
        '''
        return interp1d(self._cltve, self._values)(arg)


@deco_input_args(float, slc = range(1), kvars = ['cl'])
def bayes_asy_poisson_uncert( n, cl = config.cl, disc = config.disc, nsig = config.nsig ):
    '''
    Return the asymmetric poisson uncertainty of having "n"
    events (must be an integer). If "disc" is set to true, the
    integer interval will be returned.
    '''
    pdf = st.poisson(n)
    m   = pdf.mean()
    
    if disc:
        lw, up = pdf.interval(cl)
    else:
        s = pdf.std()

        lb = round(m - nsig*s)
        if lb < 0:
            lb = 0
        rb = round(m + nsig*s)
        
        larr = np.linspace(lb, m, num = m - lb, endpoint = False)
        rarr = np.linspace(rb, m, num = rb - m, endpoint = False)
        
        pm = pdf.pmf(m)
        cm = pdf.cdf(m)
        lv = cm - pm - pdf.cdf(larr)
        rv = pdf.cdf(rarr) - cm
        
        p = (cl - pm)/2.

        lw = interp1d(lv, larr)(p)
        up = interp1d(rv, rarr)(p)
        
    return m - lw, up - m


@deco_input_args(float, kvars = ['cl'])
def bayes_asy_eff( N, k, cl = config.cl, disc = config.disc ):
    '''
    Return the efficiency and asymmetric uncertainties of having
    "k" elements in "N".
    '''
    p = k/N
    
    if disc:
        pdf = st.binom(N, p)
        # The binomial distribution works with "k"
        lw, up = np.array(pdf.interval(cl))/N
    else:
        pdf = st.beta(k + 1, N - k + 1)
        # The beta distribution works directly with "p"
        lw, up = pdf.interval(cl)
    
    m = pdf.mean()
    
    return p, p - lw, up - p


def bayes_eff( N, k, cl = config.cl, disc = config.disc ):
    '''
    Return the efficiency and the symmetric and asymmetric
    uncertainties. The values returned are float.
    '''
    eff, s_sy = bayes_sym_eff(N, k, disc = disc)
    eff, s_lw, s_up = bayes_asy_eff(N, k, disc = disc)

    return eff, s_sy, s_lw, s_up


@deco_input_args(float, slc = range(2))
def bayes_sym_eff( N, k, disc = config.disc ):
    '''
    Return the efficiency and symmetric uncertainty of having
    "k" elements in "N".
    '''
    p = k/N
    
    if disc:
        pdf = st.binom(N, p)
        s = pdf.std()/N
    else:
        pdf = st.beta(k + 1, N - k + 1)
        s = pdf.std()

    return p, s


@deco_input_args(float, kvars = ['cl', 'prec'])
def freq_eff( N, k, cl = config.cl, prec = config.prec, nsig = config.nsig ):
    '''
    Return the frequentist efficiency of having "k" events
    in "N". The confidence level and the precision can be
    provided.
    '''
    p = k/N

    s_k = nsig*sqrt(k)
    s_N = nsig*sqrt(N)

    n_k = int(s_k/prec)
    n_N = int(s_N/prec)

    # Create the grid
    m_k = k - s_k
    v_k = np.linspace(m_k if m_k > 0 else 0, k + s_k, n_k)
    m_N = N - s_N
    v_N = np.linspace(m_N if m_N > 0 else 1, N + s_N, n_N)
    
    g_k, g_N = np.meshgrid(v_k, v_N)
    
    cond = (g_N >= g_k)
    g_k  = g_k[cond]
    g_N  = g_N[cond]
    g_p  = g_k/g_N
    
    pdf = st.beta(g_k + 1, g_N - g_k + 1)
    int_p = pdf.cdf(p) - pdf.cdf(pdf.mean())
    
    c = cl/2.

    # Positive values correspond to the left
    lc = (int_p > 0)
    lw = interp1d(int_p[lc], g_p[lc])(c)
    
    # Negative values correspond to the right
    rc = (int_p <= 0)
    up = interp1d(-1*int_p[rc], g_p[rc])(c)
    
    return p, p - lw, up - p


@deco_input_args(float, slc = [1, 2], kvars = ['cl', 'prec'])
def freq_poisson_uncert( mean, cl = config.cl, prec = config.prec, nsig = config.nsig ):
    '''
    Calculate the frequentist poisson uncertainties for a given
    integer value. The confidence level may be provided. Also the
    precision required in the outcoming uncertainties.
    '''
    s_sy = sqrt(mean)
    
    if mean != 0:
        stp = nsig*s_sy
    else:
        stp = 2.*nsig

    nsteps = int(stp/prec)
    
    pb = (1. - cl)/2.
    
    if mean != 0:
        ''' Adding the value at "mean" is necessary '''
        lw_mean_lst = np.linspace(mean - stp, mean, nsteps)
        lw_probs    = map(lambda x: rt.Math.poisson_cdf_c(mean, x) + rt.Math.poisson_pdf(mean, x),
                          lw_mean_lst)
        mean_lw = interp1d(lw_probs, lw_mean_lst)(pb)
        s_lw    = mean - mean_lw
    else:
        s_lw = 0.
    
    up_mean_lst = np.linspace(mean + stp, mean, nsteps)
    up_probs    = map(lambda x: rt.Math.poisson_cdf(mean, x), up_mean_lst)
    mean_up     = interp1d(up_probs, up_mean_lst)(pb)

    s_up = mean_up - mean
    
    return s_lw, s_up
