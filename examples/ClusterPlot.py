################################################
# Script to read the results of Clustering.cpp #
################################################

from ROOT import TCanvas, TFile, gStyle
from Isis.plot_tools import ColorList

gStyle.SetOptTitle( 0 )

# Gets the scatter plots from the file
ifile  = TFile.Open( "Clustering.root" )
lst    = ifile.GetListOfKeys()
graphs = []
for kw in lst:
    graphs.append( ifile.Get( kw.GetName() + '/gr' ) )

# Minimum and maximum values for each axis
mnx, mxx = -15, 15
mny, mxy = -10, 10
graphs[ 0 ].SetMaximum( mxy )
graphs[ 0 ].SetMinimum( mny )

# Formats the different objects to be drawn
colorlst = ColorList()
for el, col in zip( graphs, colorlst ):
    el.SetMarkerColor( col )
    el.SetLineColor( col )
    el.SetMarkerStyle( 6 )
    el.GetXaxis().SetLimits( mnx, mxx )

# Draws the different cluster points in the same canvas
canvas = TCanvas()
graphs[ 0 ].Draw("AP")
for el in graphs[ 1: ]:
    el.Draw("SAMEP")
