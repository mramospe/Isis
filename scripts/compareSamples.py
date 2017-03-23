# -------------------------------------------------------------------
# Script to plot a list of variables shared by different Root samples
# -------------------------------------------------------------------

from Isis.DataManagement import DataMgr
from Isis.PlotTools import multiPlot

# List of variables to plot
varlst = ['var0', 'var1', 'var2']

# Ranges for the variables to be displayed
ranges = {'var0': (0, 10),
          'var1': (0, 10),
          'var2': (0, 10)
      }

# Dictionary with the name of the managers, path to the root file and
# to the tree inside it
paths = {
    'smp1' : ('smp1.root', 'DecayTree'),
    'smp2' : ('smp2.root', 'DecayTree')
}

# Load the data
mgrs = {}
for mgrname, inputs in paths.iteritems():

    fpath, tpath = inputs
    
    mgrs[mgrname] = DataMgr(mgrname, fpath, tpath, variables = varlst)

# Make the canvas
output = multiPlot([mgr for mgr in mgrs.itervalues()], varlst,
                   cuts   = False,
                   norm   = True,
                   name   = 'canvas',
                   ranges = ranges)
