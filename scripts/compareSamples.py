# -------------------------------------------------------------------
# Script to plot a list of variables shared by different Root samples
# -------------------------------------------------------------------

from Isis.data_management import DataMgr
import Isis.plot_tools as iplt
import Isis.utils as utils
import ROOT as rt

# Maximum number of variables displayed per canvas, so "npads = maxVars + 1"
vars_per_plot = 8

# Maximum number of managers displayed on the same canvas in the correlation plots
mgrs_per_plot = 2

# List of variables to plot
varlst = ['var0', 'var1', 'var2']

# Number of bins and ranges for the variables to be displayed
ranges = {'var0': (100, 0, 10),
          'var1': (100, 0, 10),
          'var2': (100, 0, 10)
}

# Additional cuts
cuts = False

# Dictionary with the name of the managers, path to the root file and
# to the tree inside it
paths = {
    'smp1' : ('smp1.root', 'DecayTree'),
    'smp2' : ('smp2.root', 'DecayTree')
}

# Some functions
def genCorrPlots( mgrdct, varlst, nmgrs = 1, vartitles = [], cuts = False ):
    '''
    Make the correlation plots given a dictionary of managers
    '''
    mgrlst = mgrdct.values()
    
    mgr_sets = [mgrlst[i:(i + nmgrs)] for i in xrange(0, len(mgrlst), nmgrs)]

    if vartitles == []:
        vartitles = varlst

    plots = []
    for i, mset in enumerate(mgr_sets):

        name   = 'CorrCanvas_' + str(i)
        canvas = rt.TCanvas(name, name)
        nyvars, nxvars = iplt.optCanvasDivision(len(mset))
        canvas.Divide(nyvars, nxvars)

        cst = iplt.CanvasStorer(canvas, [], [])
        
        for i, mgr in enumerate(mset):

            canvas.cd(i + 1)
            
            name   = mgr.name + '_CorrHist'
            matrix = mgr.subsample(columns = varlst, cuts = cuts).as_matrix().T
            chist  = iplt.corr_hist(matrix, name = name, vartitles = vartitles)
            
            chist.Draw()
            
            cst.dataObjs.append(chist)

        canvas.Update()

        plots.append(cst)

    return plots


def genVarPlots( mgrdct, varlst, nvars, cuts ):
    '''
    Make the canvases with the variables superimposed
    '''
    var_sets = [varlst[i:(i + nvars)] for i in xrange(0, len(varlst), nvars)]

    plots = []
    for i, vset in enumerate(var_sets):
        cst = iplt.multiplot([mgr for mgr in mgrs.itervalues()], vset,
                             cuts   = cuts,
                             norm   = True,
                             name   = 'VarCanvas_' + str(i),
                             ranges = ranges)
        plots.append(cst)

    return plots


def loadData( paths ):
    '''
    Load the data
    '''
    mgrs = {}
    for mgrname, inputs in paths.iteritems():
        
        fpath, tpath = inputs
        
        mgrs[mgrname] = DataMgr.from_root(fpath, tpath, columns = varlst, name = mgrname)
        
    return mgrs

# Load data
mgrs = loadData(paths)

# Make the canvases
var_plots = genVarPlots(mgrs, varlst, vars_per_plot, cuts = cuts)

# Make the correlation plots
corr_plots = genCorrPlots(mgrs, varlst, nmgrs = mgrs_per_plot, cuts = cuts)
