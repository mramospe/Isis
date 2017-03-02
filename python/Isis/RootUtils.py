#/////////////////////////////////////////////////////////////
#//
#//  Python modules
#//
#// ----------------------------------------------------------
#//
#//  AUTHOR: Miguel Ramos Pernas
#//  e-mail: miguel.ramos.pernas@cern.ch
#//
#//  Last update: 02/03/2017
#//
#// ----------------------------------------------------------
#//
#//  Description:
#//
#//  Defines some utils to manage Root objects.
#//
#// ----------------------------------------------------------
#/////////////////////////////////////////////////////////////


import ROOT as rt


#_______________________________________________________________________________
# This function extracts the values from a TGraph object into two lists
def ExtractGraphValues( graph ):
    
    xbin = rt.Double( 0. )
    ybin = rt.Double( 0. )
    np   = graph.GetN()
    xlst = np*[ 0. ]
    ylst = np*[ 0. ]
    
    for ib in xrange( graph.GetN() ):
        graph.GetPoint( ib, xbin, ybin )
        xlst[ ib ] += xbin
        ylst[ ib ] += ybin
        
    return xlst, ylst

#_______________________________________________________________________________
# Function to extract bin centers, values and widths from a Root histogram. If
# the dimension is 2, then centers and widths contain both x and y coordinates.
# The result is given completely splitted.
def ExtractHistValues( hist ):

    if isinstance( hist, rt.TH2 ):
        axis = [ hist.GetXaxis(), hist.GetYaxis() ]
    else:
        axis = [ hist.GetXaxis() ]

    nbins = hist.GetSize() - 2

    values  = []
    centers = [ [] for _ in axis ]
    widths  = [ [] for _ in axis ]
    
    for i in xrange( 1, nbins + 1 ):

        for ax, c, w in zip( axis, centers, widths ):
            c.append( ax.GetBinCenter( i ) )
            w.append( ax.GetBinWidth( i ) )
            
        values.append( hist.GetBinContent( i ) )
        
    return centers + [ values ] + widths
