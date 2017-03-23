#//////////////////////////////////////////////////////////
#//
#//  Python modules
#//
#// -------------------------------------------------------
#//
#//  AUTHOR: Miguel Ramos Pernas
#//  e-mail: miguel.ramos.pernas@cern.ch
#//
#//  Last update: 23/03/2017
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

from Isis.Decorators import decoInputArgs
from Isis.IBoost.PyGeneral import sendWarningMsg
from Isis.RootUtils import extractHistValues

from math import sqrt
import numpy as np
import numbers
from scipy.special import betainc
from scipy.stats import beta


@decoInputArgs(float, kvars = ['cl', 'prec'])
def binomialUncert( N, k, cl = 0.683, prec = 0.01 ):
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
            
            centers, values, widths = extractHistValues(arg)

            self.__init__(centers, values)
            
        else:
            if weights == None:
                weights = len(arg)*[1.]

            values, weights = zip(*sorted(zip(arg, weights)))

            self.Values = np.array(values)
            self.Cltve  = np.cumsum(weights)*1./sum(weights)
    
    def transform( self, arg ):
        '''
        Return the integral transformated value(s)
        '''
        return np.interp(arg, self.Values, self.Cltve)

    def deTransform( self, arg ):
        '''
        Return the de-transformated value(s)
        '''
        return np.interp(arg, self.Cltve, self.Values)


@decoInputArgs(float, slc = [], kvars = ['cl', 'prec'])
def poissonUncert( mean, cl = 0.683, prec = 0.01 ):
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
