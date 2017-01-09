#////////////////////////////////////////////////////////////////
#//
#//  Python modules
#//
#// -------------------------------------------------------------
#//
#//  AUTHOR: Miguel Ramos Pernas
#//  e-mail: miguel.ramos.pernas@cern.ch
#//
#//  Last update: 09/01/2017
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


from ROOT import ( TCanvas, TLegend, TPaveText, gStyle,
                   TGraph, TGraphErrors, TH1,
                   TH1F, TH1D,
                   TH2F, TH2D,
                   kBlue, kRed, kOrange, kGreen, kMagenta, kCyan )
from array import array
from math import sqrt
import itertools
import numpy
import sys
from Isis.MathExt import NearestSquare
from Isis.Utils import CalcMinDist, FormatEvalExpr


#_______________________________________________________________________________
# Output class for an iteration over a < FormatList > instance. This class
# stores the information of the color, line style and fill style. It also has
# features to apply it to Root objects.
class FormatListIter:

    def __init__( self, color, linest, markst, fillst ):
        '''
        The color and the line, marker and fill styles are passed to the class
        '''
        self.Color  = color
        self.LineSt = linest
        self.MarkSt = markst
        self.FillSt = fillst

    def GetStyles( self ):
        '''
        Returns the members of this class in a tuple
        '''
        return tuple( el for el in ( self.Color, self.LineSt, self.MarkSt, self.FillSt )
                      if el != None )
        
    def ApplyFormat( self, obj, lw = 2 ):
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

#_______________________________________________________________________________
# This class allows to generate a list storing colors and line, marker and fill
# styles, so one can iterate over it. The formats are extracted using
# < __getitem__ > or on an iteration process. If the index is greater than the
# number of colors in the list, the color will correspond to the loop number,
# added to the remainder araising from the division. For the other features
# the quantity is directly extracted from the remainder.
class FormatList:

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
            self.Colors = [ kBlue, kRed, kOrange, kGreen, kMagenta, kCyan ]
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
            print 'ERROR: Lists passed to FormatList instance have different lengths'

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
        
#_______________________________________________________________________________
# Draws the given list of histograms. If the variable < norm > is set to True,
# then the histograms will be normalized and the function will return
# the list with the clone histograms. It always returns a list with at least an
# histogram used to give format to the plot. The draw options are set using the
# < drawopt > keyword.
def DrawHistograms( hlst, drawopt = '', norm = True ):
    if norm:
        imax = max( h.GetMaximum()*1./h.GetSumOfWeights() for h in hlst )
        imin = min( h.GetMinimum()*1./h.GetSumOfWeights() for h in hlst )
        meth = TH1.DrawNormalized
    else:
        imax = max( h.GetMaximum() for h in hlst )
        imin = min( h.GetMinimum() for h in hlst )
        meth = TH1.Draw
    offset = ( imax + imin )/10.
    vmin   = min( h.GetXaxis().GetXmin() for h in hlst )
    vmax   = max( h.GetXaxis().GetXmax() for h in hlst )
    hformat = hlst[ 0 ].__class__( 'HistFormat', 'HistFormat', 1, vmin, vmax )
    hformat.SetBinContent( 1, imin )
    hformat.GetYaxis().SetRangeUser( imin, imax + offset )
    hformat.Draw()
    drawopt += 'SAME'
    return [ hformat ] + [ meth( h, drawopt ) for h in hlst ]

#_______________________________________________________________________________
# This function extracts the points of the given array of data which are
# supposed to be used to make a histogram
def ExtractHistPoints( varlst, nbins, vmin = None, vmax = None ):
    if vmin == None:
        vmin = min( varlst )
    if vmax == None:
        vmax  = max( varlst )
        vmax += ( vmax - vmin )/( 2.*nbins )
    return [ i for i, v in enumerate( varlst ) if v >= vmin and v < vmax ]

#_______________________________________________________________________________
# Returns the histogram constructor given the type as a string
def HistFromType( tp, dim = 1 ):
    if tp not in ( 'float', 'double', 'int' ):
        print 'ERROR: Histogram type', tp, 'not known'
        return
    if dim == 1:
        if tp == 'float':
            return TH1F
        elif tp == 'double':
            return TH1D
        else:
            return TH1I
    elif dim == 2:
        if tp == 'float':
            return TH2F
        elif tp == 'double':
            return TH2D
        else:
            return TH2I
    else:
        print 'ERROR: Histogram dimension', dim, 'not allowed'

#_______________________________________________________________________________
# This function imports different plotting classes from Root
def ImportRootPlotClasses():
    glob = sys._getframe( 1 ).f_globals
    loc  = sys._getframe( 1 ).f_locals
    modlist = [ 'gROOT', 'TBrowser', 
                'TDirectory', 'TDirectoryFile', 'gDirectory', 'TFile',
                'TTree', 'TBranch', 'TLeaf',
                'TCanvas', 'TPave', 'TPaveText', 'TLegend',
                'TH1D', 'TH1F', 'TH1I',
                'TH2D', 'TH2F', 'TH2I',
                'TGraph', 'TGraphErrors',
                'TF1', 'TF2', 'TLine',
                'TColor',
                'kBlue', 'kViolet', 'kMagenta', 'kPink',
                'kRed', 'kOrange', 'kYellow',
                'kSpring', 'kGreen', 'kTeal', 'kCyan', 'kAzure',
                'kWhite', 'kBlack', 'kGray' ]
    glob[ 'ROOT' ] = __import__( 'ROOT' )
    for el in modlist:
        glob[ el ] = __import__( 'ROOT.' + el, glob, loc, [ '*' ] )

#_______________________________________________________________________________
# This function creates a 1-D adaptive binning histogram given a name, the
# minimum occupancy value and a list. Adding a list of weights is also possible.
def MakeAdaptiveBinnedHist( name, minocc, values,
                            htype   = 'double',
                            title   = None,
                            weights = False,
                            xtitle  = None,
                            ytitle  = 'Entries' ):

    if title == None:
        title = name
    if not xtitle:
        xtitle = name
    histcall = HistFromType( htype, 1 )
    
    ''' Calculates the array of weights '''
    length = len( values )
    if weights:
        sw    = float( sum( weights ) )
        nbins = int( sw )/minocc
    else:
        weights = length*[ 1. ]
        sw      = float( length )
        nbins   = length/minocc
    
    idxs   = ExtractHistPoints( values, nbins )
    values = [ values[ i ] for i in idxs ]
    vmax   = max( values )
    
    ''' If the occupancy requested is too big, an error message is displayed '''
    if nbins == 0:
        print 'ERROR: Occupancy requested is too big:', minocc
    
    ''' Creates a list with the values and the weights joint and sorts it by the values '''
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

    ''' To create the Root histogram, an array of doubles has to be created, with the minimum
    value for the bins '''
    bins = array( 'd', ( nbins + 1 )*[ 0. ] )
    for i, ib in enumerate( binlist ):
        bins[ i ] = ib[ 0 ]
    bins[ -1 ] = vmax
    
    hist = histcall( name, title, nbins, bins )
    hist.GetXaxis().SetTitle( xtitle )
    hist.GetYaxis().SetTitle( ytitle )

    return hist

#_______________________________________________________________________________
# Creates a correlation histogram given a list of lists. By default it is drawn
# in color, without palette, and with the contents written inside each bin. No
# statistical box is displayed neither.
def MakeCorrelationHist(matrix, name = '', title = None, vartitles = []):

    if title == None:
        title = name
    
    lm = len( matrix )
    lv = len( vartitles )
    if vartitles != []:
        if lm != lv:
            print 'WARNING: Number of titles is not the same as that of the matrix. '\
                'New variables will have names < Variables_# >'
            for i in xrange( lv + 1, lm ):
                vartitles.append( 'Variable_' + str( i ) )
    else:
        vartitles = [ 'Variable_' + str( i ) for i in xrange( lm ) ]

    corr_matrix = 100*numpy.corrcoef( matrix )
    
    hist = TH2D( name, title, lm, 0, lm, lm, 0, lm )
    for i, row in enumerate( corr_matrix ):
        for j, el in enumerate( row ):
            hist.SetBinContent( i + 1, j + 1, int( el ) )
    for i, tit in enumerate( vartitles ):
        hist.GetXaxis().SetBinLabel( i + 1, tit )
        hist.GetYaxis().SetBinLabel( i + 1, tit )

    hist.SetOption( 'COLTEXT' )
    hist.SetStats( False )
            
    return hist

#_______________________________________________________________________________
# Returns a histogram containing the cumulative distribution of that given. If
# the option < norm > is given, the histogram will be scaled in such a way that
# the maximum value will be one.
def MakeCumulative( hist, name = '', norm = True, title = None ):
    if title == None:
        title = name
    chist = hist.Clone()
    chist.SetNameTitle( name, title )
    cumulative = chist.GetBinContent( 1 )
    for i in xrange( 2, hist.GetNbinsX() + 1 ):
        cumulative += hist.GetBinContent( i )
        chist.SetBinContent( i, cumulative )
        chist.SetBinError( i, sqrt( cumulative ) )
    if norm:
        chist.Scale( 1./chist.GetMaximum() )
    return chist

#_______________________________________________________________________________
# Function to generate a Root histogram given a list. By default no y-title is
# drawn, but it can be set with the < ytitle > option. For values of type int,
# the histogram will be of type double.
def MakeHistogram( var,
                   name   = '',
                   nbins  = 100,
                   htype  = 'double',
                   title  = None,
                   vmin   = None,
                   vmax   = None,
                   wvar   = False,
                   xtitle = '',
                   ytitle = '' ):
    
    if title == None:
        title = name
    histcall = HistFromType( htype, 1 )

    idxs  = ExtractHistPoints( var, nbins, vmin = vmin, vmax = vmax )
    varin = [ var[ i ] for i in idxs ]
    vmin  = min( v for v in varin + [ vmin ] if v != None )
    vmax  = max( v for v in varin + [ vmax ] if v != None )
    
    hist = histcall( name, title, nbins, vmin, vmax )
    
    if wvar:
        for el, w in zip( var, wvar ):
            hist.Fill( el, w )
    else:
        for el in var:
            hist.Fill( el )
    
    hist.GetXaxis().SetTitle( xtitle )
    hist.GetYaxis().SetTitle( ytitle )
    
    return hist

#_______________________________________________________________________________
# Creates a 2-dimensional histogram given two lists
def MakeHistogram2D( xvar, yvar,
                     name   = '',
                     htype  = 'double',
                     title  = None,
                     wvar   = False,
                     xbins  = 100,
                     xmax   = None,
                     xmin   = None,
                     xtitle = 'X',
                     ybins  = 100,
                     ymax   = None,
                     ymin   = None,
                     ytitle = 'Y' ):

    if title == None:
        title = name
    histcall = HistFromType( htype, 2 )
    
    xidxs = ExtractHistPoints( xvar, xbins, vmin = xmin, vmax = xmax )
    yidxs = ExtractHistPoints( yvar, ybins, vmin = ymin, vmax = ymax )

    ''' The values used are the intersection between the two lists '''
    idxs = set( xidxs ) & set( yidxs )

    xvarin = [ xvar[ i ] for i in idxs ]
    xmin   = min( v for v in xvarin + [ xmin ] if v != None )
    xmax   = max( v for v in xvarin + [ xmax ] if v != None )

    yvarin = [ yvar[ i ] for i in idxs ]
    ymin   = min( v for v in yvarin + [ ymin ] if v != None )
    ymax   = max( v for v in yvarin + [ ymax ] if v != None )
    
    hist = histcall( name, title, xbins, xmin, xmax, ybins, ymin, ymax )

    if wvar:
        for x, y, w in zip( xvar, yvar, wvar ):
            hist.Fill( x, y, w )
    else:
        for x, y in zip( xvar, yvar ):
            hist.Fill( x, y )
    hist.GetXaxis().SetTitle( xtitle )
    hist.GetYaxis().SetTitle( ytitle )
    return hist

#_______________________________________________________________________________
# Generates a scatter plot given two lists of data
def MakeScatterPlot( xvar, yvar, xerr = False, yerr = False,
                     name   = '',
                     title  = None,
                     xtitle = 'X',
                     ytitle = 'Y' ):

    if title == None:
        title = name
    npoints = len( xvar )
    xvar    = array( 'd', xvar )
    yvar    = array( 'd', yvar )
    if xerr or yerr:
        if   not xerr:
            xerr = array( 'd', npoints*[ 0 ] )
            yerr = array( 'd', yerr )
        elif not yerr:
            xerr = array( 'd', yerr )
            yerr = array( 'd', npoints*[ 0 ] )
        else:
            xerr = array( 'd', xerr )
            yerr = array( 'd', yerr )
        graph = TGraphErrors( npoints, xvar, yvar, xerr, yerr )
    else:
        graph = TGraph( npoints, xvar, yvar )
    
    graph.SetNameTitle( name, title )
    graph.GetXaxis().SetTitle( xtitle )
    graph.GetYaxis().SetTitle( ytitle )
    
    return graph

#_______________________________________________________________________________
# This function plots in the same canvas the distributions of the given
# variables from different DataManager classes. Different options can
# also been provided to modify the canvas and the information displayed.
def MultiPlot( mgrs, variables,
               cuts   = False,
               errors = False,
               flist  = FormatList(),
               legend = True,
               name   = 'canvas',
               nbins  = 100,
               norm   = True,
               title  = None ):

    if title == None:
        title = name
    
    nvars    = len( variables ) + 1
    results  = {}

    ''' Get the true variables associated with the given expressions '''
    truevars = [ FormatEvalExpr( var )[ 1 ] for var in variables ]
    truevars = list( itertools.chain.from_iterable( truevars ) )
    
    if all( var in mgr for mgr in mgrs for var in truevars ):
        ''' Checks if the number of variables is a square number '''
        nstsq = NearestSquare( nvars )
        nstrt = int( sqrt( nstsq ) )
        if nstsq >= nvars:
            nxvars = nstrt
            nyvars = nxvars
        else:
            nxvars = nstrt
            nyvars = nstrt + 1
        
        ''' Generates and divides the canvas '''
        canvas = TCanvas( name, title, 300*nyvars, 300*nxvars )
        canvas.Divide( nyvars, nxvars )
        
        nmgrs = len( mgrs )
        ''' If cuts are specified it calculates the true managers '''
        if cuts:
            for i, mgr in enumerate( mgrs ):
                mgr, mgr.Name = mgr.SubSample( cuts = cuts ), mgr.Name
        
        ''' Disables the stat box of the histograms '''
        gStyle.SetOptStat( 0 )
        
        ''' Constructs the legend and the information panel if specified '''
        if legend:
            rlegend = TLegend( 0.1, 0.8 - nmgrs*0.05, 0.9, 0.9 )
            rlegend.SetHeader( '#bf{-- Legend --}' )
            rlegend.SetTextAlign( 22 )
            rlegend.SetTextSize( 0.075 )
            rlegend.SetFillColor( 15 )
            rtxtinf = TPaveText( 0.1, 0.8 - nmgrs*0.05, 0.9, 0.9 )
            rtxtinf.AddText( '-- Number of entries --' )
            rtxtinf.SetTextSize( 0.075 )
            rtxtinf.SetFillColor( 42 )
            rtxtinf.SetShadowColor( 0 )
        
        ''' Generates and draws the histograms '''
        for iv, var in enumerate( variables ):
            canvas.cd( iv + 1 )

            ''' This is done to reduce disk usage '''
            totlst = [ mgr.VarEvents( [ var ], cuts = cuts ) for mgr in mgrs ]
            vmax   = max( el for lst in totlst for el in lst )
            vmin   = min( el for lst in totlst for el in lst )
            hists  = []
            
            for im, ( mgr, vals ) in enumerate( zip( mgrs, totlst ) ):
                hname = mgr.Name + '_' + var
                h = MakeHistogram( vals,
                                   name  = hname,
                                   title = var,
                                   nbins = nbins,
                                   vmin  = vmin,
                                   vmax  = vmax )
                hists.append( h )
                if norm:
                    h.Scale( float( norm )/h.GetSumOfWeights() )

                flist[ im ].ApplyFormat( h )
                    
                if legend and iv == 0:
                    ''' In the first iteration adds the entries to the legend '''
                    rlegend.AddEntry( h, '#bf{' + mgr.Name + '}', 'L' )
                    rtxtinf.AddText( mgr.Name + ': %i' % h.GetEntries() )
                results[ hname ] = h
            ''' The maximum of the y-axis in the pad is set to the 110% of the maximum
            value for all the histograms to be drawn '''
            hists[ 0 ].SetMaximum( 1.1*max( h.GetMaximum() for h in hists ) )
            for h in hists:
                if errors: h.Draw( 'SAMEE' )
                else:      h.Draw( 'SAME'  )
        if legend:
            pad = canvas.cd( nvars )
            pad.Divide( 2, 1 )
            pad.cd( 1 ); rlegend.Draw()
            pad.cd( 2 ); rtxtinf.Draw()
            results[ 'legend' ] = rlegend
            results[ 'info' ]   = rtxtinf
        canvas.Update()
        results[ name ] = canvas
        return results
    else:
        print 'ERROR: Any of the managers does not have access to some of the variables'
        return
