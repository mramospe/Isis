#////////////////////////////////////////////////////////////////
#//                                                            //
#//  Python modules                                            //
#//                                                            //
#// ---------------------------------------------------------- //
#//                                                            //
#//  AUTHOR: Miguel Ramos Pernas                               //
#//  e-mail: miguel.ramos.pernas@cern.ch                       //
#//                                                            //
#//  Last update: 24/11/2015                                   //
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


from ROOT import ( TGraph, TGraphErrors,
                   TH1F, TH1D, TH1I,
                   TH2F, TH2D, TH2I )
from array import array


#_______________________________________________________________________________
# Function to generate a Root histogram given a list
def MakeHistogram( var, wvar = False, **kwargs ):
    if "name" in kwargs: name = kwargs[ "name" ]
    else: name = "hist"
    if "title" in kwargs: title = kwargs[ "title" ]
    else: title = "hist"
    if "nbins" in kwargs: nbins = kwargs[ "nbins" ]
    else: nbins = 100
    if "vmin" in kwargs: vmin = kwargs[ "vmin" ]
    else: vmin = min( var )
    if "vmax" in kwargs: vmax = kwargs[ "vmax" ]
    else: vmax = max( var )
    if "vtype" in kwargs: tp = kwargs[ "vtype" ]
    else: tp = "double"
    if tp == "float":
        hist = TH1F( name, title, nbins, vmin, vmax )
    elif tp == "double":
        hist = TH1D( name, title, nbins, vmin, vmax )
    elif tp == "int":
        hist = TH1I( name, title, nbins, vmin, vmax )
    else:
        print "Histogram type", tp, "not known"
        return
    if wvar:
        for el, w in zip( var, wvar ):
            hist.Fill( el, w )
    else:
        for el in var:
            hist.Fill( el )
    return hist

#_______________________________________________________________________________
# Creates a 2-dimensional histogram given two lists
def MakeHistogram2D( xvar, yvar, wvar = False, **kwargs ):
    if "name" in kwargs: name = kwargs[ "name" ]
    else: name = "hist"
    if "title" in kwargs: title = kwargs[ "title" ]
    else: title = "hist"
    if "xbins" in kwargs: xbins = kwargs[ "xbins" ]
    else: xbins = 100
    if "ybins" in kwargs: ybins = kwargs[ "ybins" ]
    else: ybins = 100
    if "xmax" in kwargs: xmax = kwargs[ "xmax" ]
    else: xmax = min( xvar )
    if "ymax" in kwargs: ymax = kwargs[ "ymax" ]
    else: ymax = min( yvar )
    if "xmin" in kwargs: xmin = kwargs[ "xmin" ]
    else: xmin = min( xvar )
    if "ymin" in kwargs: ymin = kwargs[ "ymin" ]
    else: ymin = min( yvar )
    if "vtype" in kwargs: tp = kwargs[ "vtype" ]
    else: tp = "double"
    if tp == "float":
        hist = TH2F( name, title, xbins, xmin, xmax, ybins, ymin, ymax )
    elif tp == "double":
        hist = TH2D( name, title, xbins, xmin, xmax, ybins, ymin, ymax )
    elif tp == "int":
        hist = TH2I( name, title, xbins, xmin, xmax, ybins, ymin, ymax )
    else:
        print "Histogram type", tp, "not known"
        return
    if wvar:
        for x, y, w in zip( xvar, yvar, wvar ):
            hist.Fill( x, y, w )
    else:
        for x, y in zip( xvar, yvar ):
            hist.Fill( x, y )
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
