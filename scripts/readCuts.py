# ----------------------------------------------------------------------
# Script to display the cuts booked in a file using the CutManager class
# ----------------------------------------------------------------------

from Isis.IBoost.PyGeneral import CutManager, sendErrorMsg

import sys

# Check the input arguments
nargs = len(sys.argv)
if nargs < 2:
    sendErrorMsg('The input parameters are: < file name > < cut1 > < cut2 >...')
elif nargs == 2:
    sendErrorMsg('At least one cut name must be provided')
else:
    fname = sys.argv[1]
    print 'Reading cuts from:', fname

    cuts = sys.argv[2:]

    # Get the size of the cut names (to format the output message)
    strsize  = map(len, cuts)
    maxsize  = max(strsize)
    nwhitesp = map(lambda x: maxsize - x, strsize)

    # Read the cuts and display them
    cutmgr = CutManager(fname)
    for c, n in zip(cuts, nwhitesp):
        ctstr = cutmgr.getCut(c)
        print ' - %s' %c, n*' ', '=>', ctstr
