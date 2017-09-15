#
# Auxiliar functions and methods to use together with clsMuScan.py
#
# author: Miguel Ramos Pernas
# email:  miguel.ramos.pernas@cern.ch
#

from Isis.plot_tools import draw_hists, hist, scatter_plot

import numpy as np
import iroot as rt

class CLsStepList:
    '''
    Store the result for each step of the mu-scan
    '''
    def __init__( self ):
        '''
        Values for the median, observation and 1(2)-sigma
        '''
        self.cls_med = []
        self.cls_obs = []
        self.cls_1sl = []
        self.cls_1sr = []
        self.cls_2sl = []
        self.cls_2sr = []

    def append( self, cls_med, cls_1sl, cls_1sr, cls_2sl, cls_2sr, cls_obs = None ):
        '''
        Add values for one step
        '''
        self.cls_med.append(cls_med)
        self.cls_1sl.append(cls_1sl)
        self.cls_1sr.append(cls_1sr)
        self.cls_2sl.append(cls_2sl)
        self.cls_2sr.append(cls_2sr)
        self.cls_obs.append(cls_obs)
        
        
def makeCLsPlot( mu_vals, cls_vals,
                 vmin = None, vmax = None,
                 name = 'CLsPlot', title = 'CLs plot' ):
    '''
    Generate the CLs plot
    '''
    if vmin == None:
        vmin = min(mu_vals)
    if vmax == None:
        vmax = max(mu_vals)
    
    gr = scatter_plot(mu_vals, cls_vals.cls_med, name = 'Median')
    gr.GetYaxis().SetRangeUser(0, 1)
    gr.GetXaxis().SetRangeUser(vmin, vmax)
    
    canvas = rt.TCanvas(name, title)
    gr.Draw('APC')

    canvas.Update()

    cont_1s = makeContour(mu_vals, cls_vals.cls_1sl, cls_vals.cls_1sr,
                          name = 'sigma1', title = '1 #sigma')
    cont_2s = makeContour(mu_vals, cls_vals.cls_2sl, cls_vals.cls_2sr,
                          name = 'sigma2', title = '2 #sigma')

    cl_llst = []
    gr_obs  = None
    cls_obs = cls_vals.cls_obs
    if cls_obs[0] != None:
        ''' Get lines for 90% and 95% CL '''
        
        gr_obs = scatter_plot(mu_vals, cls_obs, name = 'Observation')
        gr_obs.SetLineColor(rt.kBlack)
        
        scls_obs, smu_vals = zip(*sorted(zip(cls_obs, mu_vals)))
        
        mu_90    = np.interp(0.10, scls_obs, smu_vals)
        mu_95    = np.interp(0.05, scls_obs, smu_vals)
        hline_90 = rt.TLine(vmin , 0.10, mu_90, 0.10)
        vline_90 = rt.TLine(mu_90,   0., mu_90, 0.10)
        hline_95 = rt.TLine(vmin , 0.05, mu_95, 0.05)
        vline_95 = rt.TLine(mu_95,   0., mu_95, 0.05)

        cl_llst = [hline_90, vline_90, hline_95, vline_95]
        for l in cl_llst:
            l.SetLineColor(rt.kRed)
        for l in (hline_95, vline_95):
            l.SetLineStyle(rt.kDashed)
            
    ''' Typical CLs colors and styles '''
    gr.SetLineColor(rt.kBlue)
    gr.SetLineStyle(rt.kDashed)
    cont_2s.SetFillColor(rt.kYellow)
    cont_1s.SetFillColor(rt.kGreen)
    
    cont_2s.Draw('SAMEFL')
    cont_1s.Draw('SAMEFL')
    gr.Draw('SAMEPC')

    out = [canvas, gr, cont_1s, cont_2s]
    
    if gr_obs:
        gr_obs.Draw('SAMEPC')
        out.append(gr_obs)
        for l in cl_llst:
            l.Draw('SAME')
        out += cl_llst
    
    return out


def makeContour( arr_x, arrA_y, arrB_y, name = '', title = '' ):
    '''
    Generate contour plot for the given arrays
    '''
    arrA_y, arrA_x = zip(*zip(arrA_y, arr_x))
    arrB_y, arrB_x = zip(*reversed(zip(arrB_y, arr_x)))

    comp_x = np.array(arrA_x + arrB_x, dtype=float)
    comp_y = np.array(arrA_y + arrB_y, dtype=float)
    
    cont = rt.TGraph(len(comp_x), comp_x, comp_y)
    cont.SetNameTitle(name, title)

    return cont
    

def makeSnapShot( name, idx, mu_value, bkg_hyp, sig_hyp, tstat, nbins = 100 ):
    '''
    Plot the background and signal test statistics
    '''
    bkg = bkg_hyp.getTSVals()
    sig = sig_hyp.getTSVals()

    ltot = len(bkg) + len(sig)
    mean = sorted(np.concatenate([bkg, sig]))[ltot/2]
    vmin = min(bkg[0], sig[0])
    vmax = max(bkg[-1], sig[-1])
    rg   = (vmax - vmin)/2.

    vmin = mean - rg
    vmax = mean + rg

    hbkg = hist(bkg, nbins = nbins, vmin = vmin, vmax = vmax,
                name  = 'NullHyp_' + str(idx),
                title = 'Null hypothesis #mu = {:.2f}'.format(mu_value))
    hsig = hist(sig, nbins = nbins, vmin = vmin, vmax = vmax,
                name  = 'SigHyp_' + str(idx),
                title = 'Signal hypothesis #mu = {:.2f}'.format(mu_value))

    hbkg.SetLineColor(rt.kBlue)
    hsig.SetLineColor(rt.kRed)

    for h in (hbkg, hsig):
        h.SetLineWidth(2)

    mxline = max(hbkg.GetMaximum(), hsig.GetMaximum())
    line   = rt.TLine(tstat, 0, tstat, 1.1*mxline)
    line.SetLineColor(rt.kBlack)

    title  = 'Test statistics ( #mu = {:.2f} )'.format(mu_value)
    canvas = rt.TCanvas(name, title)
    output = draw_hists([hbkg, hsig], title = title, norm = False)
    line.Draw('SAME')
    output.append(line)
    
    canvas.Update()

    output.append(canvas)
    
    return output
