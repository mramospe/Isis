#////////////////////////////////////////////////////////////////
#//
#//  Python modules
#//
#// -------------------------------------------------------------
#//
#//  AUTHOR: Miguel Ramos Pernas
#//  e-mail: miguel.ramos.pernas@cern.ch
#//
#//  Last update: 23/03/2017
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


from Isis.Efficiencies import calcEfficiency
from Isis.IBoost.PyGeneral import sendErrorMsg, sendWarningMsg
from Isis.MathExt import nearestSquare
from Isis.Utils import formatEvalExpr

import ROOT as rt

import itertools
from math import sqrt
import numpy as np
import sys


class CanvasStorer:
    '''
    Simple struct with three attributes to store a canvas and two lists:
    one with data objects (histograms, graphs, ...) and the other one
    focused adding information to the plot (legend, lines, ...)
    '''
    def __init__( self, canvas = None, data_objs = [], info_objs = [] ):
        '''
        Constructor given the canvas and the lists of data objects and
        information objects
        '''
        self.Canvas   = canvas
        self.DataObjs = data_objs
        self.InfoObjs = info_objs


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
        self.Color  = color
        self.LineSt = linest
        self.MarkSt = markst
        self.FillSt = fillst

    def getStyles( self ):
        '''
        Returns the members of this class in a tuple
        '''
        return tuple( el for el in ( self.Color, self.LineSt, self.MarkSt, self.FillSt )
                      if el != None )
        
    def applyFormat( self, obj, lw = 2 ):
        '''
        Apply the format stored to a Root object. By default the line width is also
        set to < 2 >.
        '''
        obj.SetLineWidth( lw )
        obj.SetLineColor( self.Color )
        obj.SetMarkerColor( self.Color )
        obj.SetLineColor( self.Color )
        obj.SetMarkerStyle( self.MarkSt )
        if self.LineSt != None:
            obj.SetLineStyle( self.LineSt )
        if self.FillSt != None:
            obj.SetFillColor( self.Color )
            obj.SetFillStyle( self.FillSt )


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
        self.Iter = 0
        if colors != None:
            self.Colors = colors
        else:
            self.Colors = [ rt.kBlue, rt.kRed, rt.kOrange,
                            rt.kGreen, rt.kMagenta, rt.kCyan ]
        if linest != None:
            self.LineSt = linest
        else:
            self.LineSt = range( 1, 7 )
        if markst != None:
            self.MarkSt = markst
        else:
            self.MarkSt = range( 20, 26 )
        if fillst != None:
            self.FillSt = fillst
        else:
            self.FillSt = range( 3000, 3006 )
            
        ''' Check that all the lists given have the same length '''
        lgths = [ len( lst ) for lst in ( self.Colors, self.LineSt,
                                          self.MarkSt, self.FillSt )
                  if isinstance( lst, list ) ]
        if len( set( lgths ) ) != 1:
            sendErrorMsg('Lists passed to FormatList instance have different lengths')

    def __getitem__( self, idx ):
        ''' Gets the format for the given index '''
        if isinstance( self.Colors, list ):
            lst = self.Colors
        elif isinstance( self.LineSt, list ):
            lst = self.LineSt
        elif isinstance( self.MarkSt, list ):
            lst = self.MarkSt
        elif isinstance( self.FillSt, list ):
            lst = self.FillSt
        else:
            lst = [ 0 ]
        n     = len( lst )
        nloop = idx / n
        niter = idx % n
        col   = self._getit( self.Colors, niter, nloop )
        lst   = self._getit( self.LineSt, niter )
        mst   = self._getit( self.MarkSt, niter )
        fst   = self._getit( self.FillSt, niter )
        return FormatListIter( col, lst, mst, fst )

    def __iter__( self ):
        ''' Definition of the iterator '''
        self.Iter = 0
        return self

    def next( self ):
        '''
        Sets the new value for the iteration. In order to use this class in an
        iterative mode, another iterable object has to be the one that raises the
        exception to stop the iteration.
        '''
        frmt = self.__getitem__( self.Iter )
        self.Iter += 1
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


def divideHistograms( hN, hK, asym = True, name = '', title = None, xtitle = '', ytitle = '' ):
    '''
    Divide two histograms < hK/hN >. By default, asymmetric errors are
    considered. Returns a TGraphAsymmErrors instance.
    '''
    nbins   = hN.GetNbinsX()
    tprg    = (1, nbins + 1)
    centers = [hN.GetBinCenter(i) for i in xrange(*tprg)]
    swidth  = [hN.GetBinWidth(i) for i in xrange(*tprg)]
    
    nN = [hN.GetBinContent(i) for i in xrange(*tprg)]
    nK = [hK.GetBinContent(i) for i in xrange(*tprg)]

    eff     = []
    seff    = []
    seff_lw = []
    seff_up = []

    for nn, nk in zip(nN, nK):
        p, s_sy, s_lw, s_up = calcEfficiency(nn, nk)
        eff.append(p)
        seff.append(s_sy)
        seff_lw.append(s_lw)
        seff_up.append(s_up)

    if not asym:
        ''' If specified, the symmetric error is used '''
        seff_lw = seff_up = seff

    '''
    Build the graph
    '''
    centers = np.array(centers, dtype = float)
    eff     = np.array(eff    , dtype = float)
    swidth  = np.array(swidth , dtype = float)
    seff_lw = np.array(seff_lw, dtype = float)
    seff_up = np.array(seff_up, dtype = float)
    
    graph = rt.TGraphAsymmErrors(nbins,
                                 centers, eff,
                                 swidth, swidth,
                                 seff_lw, seff_up)
    
    formatPlottable2D(graph, name, title, xtitle, ytitle)
    
    return graph


def drawHistograms( hlst, drawopt = '', norm = True, title = 'List of histograms' ):
    '''
    Draws the given list of histograms. If the variable < norm > is set to True,
    then the histograms will be normalized. It returns the histogram used to give
    format to the plot, and the list of input histograms or the normalized clones.
    The draw options are set using the < drawopt > keyword. The titles of the axes
    are taken from the first histogram in the list.
    '''
    if norm:
        imax = max( h.GetMaximum()*1./h.GetSumOfWeights() for h in hlst )
        imin = min( h.GetMinimum()*1./h.GetSumOfWeights() for h in hlst )
        meth = rt.TH1.DrawNormalized
    else:
        imax = max( h.GetMaximum() for h in hlst )
        imin = min( h.GetMinimum() for h in hlst )
        meth = rt.TH1.Draw
    
    offset  = ( imax + imin )/10.
    vmin    = min( h.GetXaxis().GetXmin() for h in hlst )
    vmax    = max( h.GetXaxis().GetXmax() for h in hlst )
    hformat = hlst[0].__class__( '', title, 1, vmin, vmax )

    for get_ax in (rt.TH1.GetXaxis, rt.TH1.GetYaxis):
        tit = get_ax(hlst[0]).GetTitle()
        get_ax(hformat).SetTitle(tit)
    
    hformat.SetBinContent( 1, imin )
    hformat.GetYaxis().SetRangeUser( imin, imax + offset )
    hformat.Draw()
    
    drawopt += 'SAME'
    
    outhlst = [ hformat ]

    for h in hlst:

        hdr = meth( h, drawopt )

        if hdr:
            outhlst.append(hdr)
    
    if len(outhlst) == 1:
        outhlst += hlst
    
    return outhlst


def extractHistPoints( varlst, nbins, vmin = None, vmax = None ):
    '''
    This function extracts the points of the given array of data which are
    supposed to be used to make a histogram
    '''
    if vmin == None:
        vmin = min( varlst )
    if vmax == None:
        vmax  = max( varlst )
        vmax += ( vmax - vmin )/( 2.*nbins )
    return [ i for i, v in enumerate( varlst ) if v >= vmin and v < vmax ]


def formatPlottable2D( obj, name = '', title = None, xtitle = '', ytitle = '' ):
    '''
    Set name, main title and titles for each axis of a 2D object
    '''
    if title == None:
        title = name

    obj.SetNameTitle(name, title)

    if xtitle:
        obj.GetXaxis().SetTitle(xtitle)
    if ytitle:
        obj.GetYaxis().SetTitle(ytitle)


def histFromType( tp, dim = 1 ):
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


def makeAdaptiveBinnedHist( name, minocc, values,
                            htype   = 'double',
                            title   = None,
                            weights = False,
                            xtitle  = '',
                            ytitle  = '' ):
    '''
    This function creates a 1-D adaptive binning histogram given a name, the
    minimum occupancy value and a list. Adding a list of weights is also possible.
    '''
    histcall = histFromType( htype, 1 )
    
    ''' Calculates the array of weights '''
    length = len( values )
    if weights:
        sw    = float( sum( weights ) )
        nbins = int( sw )/minocc
    else:
        weights = length*[ 1. ]
        sw      = float( length )
        nbins   = length/minocc
    
    idxs   = extractHistPoints( values, nbins )
    values = [ values[ i ] for i in idxs ]
    vmax   = max( values )
    
    ''' If the occupancy requested is too big, an error message is displayed '''
    if nbins == 0:
        sendErrorMsg('Occupancy requested is too big: %i' %iminocc)
    
    '''
    Creates a list with the values and the weights joint and sorts it by the values
    '''
    values = zip( values, weights )
    values.sort()
    
    ''' Fills the bins with the data '''
    binlist = tuple( [ vmax, 0 ] for i in xrange( nbins ) )
    idat, swpb = 0, 0
    for idf, ib in enumerate( binlist ):
        swpb = sw/( nbins - idf )
        while ib[ 1 ] < swpb and idat < length:
            val, wgt = values[ idat ]
            if val < ib[ 0 ]:
                ib[ 0 ] = val
            ib[ 1 ] += wgt
            idat += 1
        sw -= ib[ 1 ]
    while idat < length:
        binlist[ -1 ][ 0 ], binlist[ -1 ][ 1 ] = values[ idat ]
        idat += 1

    '''
    To create the Root histogram, an array of doubles has to be created, with the
    minimum value for the bins
    '''
    bins = np.array(( nbins + 1 )*[ 0. ], dtype = float)
    for i, ib in enumerate( binlist ):
        bins[ i ] = ib[ 0 ]
    bins[ -1 ] = vmax
    
    hist = histcall('', '', nbins, bins)

    formatPlottable2D(hist, name, title, xtitle, ytitle)
    
    return hist


def makeCorrelationHist( matrix, name = '', title = None, vartitles = [] ):
    '''
    Creates a correlation histogram given a list of lists. By default it is drawn
    in color, without palette, and with the contents written inside each bin. No
    statistical box is displayed neither.
    '''
    lm = len( matrix )
    lv = len( vartitles )
    if vartitles != []:
        if lm != lv:
            sendWarningMsg('Number of titles is not the same as that of the matrix; '\
                           'new variables will have names < Variables_# >')
            for i in xrange( lv + 1, lm ):
                vartitles.append( 'Variable_' + str( i ) )
    else:
        vartitles = [ 'Variable_' + str( i ) for i in xrange( lm ) ]

    corr_matrix = 100*np.corrcoef( matrix )
    
    hist = rt.TH2D('', '', lm, 0, lm, lm, 0, lm)
    for i, row in enumerate( corr_matrix ):
        for j, el in enumerate( row ):
            hist.SetBinContent( i + 1, j + 1, int( el ) )
    for i, tit in enumerate( vartitles ):
        hist.GetXaxis().SetBinLabel( i + 1, tit )
        hist.GetYaxis().SetBinLabel( i + 1, tit )

    formatPlottable2D(hist, name, title, '', '')
    
    hist.GetXaxis().SetTickLength( 0 )
    hist.GetYaxis().SetTickLength( 0 )
    hist.SetOption( 'COLTEXT' )
    hist.SetStats( False )
    
    return hist


def makeCumulative( hist, name = '', norm = False, title = None ):
    '''
    Returns a histogram containing the cumulative distribution of that given. If
    the option < norm > is given, the histogram will be scaled in such a way that
    the maximum value will be one.
    '''
    chist = hist.Clone()
    cumulative = chist.GetBinContent( 1 )
    for i in xrange( 2, hist.GetNbinsX() + 1 ):
        cumulative += hist.GetBinContent( i )
        chist.SetBinContent( i, cumulative )
        chist.SetBinError( i, sqrt( cumulative ) )
    if norm:
        chist.Scale( 1./chist.GetMaximum() )
    formatPlottable2D(hist, name, title)
    return chist


def makeHistogram( var,
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
    histcall = histFromType( htype, 1 )

    idxs  = extractHistPoints( var, nbins, vmin = vmin, vmax = vmax )
    varin = [ var[ i ] for i in idxs ]
    vmin  = min( v for v in varin + [ vmin ] if v != None )
    vmax  = max( v for v in varin + [ vmax ] if v != None )
    
    hist = histcall('', '', nbins, vmin, vmax)
    
    if wvar:
        for el, w in zip( var, wvar ):
            hist.Fill( el, w )
    else:
        for el in var:
            hist.Fill( el )
    
    formatPlottable2D(hist, name, title, xtitle, ytitle)
            
    return hist


def makeHistogram2D( xvar, yvar,
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
    histcall = histFromType( htype, 2 )
    
    xidxs = extractHistPoints( xvar, xbins, vmin = xmin, vmax = xmax )
    yidxs = extractHistPoints( yvar, ybins, vmin = ymin, vmax = ymax )

    ''' The values used are the intersection between the two lists '''
    idxs = set( xidxs ) & set( yidxs )

    xvarin = [ xvar[ i ] for i in idxs ]
    xmin   = min( v for v in xvarin + [ xmin ] if v != None )
    xmax   = max( v for v in xvarin + [ xmax ] if v != None )

    yvarin = [ yvar[ i ] for i in idxs ]
    ymin   = min( v for v in yvarin + [ ymin ] if v != None )
    ymax   = max( v for v in yvarin + [ ymax ] if v != None )
    
    hist = histcall('', '', xbins, xmin, xmax, ybins, ymin, ymax)

    if wvar:
        for x, y, w in zip( xvar, yvar, wvar ):
            hist.Fill( x, y, w )
    else:
        for x, y in zip( xvar, yvar ):
            hist.Fill( x, y )

    formatPlottable2D(hist, name, title, xtitle, ytitle)

    return hist


class _GraphInConfig:
    '''
    Auxiliar class to build a TGraph, TGraphErrors or TGraphAsymmErrors objects
    depending on the input arguments
    '''
    def __init__( self, values, err, errlo, errup ):
        '''
        Values, symmetric and asymmetric errors must be provided
        '''
        self.Values = np.array(values, dtype = float)

        self.Err = False
        self.Sym = None
        
        if any((el is not False) for el in (err, errlo, errup)):

            self.Err = True
            
            if err:

                self.Sym = True
                
                if errlo or errup:
                    sendWarningMsg('Specified both sym. and asym. errors; only sym. '\
                                   'will be considered')
                
                self.Errors = np.array(err, dtype = float)

            else:

                self.Sym = False

                if errlo:
                    self.ErrLo = np.array(errlo, dtype = float)
                else:
                    self.ErrLo = np.zeros(len(errup), dtype = float)

                if errup:
                    self.ErrUp = np.array(errup, dtype = float)
                else:
                    self.ErrUp = np.zeros(len(errlo), dtype = float)

                self.Errors = np.zeros(len(self.ErrLo), dtype = float)
                    

    def buildAsym( self ):
        '''
        If this class has symmetric errors attached, generates a duplicate
        '''
        if self.Sym:
            self.ErrLo = self.ErrUp = self.Errors


def makeScatterPlot( xvar, yvar,
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

    npoints = len(xconfig.Values)
    xvar    = xconfig.Values
    yvar    = yconfig.Values
    
    if not xconfig.Err and not yconfig.Err:
        
        graph = rt.TGraph(npoints, xvar, yvar)
        
    else:
        
        if xconfig.Sym and yconfig.Sym:

            xerr = xconfig.Errors
            yerr = yconfig.Errors
            
            graph = rt.TGraphErrors(npoints, xvar, yvar, xerr, yerr)

        else:
            
            for cfg in (xconfig, yconfig):
                cfg.BuildAsym()

            xerrlo, xerrup = xconfig.ErrLo, xconfig.ErrUp
            yerrlo, yerrup = yconfig.ErrLo, yconfig.ErrUp

            graph = rt.TGraphAsymmErrors(npoints, xvar, yvar, xerrlo, xerrup, yerrlo, yerrup)
            
    formatPlottable2D(graph, name, title, xtitle, ytitle)
    
    return graph


def multiPlot( mgrs, variables,
               cuts   = False,
               errors = False,
               flist  = FormatList(),
               legend = True,
               name   = 'canvas',
               nbins  = 100,
               norm   = True,
               ranges = {},
               title  = None ):
    '''
    This function plots in the same canvas the distributions of the given
    variables from different "DataMgr" classes. Different options can also
    been provided to modify the canvas and the information displayed. If
    < ranges > is provided, it must contain the same name of the variables
    passed in < variables > (this applies also to formulas).
    '''
    if title == None:
        title = name
    
    nvars = len( variables ) + 1

    ''' Get the true variables associated with the given expressions '''
    truevars = [ formatEvalExpr( var )[ 1 ] for var in variables ]
    truevars = list( itertools.chain.from_iterable( truevars ) )
    
    if all( var in mgr for mgr in mgrs for var in truevars ):
        
        ''' Generates and divides the canvas '''
        nyvars, nxvars = optCanvasDivision( nvars )
        canvas = rt.TCanvas( name, title, 300*nyvars, 300*nxvars )
        canvas.Divide( nyvars, nxvars )

        canvas_info = CanvasStorer(canvas)
        
        nmgrs = len( mgrs )
        ''' If cuts are specified it calculates the true managers '''
        if cuts:
            for i, mgr in enumerate( mgrs ):
                mgr, mgr.Name = mgr.subSample( cuts = cuts ), mgr.Name
        
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

            canvas_info.InfoObjs += [rlegend, rtxtinf]
        
        ''' Generates and draws the histograms '''
        for iv, var in enumerate( variables ):
            
            canvas.cd( iv + 1 )

            ''' This is done to reduce disk usage '''
            totlst = [ mgr.varEvents( [ var ], cuts = cuts ) for mgr in mgrs ]

            ''' Extract the ranges for each variable (if any) '''
            if var in ranges.keys():
                vmin, vmax = ranges[var]
            else:
                vmax = max( el for lst in totlst for el in lst )
                vmin = min( el for lst in totlst for el in lst )

            entries = []
            hists   = []
            for im, ( mgr, vals ) in enumerate( zip( mgrs, totlst ) ):
                hname = mgr.Name + '_' + var
                h = makeHistogram( vals,
                                   name  = hname,
                                   title = var,
                                   nbins = nbins,
                                   vmin  = vmin,
                                   vmax  = vmax )
                
                entries.append(h.GetEntries())
                hists.append( h )
                
                flist[ im ].applyFormat( h )

                h.GetXaxis().SetTitle(var)
                
            ''' Draw histograms, with error bars if specified '''
            hists = drawHistograms(hists, drawopt = errors*'E', norm = norm, title = '')

            if legend and iv == 0:
                ''' In the first iteration add the entries to the legend '''
                for mgr, h, sw in zip(mgrs, hists[1:], entries):
                    rlegend.AddEntry( h, '#bf{' + mgr.Name + '}', 'L' )
                    rtxtinf.AddText( mgr.Name + ': %i' % sw )
                    
            canvas_info.DataObjs += hists
            
        if legend:
            pad = canvas.cd( nvars )
            pad.Divide( 2, 1 )
            pad.cd( 1 ); rlegend.Draw()
            pad.cd( 2 ); rtxtinf.Draw()
            
        canvas.Update()
        
        return canvas_info
    else:
        sendErrorMsg('Any of the managers does not have access to some of the variables')
        return


def optCanvasDivision( nvars ):
    '''
    Create the optimal canvas division for a given number of pads
    '''
    nstsq = nearestSquare( nvars )
    nstrt = int( sqrt( nvars ) )
    
    if nstsq >= nvars:
        nxvars = nstrt
        nyvars = nxvars
    else:
        nxvars = nstrt
        nyvars = nstrt + 1
        
    return nyvars, nxvars
