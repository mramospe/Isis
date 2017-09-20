#////////////////////////////////////////////////////////////////
#//
#//  Python modules
#//
#// -------------------------------------------------------------
#//
#//  AUTHOR: Miguel Ramos Pernas
#//  e-mail: miguel.ramos.pernas@cern.ch
#//
#//  Last update: 15/09/2017
#//
#// -------------------------------------------------------------
#//
#//  Description:
#//
#//  This module implements different tools to generate plots
#//  using Root objects.
#//
#// -------------------------------------------------------------
#////////////////////////////////////////////////////////////////


from Isis.data_management import DataMgr
from Isis.efficiencies import efficiency
from Isis.iboost.general import sendErrorMsg, sendWarningMsg
from Isis.iroot import ROOT as rt
from Isis.math_ext import nearest_square

import itertools, sys
from math import sqrt
import numpy as np
import scipy as sc


class CanvasStorer:
    '''
    Simple struct with three attributes to store a canvas and two lists:
    one with data objects (histograms, graphs, ...) and the other one
    focused adding information to the plot (legend, lines, ...)
    '''
    def __init__( self, canvas, data_objs = None, info_objs = None ):
        '''
        Constructor given the canvas and the lists of data objects and
        information objects
        '''
        self.canvas   = canvas
        self.dataObjs = data_objs or []
        self.infoObjs = info_objs or []

    def write( self, info = True ):
        '''
        Write the canvas and its objects in the current file
        '''
        self.canvas.Write()
        for obj in self.dataObjs:
            obj.Write()
        if info:
            for obj in self.infoObjs:
                obj.Write()

class FormatListIter:
    '''
    Output class for an iteration over a < FormatList > instance. This class
    stores the information of the color, line style and fill style. It also has
    features to apply it to Root objects.
    '''
    def __init__( self, color, linest, markst, fillst ):
        '''
        The color and the line, marker and fill styles are passed to the class
        '''
        self.color  = color
        self.lineSt = linest
        self.markSt = markst
        self.fillSt = fillst

    def apply( self, obj, lw = 2 ):
        '''
        Apply the format stored to a Root object. By default the line width is also
        set to < 2 >.
        '''
        obj.SetLineWidth( lw )
        obj.SetLineColor( self.color )
        obj.SetMarkerColor( self.color )
        obj.SetLineColor( self.color )
        obj.SetMarkerStyle( self.markSt )
        if self.lineSt != None:
            obj.SetLineStyle( self.lineSt )
        if self.fillSt != None:
            obj.SetFillColor( self.color )
            obj.SetFillStyle( self.fillSt )

    def styles( self ):
        '''
        Returns the members of this class in a tuple
        '''
        return tuple(el for el in (self.color, self.lineSt,
                                   self.markSt, self.fillSt)
                     if el != None )


class FormatList:
    '''
    This class allows to generate a list storing colors and line, marker and fill
    styles, so one can iterate over it. The formats are extracted using
    < __getitem__ > or on an iteration process. If the index is greater than the
    number of colors in the list, the color will correspond to the loop number,
    added to the remainder araising from the division. For the other features
    the quantity is directly extracted from the remainder.
    '''
    def __init__( self, colors = None, linest = None, markst = None, fillst = False ):
        '''
        Any of the input arguments can be passed explicitily to the class. However,
        this class stores default values for each input parameter. By default the fill
        style is not used, since Root automatically fills the histogram if it is set.
        If a list is passed to any of the input arguments, a check will be made to see
        that all of them have the same length (beware of the length of the default
        arguments). If only one value is specified, all the objects using these formats
        will have the same value of that quantity too.
        '''
        self._iter = 0
        if colors != None:
            self.colors = colors
        else:
            self.colors = [ rt.kBlue, rt.kRed, rt.kOrange,
                            rt.kGreen, rt.kMagenta, rt.kCyan ]
        if linest != None:
            self.lineSt = linest
        else:
            self.lineSt = range( 1, 7 )
        if markst != None:
            self.markSt = markst
        else:
            self.markSt = range( 20, 26 )
        if fillst != None:
            self.fillSt = fillst
        else:
            self.fillSt = range( 3000, 3006 )
            
        ''' Check that all the lists given have the same length '''
        lgths = [ len( lst ) for lst in ( self.colors, self.lineSt,
                                          self.markSt, self.fillSt )
                  if isinstance( lst, list ) ]
        if len( set( lgths ) ) != 1:
            sendErrorMsg('Lists passed to FormatList instance have different lengths')

    def __getitem__( self, idx ):
        ''' Gets the format for the given index '''
        if isinstance( self.colors, list ):
            lst = self.colors
        elif isinstance( self.lineSt, list ):
            lst = self.lineSt
        elif isinstance( self.markSt, list ):
            lst = self.markSt
        elif isinstance( self.fillSt, list ):
            lst = self.fillSt
        else:
            lst = [ 0 ]
        n     = len( lst )
        nloop = idx / n
        niter = idx % n
        col   = self._getit( self.colors, niter, nloop )
        lst   = self._getit( self.lineSt, niter )
        mst   = self._getit( self.markSt, niter )
        fst   = self._getit( self.fillSt, niter )
        return FormatListIter( col, lst, mst, fst )

    def __iter__( self ):
        ''' Definition of the iterator '''
        self._iter = 0
        return self

    def next( self ):
        '''
        Sets the new value for the iteration. In order to use this class in an
        iterative mode, another iterable object has to be the one that raises the
        exception to stop the iteration.
        '''
        frmt = self.__getitem__(self._iter)
        self._iter += 1
        return frmt

    def _getit( self, lst, idx, nloop = False ):
        '''
        Auxiliar function to get the content of the next item in a list (if any)
        '''
        if lst:
            if isinstance( lst, list ):
                return lst[ idx ] + nloop
            else:
                return lst
        else:
            return


def divide_hists( hN, hK, asym = True, name = '', title = None, xtitle = '', ytitle = '' ):
    '''
    Divide two histograms < hK/hN >. By default, asymmetric errors are
    considered. Returns a TGraphAsymmErrors instance.
    '''
    nbins   = hN.GetNbinsX()
    tprg    = (1, nbins + 1)
    centers = np.array([hN.GetBinCenter(i) for i in xrange(*tprg)], dtype = float)
    swidth  = np.array([hN.GetBinWidth(i) for i in xrange(*tprg)], dtype = float)
    
    nN = [hN.GetBinContent(i) for i in xrange(*tprg)]
    nK = [hK.GetBinContent(i) for i in xrange(*tprg)]
    
    eff     = np.zeros(nbins, dtype = float)
    seff    = np.zeros(nbins, dtype = float)
    seff_lw = np.zeros(nbins, dtype = float)
    seff_up = np.zeros(nbins, dtype = float)

    for nn, nk in zip(nN, nK):
        p, s_sy, s_lw, s_up = efficiency(nn, nk)
        eff[i]     = p
        seff[i]    = s_sy
        seff_lw[i] = s_lw
        seff_up[i] = s_up
        
    if not asym:
        ''' If specified, the symmetric error is used '''
        seff_lw = seff_up = seff

    '''
    Build the graph
    '''
    graph = rt.TGraphAsymmErrors(nbins,
                                 centers, eff,
                                 swidth, swidth,
                                 seff_lw, seff_up)
    
    format_plottable_2d(graph, name, title, xtitle, ytitle)
    
    return graph


def draw_hists( hlst, drawopt = '', norm = True, title = 'List of histograms' ):
    '''
    Draws the given list of histograms. If the variable < norm > is set to True,
    then the histograms will be normalized. It returns the histogram used to give
    format to the plot, and the list of input histograms or the normalized clones.
    The draw options are set using the < drawopt > keyword. The titles of the axes
    are taken from the first histogram in the list.
    '''
    if norm:
        meth = rt.TH1.DrawNormalized
    else:
        meth = rt.TH1.Draw
    
    hformat = hist_format(hlst, title = hlst[0].GetTitle(), norm = norm)
    hformat.Draw()
    
    drawopt += 'SAME'
    
    outhlst = [hformat]
    
    for h in hlst:

        hdr = meth( h, drawopt )

        if hdr:
            hdr.SetDirectory(0)
            outhlst.append(hdr)
    
    if len(outhlst) == 1:
        outhlst += hlst
    
    return outhlst


def format_plottable_2d( obj, name = '', title = None, xtitle = '', ytitle = '' ):
    '''
    Set name, main title and titles for each axis of a 2D object
    '''
    title = title or name

    obj.SetNameTitle(name, title)

    if xtitle:
        obj.GetXaxis().SetTitle(xtitle)
    if ytitle:
        obj.GetYaxis().SetTitle(ytitle)


def hist_bounds( arr, vmin = None, vmax = None ):
    '''
    Extract the histogram bounds given a list of values and the possible
    bounds (if any). If the list is empty, bounds at (0, 1) are returned.
    '''
    if vmin:
        arr = np.append(arr, vmin)
    if vmax:
        arr = np.append(arr, vmax)
    
    if len(arr):
        return arr.min(), arr.max()
    
    return 0, 1


def hist_format( hlst, name = '', title = None, norm = True ):
    '''
    Return the histogram with the format (number of bins and bounds) to define the
    axis of a figure with all being plotted together
    '''
    title = title if title else name
    
    xmin = np.fromiter((h.GetXaxis().GetXmin() for h in hlst), float).min()
    xmax = np.fromiter((h.GetXaxis().GetXmax() for h in hlst), float).max()
    
    ymin = np.fromiter((h.GetMinimum() for h in hlst), float).min()
    ymax = np.fromiter((h.GetMaximum() for h in hlst), float).max()
    if norm:
        wgts = np.fromiter((h.GetSumOfWeights() for h in hlst), float)
        ymin /= wgts
        ymax /= wgts

    ymin = min(ymin)
    ymax = max(ymax)
    ymax += 0.1*(ymax - ymin)
    
    hform = hlst[0].__class__(name, title, hlst[0].GetNbinsX(), xmin, xmax)
    
    hform.GetXaxis().SetTitle(hlst[0].GetXaxis().GetTitle())
    hform.GetYaxis().SetTitle(hlst[0].GetYaxis().GetTitle())

    hform.SetLineColor(0)
    hform.SetLineStyle(0)
    
    hform.GetYaxis().SetRangeUser(ymin, ymax)

    return hform


def hist_points( arr, nbins, vmin = None, vmax = None ):
    '''
    This function extracts the indexes of the given array of data which are
    supposed to be used to make a histogram
    '''
    if len(arr) == 0:
        return []
    if vmin == None:
        vmin = arr.min()
    if vmax == None:
        vmax = arr.max()
        vmax += (vmax - vmin)/(2.*nbins)
    return np.logical_and(arr >= vmin, arr < vmax)


def hist_ctor_from_type( tp, dim = 1 ):
    '''
    Returns the histogram constructor given the type as a string
    '''
    if tp not in ( 'float', 'double', 'int' ):
        sendErrorMsg('Histogram type < %s > not known' %tp)
        return
    if dim == 1:
        if tp == 'float':
            return rt.TH1F
        elif tp == 'double':
            return rt.TH1D
        else:
            return rt.TH1I
    elif dim == 2:
        if tp == 'float':
            return rt.TH2F
        elif tp == 'double':
            return rt.TH2D
        else:
            return rt.TH2I
    else:
        sendErrorMsg('Histogram dimension < %i >, not allowed' %dim)


def adbin_hist( name, minocc, values,
                htype   = 'double',
                title   = None,
                weights = False,
                xtitle  = '',
                ytitle  = '' ):
    '''
    This function creates a 1-D adaptive binning histogram given a name, the
    minimum occupancy value and a list. Adding a list of weights is also possible.
    '''
    histcall = hist_ctor_from_type(htype, 1)
    
    ''' Calculates the array of weights '''
    length = len(values)
    if weights:
        sw    = float(sum(weights))
        nbins = int(sw)/minocc
    else:
        weights = length*[1.]
        sw      = float(length)
        nbins   = length/minocc
    
    idxs = hist_points(values, nbins)

    vmin, vmax = hist_bounds(values[idxs])
    
    ''' If the occupancy requested is too big, an error message is displayed '''
    if nbins == 0:
        sendErrorMsg('Occupancy requested is too big: %i' %iminocc)
    
    '''
    Creates a list with the values and the weights joint and sorts it by the values
    '''
    values = zip(values, weights)
    values.sort()
    
    ''' Fills the bins with the data '''
    binlist = tuple([vmax, 0] for i in xrange(nbins))
    idat, swpb = 0, 0
    for idf, ib in enumerate(binlist):
        swpb = sw/(nbins - idf)
        while ib[1] < swpb and idat < length:
            val, wgt = values[idat]
            if val < ib[0]:
                ib[0] = val
            ib[1] += wgt
            idat += 1
        sw -= ib[1]
    while idat < length:
        binlist[-1][0], binlist[-1][1] = values[idat]
        idat += 1

    '''
    To create the Root histogram, an array of doubles has to be created, with the
    minimum value for the bins
    '''
    bins = np.array((nbins + 1)*[0.], dtype = float)
    for i, ib in enumerate(binlist):
        bins[i] = ib[0]
    bins[-1] = vmax
    
    hist = histcall('', '', nbins, bins)

    format_plottable_2d(hist, name, title, xtitle, ytitle)
    
    return hist


def corr_hist( matrix, name = '', title = None, vartitles = None ):
    '''
    Creates a correlation histogram given a list of lists. By default it is drawn
    in color, without palette, and with the contents written inside each bin. No
    statistical box is displayed neither.
    '''
    lm = len(matrix)
    
    vartitles = vartitles or []
    if vartitles != []:
        if lm != len(vartitles):
            sendErrorMsg('Number of titles is not the same as that of the matrix')
    else:
        vartitles = ['Variable_' + str(i) for i in xrange(lm)]

    corr_matrix = 100*np.corrcoef(matrix)
    
    hist = rt.TH2D('', '', lm, 0, lm, lm, 0, lm)
    for i, row in enumerate(corr_matrix):
        for j, el in enumerate(row):
            hist.SetBinContent(i + 1, j + 1, int(el))
    for i, tit in enumerate(vartitles):
        hist.GetXaxis().SetBinLabel(i + 1, tit)
        hist.GetYaxis().SetBinLabel(i + 1, tit)

    format_plottable_2d(hist, name, title, '', '')
    
    hist.GetXaxis().SetTickLength(0)
    hist.GetYaxis().SetTickLength(0)
    hist.SetOption('COLTEXT')
    hist.SetStats(False)
    
    return hist


def cumulative( hist, name = '', norm = False, title = None ):
    '''
    Returns a histogram containing the cumulative distribution of that given. If
    the option < norm > is given, the histogram will be scaled in such a way that
    the maximum value will be one.
    '''
    chist = hist.Clone()
    cumulative = chist.GetBinContent(1)
    for i in xrange(2, hist.GetNbinsX() + 1):
        cumulative += hist.GetBinContent(i)
        chist.SetBinContent(i, cumulative)
        chist.SetBinError(i, sqrt(cumulative))
    if norm:
        chist.Scale(1./chist.GetMaximum())
    format_plottable_2d(hist, name, title)
    return chist


def hist( var,
          name   = '',
          nbins  = 100,
          htype  = 'double',
          title  = None,
          vmin   = None,
          vmax   = None,
          wvar   = False,
          xtitle = '',
          ytitle = '' ):
    '''
    Function to generate a Root histogram given a list. By default no y-title is
    drawn, but it can be set with the < ytitle > option. For values of type int,
    the histogram will be of type double.
    '''
    histcall = hist_ctor_from_type(htype, 1)

    idxs = hist_points(var, nbins, vmin = vmin, vmax = vmax)
    
    vmin, vmax = hist_bounds(var[idxs], vmin = vmin, vmax = vmax)
    
    hist = histcall('', '', nbins, vmin, vmax)
    
    if wvar:
        for el, w in zip(var, wvar):
            hist.Fill(el, w)
    else:
        for el in var:
            hist.Fill(el)
    
    format_plottable_2d(hist, name, title, xtitle, ytitle)
            
    return hist


def hist2d( xvar, yvar,
            name   = '',
            htype  = 'double',
            title  = None,
            wvar   = False,
            xbins  = 100,
            xmax   = None,
            xmin   = None,
            xtitle = '',
            ybins  = 100,
            ymax   = None,
            ymin   = None,
            ytitle = '' ):
    '''
    Creates a 2-dimensional histogram given two lists
    '''
    histcall = hist_ctor_from_type(htype, 2)
    
    xidxs = hist_points(xvar, xbins, vmin = xmin, vmax = xmax)
    yidxs = hist_points(yvar, ybins, vmin = ymin, vmax = ymax)

    ''' The values used are the intersection between the two lists '''
    idxs = set( xidxs ) & set( yidxs )

    xmin, xmax = hist_bounds(xvar[idxs], vmin = xmin, vmax = xmax)
    ymin, ymax = hist_bounds(yvar[idxs], vmin = ymin, vmax = ymax)
    
    hist = histcall('', '', xbins, xmin, xmax, ybins, ymin, ymax)

    if wvar:
        for x, y, w in zip(xvar, yvar, wvar):
            hist.Fill( x, y, w )
    else:
        for x, y in zip(xvar, yvar):
            hist.Fill(x, y)

    format_plottable_2d(hist, name, title, xtitle, ytitle)

    return hist


def kstest( mgrA, mgrB, variables,
            vartitles = [], usecl = True, name = '', title = '' ):
    '''
    Perform Kolmogorov-Smirnov tests for the two given DataMgr objects and variables.
    If "usecl" is set to True, the confidence level of exclusion that the two
    samples follow the same distribution is given instead of the p-value. Returns
    a dictionary with the values of the test statistic and p-value for each variable,
    together with the filled histograms.
    '''
    results = {var: sc.stats.ks_2samp(mgrA[var], mgrB[var])
               for var in variables}
    
    lm = len(variables)
    if vartitles != []:
        if lm != len(vartitles):
            sendErrorMsg('Number of titles is not the same as the number of variables')
    else:
        vartitles = variables
        
    h_tstat = rt.TH1D('', '', lm, 0, lm)
    h_pval  = rt.TH1D('', '', lm, 0, lm)
    
    for i, (v, t) in enumerate(zip(variables, vartitles)):

        tstat, pval = results[v]

        if usecl:
            pval = (1. - pval)*100.
            
        h_tstat.SetBinContent(i + 1, tstat)
        h_pval.SetBinContent(i + 1, pval)
        
        for h in (h_tstat, h_pval):
            h.GetXaxis().SetBinLabel(i + 1, t)

        if usecl:
            h_pval.GetYaxis().SetRangeUser(0, 110)
        else:
            h_pval.GetYaxis().SetRangeUser(0, 1.1)

        h_tstat.GetYaxis().SetRangeUser(0, 1.1)
            
    return results, h_tstat, h_pval


class _GraphInConfig:
    '''
    Auxiliar class to build a TGraph, TGraphErrors or TGraphAsymmErrors objects
    depending on the input arguments
    '''
    def __init__( self, values, err, errlo, errup ):
        '''
        Values, symmetric and asymmetric errors must be provided
        '''
        self.values = np.array(values, dtype = float)

        self.err    = any((el is not False) for el in (err, errlo, errup))
        self.sym    = None
        self.errLo  = None
        self.errUp  = None
        self.errors = None
        
        if self.err:

            if err:
                ''' Symmetric errors '''
                
                self.sym = True
                
                if errlo or errup:
                    sendWarningMsg('Specified both sym. and asym. errors; only sym. '\
                                   'will be considered')
                
                self.errors = np.array(err, dtype = float)
                
                self._build_asym()
                
            else:
                ''' Asymmetric errors '''
                
                self.sym = False

                if errlo:
                    self.errLo = np.array(errlo, dtype = float)
                else:
                    self.errLo = np.zeros(len(errup), dtype = float)

                if errup:
                    self.errUp = np.array(errup, dtype = float)
                else:
                    self.errUp = np.zeros(len(errlo), dtype = float)

                self.errors = np.zeros(len(self.errLo), dtype = float)

        else:
            ''' If no errors are specified, they are considered symmetric '''
            
            self.sym    = True
            self.err    = False
            self.errors = np.zeros(len(values), dtype = float)
            
            self._build_asym()

    def _build_asym( self ):
        '''
        Sets the lower and upper errors to the symmetric errors
        '''
        self.errLo = self.errUp = self.errors


def multiplot( mgrs, variables,
                cuts   = False,
                errors = False,
                flist  = None,
                legend = True,
                name   = 'canvas',
                nbins  = 100,
                norm   = True,
                ranges = None,
                title  = None ):
    '''
    This function plots in the same canvas the distributions of the given
    variables from different "DataMgr" classes. Different options can also
    been provided to modify the canvas and the information displayed. If
    < ranges > is provided, it must contain the same name of the variables
    passed in < variables > (this applies also to formulas).
    '''
    ranges = ranges or {}
    flist  = flist or FormatList()
    title  = title or name
    
    nvars = len(variables) + 1

    if all(var in mgr for mgr in mgrs for var in variables):
        
        ''' Generates and divides the canvas '''
        nyvars, nxvars = opt_canvas_div( nvars )
        canvas = rt.TCanvas(name, title, 300*nyvars, 300*nxvars)
        canvas.Divide(nyvars, nxvars)

        canvas_info = CanvasStorer(canvas)
        
        nmgrs = len(mgrs)
        ''' If cuts are specified it calculates the true managers '''
        if cuts:
            for i, mgr in enumerate(mgrs):
                mgrs[i] = mgr.subsample(cuts = cuts, name = mgr.name)
                
        ''' Disables the stat box of the histograms '''
        rt.gStyle.SetOptStat( 0 )
        
        ''' Constructs the legend and the information panel if specified '''
        if legend:
            
            pave_dim  = (0.1, 0.8 - nmgrs*0.05, 0.9, 0.9)
            text_size = 0.075
            
            rlegend = rt.TLegend(*pave_dim)
            rlegend.SetHeader( '#bf{-- Legend --}' )
            rlegend.SetTextAlign( 22 )
            rlegend.SetTextSize(text_size)
            rlegend.SetFillColor( 15 )
            
            rtxtinf = rt.TPaveText(*pave_dim)
            rtxtinf.AddText( '-- Number of entries --' )
            rtxtinf.SetTextSize(text_size)
            rtxtinf.SetFillColor( 42 )
            rtxtinf.SetShadowColor( 0 )

            canvas_info.infoObjs += [rlegend, rtxtinf]
        
        ''' Generates and draws the histograms '''
        for iv, var in enumerate(variables):
            
            canvas.cd(iv + 1)
            
            ''' This is done to reduce disk usage '''
            tot = np.array([m.subsample(columns = [var],
                                        cuts = cuts).values.T[0]
                            for m in mgrs])
            
            ''' Extract the ranges for each variable (if any) '''
            if var in ranges.keys():
                rnbins, vmin, vmax = ranges[var]
            else:
                if any(len(m) != 0 for m in tot):
                    vmax = tot.max()
                    vmin = tot.min()
                else:
                    vmin = 0
                    vmax = 1
                rnbins = nbins

            entries = []
            hists   = []
            for im, (mgr, vals) in enumerate(zip(mgrs, tot)):
                hname = mgr.name + '_' + var
                h = hist( vals,
                                   name  = hname,
                                   title = var,
                                   nbins = rnbins,
                                   vmin  = vmin,
                                   vmax  = vmax )
                
                entries.append(h.GetEntries())
                hists.append( h )
                
                flist[ im ].apply( h )

                h.GetXaxis().SetTitle(var)
                
            ''' Draw histograms, with error bars if specified '''
            hists = draw_hists(hists, drawopt = errors*'E', norm = norm, title = '')

            if legend and iv == 0:
                ''' In the first iteration add the entries to the legend '''
                for mgr, h, sw in zip(mgrs, hists[1:], entries):
                    rlegend.AddEntry( h, '#bf{' + mgr.name + '}', 'L' )
                    rtxtinf.AddText( mgr.name + ': %i' % sw )
                    
            canvas_info.dataObjs += hists
            
        if legend:
            pad = canvas.cd( nvars )
            pad.Divide( 2, 1 )
            pad.cd( 1 ); rlegend.Draw()
            pad.cd( 2 ); rtxtinf.Draw()
            
        canvas.Update()
        
        return canvas_info
    else:
        sendErrorMsg('Some managers do not have access to some of the variables')
        return


def opt_canvas_div( nvars ):
    '''
    Create the optimal canvas division for a given number of pads
    '''
    nstsq = int(sqrt(nearest_square(nvars)))
    
    if nstsq**2 > nvars:
        nxvars = nstsq
        nyvars = nstsq
    else:
        nxvars = nstsq
        nyvars = nstsq
        while nxvars*nyvars < nvars:
            nyvars += 1
        
    return nyvars, nxvars


def scatter_plot( xvar, yvar,
                  xerr   = False,
                  xerrlo = False,
                  xerrup = False,
                  yerr   = False,
                  yerrlo = False,
                  yerrup = False,
                  name   = '',
                  title  = None,
                  xtitle = '',
                  ytitle = '' ):
    '''
    Generates a scatter plot given two lists of data
    '''

    xconfig = _GraphInConfig(xvar, xerr, xerrlo, xerrup)
    yconfig = _GraphInConfig(yvar, yerr, yerrlo, yerrup)

    npoints = len(xconfig.values)
    xvar    = xconfig.values
    yvar    = yconfig.values
    
    if not xconfig.err and not yconfig.err:
        
        graph = rt.TGraph(npoints, xvar, yvar)
        
    else:
        
        if xconfig.sym and yconfig.sym:

            xerr = xconfig.errors
            yerr = yconfig.errors
            
            graph = rt.TGraphErrors(npoints, xvar, yvar, xerr, yerr)

        else:
            
            xerrlo, xerrup = xconfig.errLo, xconfig.errUp
            yerrlo, yerrup = yconfig.errLo, yconfig.errUp

            graph = rt.TGraphAsymmErrors(npoints, xvar, yvar, xerrlo, xerrup, yerrlo, yerrup)
            
    format_plottable_2d(graph, name, title, xtitle, ytitle)
    
    return graph


def superimposed_pads( canvas ):
    '''
    Add two pads to the given canvas, both being transparent
    '''
    pad1 = rt.TPad('pad1', '', 0, 0, 1, 1)
    pad2 = rt.TPad('pad2', '', 0, 0, 1, 1)

    for pad in (pad1, pad2):
        canvas.cd()
        pad.SetLineStyle(0)
        pad.SetFillStyle(4000)
        pad.SetFillColor(0)
        pad.SetFrameFillStyle(4000)
        pad.Draw()

    canvas.cd()
        
    return pad1, pad2

