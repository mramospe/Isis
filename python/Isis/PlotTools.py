#////////////////////////////////////////////////////////////////
#//                                                            //
#//  Python modules                                            //
#//                                                            //
#// ---------------------------------------------------------- //
#//                                                            //
#//  AUTHOR: Miguel Ramos Pernas                               //
#//  e-mail: miguel.ramos.pernas@cern.ch                       //
#//                                                            //
#//  Last update: 21/04/2016                                   //
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
        self.Iter += 3
        return self.Colors[ niter ] + nloop

#_______________________________________________________________________________
# Draws the given histograms in such an order, that the first to be drawn is
# that with the maximum value for the Y axis. If the variable < norm > is set
# to True, then the histograms will be normalized and the function will return
# the list with the clone histograms. On the other hand, if it is set to True,
# it will return a list of None values. The draw options are set using the
# < drawopt > keyword.
def DrawHistograms( *args, **kwargs ):
    if 'drawopt' in kwargs: drawopt = kwargs[ 'drawopt' ]
    else: drawopt = ''
    if 'norm' in kwargs: norm = kwargs[ 'norm' ]
    else: norm = True
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
def HistByType( tp, dim = 1 ):
    if tp not in ( 'float', 'double', 'int' ):
        print "ERROR: Histogram type", tp, "not known"
        return
    if dim == 1:
        if tp == "float":
            return TH1F
        elif tp == "double":
            return TH1D
        else:
            return TH1I
    elif dim == 2:
        if tp == "float":
            return TH2F
        elif tp == "double":
            return TH2D
        else:
            return TH2I
    else:
        print "ERROR: Histogram dimension", dim, "not known"

#_______________________________________________________________________________
# This function imports different plot modules from Root
def ImportPlotModules():
    glob = sys._getframe( 1 ).f_globals
    loc  = sys._getframe( 1 ).f_locals
    modlist = [ "gROOT", "TBrowser", 
                "TDirectory", "TDirectoryFile", "gDirectory", "TFile",
                "TTree", "TBranch", "TLeaf",
                "TCanvas", "TPave", "TPaveText", "TLegend",
                "TH1D", "TH1F", "TH1I",
                "TH2D", "TH2F", "TH2I",
                "TGraph", "TGraphErrors",
                "TF1", "TF2", "TLine",
                "TColor",
                "kBlue", "kViolet", "kMagenta", "kPink",
                "kRed", "kOrange", "kYellow",
                "kSpring", "kGreen", "kTeal", "kCyan", "kAzure",
                "kWhite", "kBlack", "kGray" ]
    glob[ "ROOT" ] = __import__( "ROOT" )
    for el in modlist:
        glob[ el ] = __import__( "ROOT." + el, glob, loc, [ '*' ] )

#_______________________________________________________________________________
# This function creates a 1-D adaptive binning histogram given a name, the
# minimum occupancy value and a list. Adding a list of weights is also possible.
def MakeAdaptiveBinnedHist( name, minocc, values, weights = False, **kwargs ):
    if 'title' in kwargs: title = kwargs[ 'title' ]
    else: title = name
    if 'vmin' in kwargs:
        vmin   = kwargs[ 'vmin' ]
        values = [ val for val in values if val >= vmin ]
    if 'vmax' in kwargs:
        vmax   = kwargs[ 'vmax' ]
        values = [ val for val in values if val < vmax ]
    else:
        vmax   = max( values )
    if 'xtitle' in kwargs: xtitle = kwargs[ 'xtitle' ]
    else: xtitle = name
    if 'vtype' in kwargs: histcall = HistByType( kwargs[ 'vtype' ] )
    else: histcall = TH1D
    
    ''' Calculates the minimum distance between points '''
    length = len( values )
    delta  = CalcMinDist( values )/2.

    ''' Calculates the array of weights '''
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
    
    ''' Defines the ranges for the bins '''
    for ib in binlist:
        ib[ 0 ] -= delta
    
    ''' To create the Root histogram, an array of doubles has to be created, with the minimum
    value for the bins '''
    bins = array( 'd', ( nbins + 1 )*[ 0. ] )
    for i, ib in enumerate( binlist ):
        bins[ i ] = ib[ 0 ]
    bins[ -1 ] = max( values )[ 0 ] + delta
    
    return histcall( name, title, nbins, bins )

#_______________________________________________________________________________
# Function to generate a Root histogram given a list. By default no ytitle is
# drawn, but it can be set with the < ytitle > option. For values of type int,
# the histogram will be of type double.
def MakeHistogram( name, var, wvar = False, **kwargs ):
    if "title" in kwargs: title = kwargs[ "title" ]
    else: title = name
    if "nbins" in kwargs: nbins = kwargs[ "nbins" ]
    else: nbins = 100
    if "xtitle" in kwargs: xtitle = kwargs[ "xtitle" ]
    else: xtitle = name
    if "vmax" in kwargs: vmax = kwargs[ "vmax" ]
    else: vmax = max( var )
    if "vmin" in kwargs: vmin = kwargs[ "vmin" ]
    else: vmin = min( var )
    if "vtype" in kwargs: histcall = HistByType( kwargs[ "vtype" ] )
    else: histcall = TH1D
    
    hist = histcall( name, title, nbins, vmin, vmax )
    
    if wvar:
        for el, w in zip( var, wvar ):
            hist.Fill( el, w )
    else:
        for el in var:
            hist.Fill( el )
    hist.GetXaxis().SetTitle( xtitle )
    if "ytitle" in kwargs:
        hist.GetYaxis().SetTitle( kwargs[ "ytitle" ] )
    return hist

#_______________________________________________________________________________
# Creates a 2-dimensional histogram given two lists
def MakeHistogram2D( name, xvar, yvar, wvar = False, **kwargs ):
    if "title" in kwargs: title = kwargs[ "title" ]
    else: title = name
    if "xbins" in kwargs: xbins = kwargs[ "xbins" ]
    else: xbins = 100
    if "ybins" in kwargs: ybins = kwargs[ "ybins" ]
    else: ybins = 100
    if "xtitle" in kwargs: xtitle = kwargs[ "xtitle" ]
    else: xtitle = "X"
    if "ytitle" in kwargs: ytitle = kwargs[ "ytitle" ]
    else: ytitle = "Y"
    if "xmax" in kwargs: xmax = kwargs[ "xmax" ]
    else: xmax = min( xvar )
    if "ymax" in kwargs: ymax = kwargs[ "ymax" ]
    else: ymax = min( yvar )
    if "xmin" in kwargs: xmin = kwargs[ "xmin" ]
    else: xmin = min( xvar )
    if "ymin" in kwargs: ymin = kwargs[ "ymin" ]
    else: ymin = min( yvar )
    if "vtype" in kwargs: histcall = HistByType( kwargs[ "vtype" ], 2 )
    else: histcall = TH2D

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
    if "name" in kwargs: name = kwargs[ "name" ]
    else: name = False
    if "title" in kwargs: title = kwargs[ "title" ]
    else: title = False
    if "xtitle" in kwargs: xtitle = kwargs[ "xtitle" ]
    else: xtitle = "X"
    if "ytitle" in kwargs: ytitle = kwargs[ "ytitle" ]
    else: ytitle = "Y"
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
    if name:
        graph.SetName( name )
    if title:
        graph.SetTitle( title )
    graph.GetXaxis().SetTitle( xtitle )
    graph.GetYaxis().SetTitle( ytitle )
    return graph

#_______________________________________________________________________________
# This function plots in the same canvas the distributions of the given
# variables from different DataManager classes. Different options can
# also been provided to modify the canvas and the information displayed.
def MultiPlot( mngrs, variables, **kwargs):
    if "colors" in kwargs: colors = ColorList( kwargs[ "colors" ] )
    else: colors = ColorList()
    if "cuts" in kwargs: cuts = kwargs[ "cuts" ]
    else: cuts = False
    if "errors" in kwargs: errors = kwargs[ "errors" ]
    else: errors = False
    if "legend" in kwargs: legend = kwargs[ "legend" ]
    else: legend = True
    if "name" in kwargs: name = kwargs[ "name" ]
    else: name = "canvas"
    if "title" in kwargs: title = kwargs[ "title" ]
    else: title = "canvas"
    if "nbins" in kwargs: nbins = kwargs[ "nbins" ]
    else: nbins = 100
    if "norm" in kwargs: norm = kwargs[ "norm" ]
    else: norm = True
    
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
            rlegend.SetHeader( "#bf{-- Legend --}" )
            rlegend.SetTextAlign( 22 )
            rlegend.SetTextSize( 0.075 )
            rlegend.SetFillColor( 17 )
            rtxtinf = TPaveText( 0.1, 0.8 - nmngrs*0.05, 0.9, 0.9 )
            rtxtinf.AddText( "-- Number of entries --" )
            rtxtinf.SetTextSize( 0.075 )
            rtxtinf.SetFillColor( 42 )
            rtxtinf.SetShadowColor( 0 )
        
        ''' Generates and draws the histograms '''
        for iv, var in enumerate( variables ):
            canvas.cd( iv + 1 )
            totlst = [ el for mngr in mngrs for el in mngr[ var ]  ]
            vmin, vmax, hists = min( totlst ), max( totlst ), []
            for im, mngr in enumerate( mngrs ):
                hname = mngr.Name + "_" + var
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
                    rlegend.AddEntry( h, "#bf{" + mngr.Name + "}", "L" )
                    rtxtinf.AddText( mngr.Name + ": %i" % h.GetEntries() )
                results[ hname ] = h
            ''' The maximum of the y-axis in the pad is set to the 110% of the maximum
            value for all the histograms to be drawn '''
            hists[ 0 ].SetMaximum( 1.1*max( h.GetMaximum() for h in hists ) )
            for h in hists:
                if errors: h.Draw( "SAMEE" )
                else:      h.Draw( "SAME"  )
        if legend:
            pad = canvas.cd( nvars )
            pad.Divide( 2, 1 )
            pad.cd( 1 ); rlegend.Draw()
            pad.cd( 2 ); rtxtinf.Draw()
            results[ "legend" ] = rlegend
            results[ "info" ]   = rtxtinf
        canvas.Update()
        results[ name ] = canvas
        return results
    else:
        print "Any of the managers does not have access to some of the variables"
        return
