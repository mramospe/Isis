#////////////////////////////////////////////////////////////////
#//                                                            //
#//  Python modules                                            //
#//                                                            //
#// ---------------------------------------------------------- //
#//                                                            //
#//  AUTHOR: Miguel Ramos Pernas                               //
#//  e-mail: miguel.ramos.pernas@cern.ch                       //
#//                                                            //
#//  Last update: 13/06/2016                                   //
#//                                                            //
#// ---------------------------------------------------------- //
#//                                                            //
#//  Description:                                              //
#//                                                            //
#//  This module implements different tools to generate plots  //
#//  using Root objects.                                       //
#//                                                            //
#// ---------------------------------------------------------- //
#////////////////////////////////////////////////////////////////


from ROOT import ( TCanvas, TLegend, TPaveText, gStyle,
                   TGraph, TGraphErrors, TH1,
                   TH1F, TH1D,
                   TH2F, TH2D,
                   kBlue, kRed, kOrange, kGreen, kMagenta, kCyan )
from array import array
from math import sqrt
import sys
from Isis.MathExt import NearestSquare
from Isis.Utils import CalcMinDist


#_______________________________________________________________________________
# This class allows to generate a color list to iter over. The colors are given
# by < __getitem__ > method or in an iteration process. The number of times the
# given index is greater than the number of colors in the list, corresponds to
# the number that is added to the remainder that araises from the division.
class ColorList:

    def __init__( self, *args ):
        ''' A set of colors can be specified in the constructor as < args > '''
        self.Iter = 0
        if args: self.Colors = args
        else:    self.Colors = [ kBlue, kRed, kOrange, kGreen, kMagenta, kCyan ]

    def __getitem__( self, idx ):
        ''' Gets the color for the given index '''
        return self.Colors[ idx % len( self.Colors ) ] + idx/len( self.Colors )

    def __iter__( self ):
        ''' Definition of the iterator '''
        self.Iter = 0
        return self

    def next( self ):
        ''' Sets the new value for the iteration. In order to use this class in an
        iterative mode, another iterable object has to be the one that raises the
        exception to stop the iteration. '''
        nloop = self.Iter / len( self.Colors )
        niter = self.Iter % len( self.Colors )
        self.Iter += 1
        return self.Colors[ niter ] + nloop

#_______________________________________________________________________________
# Draws the given histograms in such an order, that the first to be drawn is
# that with the maximum value for the Y axis. If the variable < norm > is set
# to True, then the histograms will be normalized and the function will return
# the list with the clone histograms. On the other hand, if it is set to True,
# it will return a list of None values. The draw options are set using the
# < drawopt > keyword.
def DrawHistograms( *args, **kwargs ):
    drawopt = kwargs.get( 'drawopt', '' )
    norm    = kwargs.get( 'norm', True )
    if norm:
        imax = [ h.GetMaximum()*1./h.GetSumOfWeights() for h in args ]
        meth = TH1.DrawNormalized
    else:
        imax = [ h.GetMaximum() for h in args ]
        meth = TH1.Draw
    imax = imax.index( max( imax ) )
    hlst = len( args )*[ 0 ]
    lst  = range( len( args ) )
    lst.remove( imax )
    hlst[ imax ] = meth( args[ imax ], drawopt )
    drawopt += 'SAME'
    for i in lst:
        hlst[ i ] = meth( args[ i ], drawopt )
    return hlst

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
# This function imports different plot modules from Root
def ImportPlotModules():
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
def MakeAdaptiveBinnedHist( name, minocc, values, weights = False, **kwargs ):

    ''' These are the options that can be passed to the function '''
    title = kwargs.get( 'title', name )
    if 'vmin' in kwargs:
        vmin   = kwargs[ 'vmin' ]
        values = [ val for val in values if val >= vmin ]
    if 'vmax' in kwargs:
        vmax   = kwargs[ 'vmax' ]
        values = [ val for val in values if val < vmax ]
    else:
        vmax   = max( values ) + CalcMinDist( values, False )/2.
    xtitle   = kwargs.get( 'xtitle', name )
    ytitle   = kwargs.get( 'ytitle', 'Entries' )
    histcall = HistFromType( kwargs.get( 'htype', 'double' ), 1 )
    
    ''' Calculates the array of weights '''
    length = len( values )
    if weights:
        sw    = float( sum( weights ) )
        nbins = int( sw )/minocc
    else:
        weights = length*[ 1. ]
        sw      = float( length )
        nbins   = length/minocc
    
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
# Returns a histogram containing the cumulative distribution of that given. If
# the option < norm > is given, the histogram will be scaled in such a way that
# the maximum value will be one.
def MakeCumulative( hist, **kwargs ):
    name  = kwargs.get( 'name', '' )
    norm  = kwargs.get( 'norm', True )
    title = kwargs.get( 'title', name )
    chist = hist.Clone()
    chist.SetNameTitle( name, title )
    cumulative = chist.GetBinContent( 1 )
    for i in xrange( 2, hist.GetNbinsX() + 1 ):
        cumulative += hist.GetBinContent( i )
        chist.SetBinContent( i, cumulative )
    if norm:
        chist.Scale( 1./chist.GetMaximum() )
    return chist

#_______________________________________________________________________________
# Function to generate a Root histogram given a list. By default no ytitle is
# drawn, but it can be set with the < ytitle > option. For values of type int,
# the histogram will be of type double.
def MakeHistogram( name, var, wvar = False, **kwargs ):
    title    = kwargs.get( 'title', name )
    nbins    = kwargs.get( 'nbins', 100 )
    xtitle   = kwargs.get( 'xtitle', name )
    ytitle   = kwargs.get( 'ytitle', 'Entries' )
    if 'vmin' in kwargs:
        vmin = kwargs[ 'vmin' ]
        var  = [ val for val in var if val >= vmin ]
    else:
        vmin = min( var )
    if 'vmax' in kwargs:
        vmax = kwargs[ 'vmax' ]
        var  = [ val for val in var if val < vmax ]
    else:
        vmax = max( var ) + CalcMinDist( var, False )/2.
        var  = [ val for val in var if val < vmax ]
    histcall = HistFromType( kwargs.get( 'htype', 'double' ), 1 )
    
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
def MakeHistogram2D( name, xvar, yvar, wvar = False, **kwargs ):
    title    = kwargs.get( 'title', name )
    xbins    = kwargs.get( 'xbins', 100 )
    ybins    = kwargs.get( 'ybins', 100 )
    xtitle   = kwargs.get( 'xtitle', 'X' )
    ytitle   = kwargs.get( 'ytitle', 'Y' )
    xmax     = kwargs.get( 'xmax', max( xvar ) )
    ymax     = kwargs.get( 'ymax', max( yvar ) )
    xmin     = kwargs.get( 'xmin', min( xvar ) )
    ymin     = kwargs.get( 'ymin', min( yvar ) )
    histcall = HistFromType( kwargs.get( 'htype', 'double' ), 2 )
    
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
def MakeScatterPlot( xvar, yvar, xerr = False, yerr = False, **kwargs ):
    name   = kwargs.get( 'name', '' )
    title  = kwargs.get( 'title', name )
    xtitle = kwargs.get( 'xtitle', 'X' )
    ytitle = kwargs.get( 'ytitle', 'Y' )

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
def MultiPlot( mngrs, variables, **kwargs):
    colors = ColorList( kwargs.get( 'colors', None ) )
    cuts   = kwargs.get( 'cuts', False )
    errors = kwargs.get( 'errors', False )
    legend = kwargs.get( 'legend', True )
    name   = kwargs.get( 'name', 'canvas' )
    title  = kwargs.get( 'title', name )
    nbins  = kwargs.get( 'nbins', 100 )
    norm   = kwargs.get( 'norm', True )
    
    nvars   = len( variables ) + 1
    results = {}
    if all( var in mngr.Variables for mngr in mngrs for var in variables ):
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
        
        nmngrs = len( mngrs )
        ''' If cuts are specified it calculates the true managers '''
        if cuts:
            for i in xrange( nmngrs ):
                mngrs[ i ], mngrs[ i ].Name = mngrs[ i ].SubSample( cuts = cuts ), mngrs[ i ].Name
        
        ''' Disables the stat box of the histograms '''
        gStyle.SetOptStat( 0 )
        
        ''' Constructs the legend and the information panel if specified '''
        if legend:
            rlegend = TLegend( 0.1, 0.8 - nmngrs*0.05, 0.9, 0.9 )
            rlegend.SetHeader( '#bf{-- Legend --}' )
            rlegend.SetTextAlign( 22 )
            rlegend.SetTextSize( 0.075 )
            rlegend.SetFillColor( 17 )
            rtxtinf = TPaveText( 0.1, 0.8 - nmngrs*0.05, 0.9, 0.9 )
            rtxtinf.AddText( '-- Number of entries --' )
            rtxtinf.SetTextSize( 0.075 )
            rtxtinf.SetFillColor( 42 )
            rtxtinf.SetShadowColor( 0 )
        
        ''' Generates and draws the histograms '''
        for iv, var in enumerate( variables ):
            canvas.cd( iv + 1 )
            totlst = [ el for mngr in mngrs for el in mngr[ var ]  ]
            vmin, vmax, hists = min( totlst ), max( totlst ), []
            for im, mngr in enumerate( mngrs ):
                hname = mngr.Name + '_' + var
                hists.append( mngr.MakeHistogram( hname,
                                                  var,
                                                  title = var,
                                                  nbins = nbins,
                                                  vmin  = vmin,
                                                  vmax  = vmax, 
                                                  cuts  = cuts ) )
                h = hists[ -1 ]
                if norm:
                    h.Scale( float( norm )/h.GetEntries() )
                h.SetLineColor( colors[ im ] )
                h.SetMarkerColor( colors[ im ] )
                if legend and iv == 0:
                    ''' In the first iteration adds the entries to the legend '''
                    rlegend.AddEntry( h, '#bf{' + mngr.Name + '}', 'L' )
                    rtxtinf.AddText( mngr.Name + ': %i' % h.GetEntries() )
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
