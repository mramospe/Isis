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


from Isis.decorators import deco_input_args
from Isis.iboost.general import sendWarningMsg
from Isis.iroot import ROOT as rt
from Isis.root_utils import hist_values

import numpy as np
import scipy.stats as st


class defaults:
    '''
    Define default values of the functions in this module
    '''
    disc = False
    cl   = st.chi2(1).cdf(1) # 1 sigma


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


@deco_input_args(float, slc = range(1), kvars = ['cl'])
def bayes_asy_poisson_uncert( n, cl = defaults.cl, disc = default.disc ):
    '''
    Return the asymmetric poisson uncertainty of having "n"
    events. If "disc" is set to true, the integer version
    will be used ("n" must be an integer), while if false,
    the gamma function is used instead.
    '''
    if disc:
        pdf = st.poisson
    else:
        pdf = st.gamma
    
    p = pdf(n)
    m = p.mean()
    
    lw, up = p.interval(cl)
    
    return m - lw, up - m


@deco_input_args(float, kvars = ['cl'])
def bayes_asy_eff( N, k, cl = defaults.cl, disc = defaults.disc ):
    '''
    Return the efficiency and asymmetric uncertainties of having
    "k" elements in "N".
    '''
    p = k/N
    
    if disc:
        pdf = st.binom(N, p)
        lw, up = np.array(pdf.interval(cl))/N
    else:
        pdf = st.beta(k + 1, N - k + 1)
        lw, up = pdf.interval(cl)
    
    m = pdf.mean()
    
    return p, p - lw, up - p


def bayes_eff( N, k, cl = defaults.cl, disc = defaults.disc ):
    '''
    Return the efficiency and the symmetric and asymmetric
    uncertainties. The values returned are float.
    '''
    eff, s_sy = bayes_sym_eff(N, k, disc = disc)
    eff, s_lw, s_up = bayes_asy_eff(N, k, disc = disc)

    return eff, s_sy, s_lw, s_up


@deco_input_args(float, slc = range(2))
def bayes_sym_eff( N, k, disc = defaults.disc ):
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

