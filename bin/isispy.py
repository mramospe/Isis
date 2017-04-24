#!/usr/bin/python
#==============================================================
# Script to load the Isis environment together with Root on a
# python shell
#--------------------------------------------------------------
# AUTHOR: Miguel Ramos Pernas
# e-mail: miguel.ramos.pernas@cern.ch
#==============================================================


def browser():
    '''
    Return a Root TBrowser object. Root has already been imported.
    '''
    return rt.TBrowser()
    
    
def initialize():
    '''
    Initialize the environment
    '''
    
    ''' Store the current global variables '''
    from Isis.utils import EnvTracker
    tracker = EnvTracker(globals(), "global")
    
    args = parse_arguments()

    ''' Load the specified files '''
    load_files(args.load)

    print '* The environment is set *'
    print '**************************'
    
    ''' Execute the script provided '''
    if args.executable:
        execfile(args.executable, globals())
        
    if args.show_globals:
        ''' Print the new global variables added to the shell '''
        tracker.display()
        

def load_files( files ):
    '''
    Load the given list of files and export them to the global scope
    '''
    
    def make_filter( *sfx ):
        ''' Thin filter given the suffix of the files '''
        return lambda el: any(el.endswith(s) for s in sfx)
    
    pyfiles = filter(make_filter('.py'), files)
    rtfiles = filter(make_filter('.root'), files)
    ttfiles = filter(make_filter('.dat', '.txt'), files)
    bnfiles = set(files).difference(pyfiles + rtfiles + ttfiles)
    
    import ROOT as rt
    
    glb = globals()

    if any([rtfiles, ttfiles, bnfiles, pyfiles]):
        print '* Loading input files'
    
    ''' Add root files '''
    for i, f in enumerate(rtfiles):
        fname = '_rf' + str(i)
        print '* - Adding new root file < %s > as: %s' %(f, fname)
        glb[fname] = rt.TFile(f, 'READ')

    ''' Add text-based files '''
    for i, f in enumerate(ttfiles):
        fname = '_df' + str(i)
        print '* - Adding new txt file < %s > as: %s' %(f, fname)
        glb[fname] = open(f, 'rt')

    ''' Add binary files '''
    for i, f in enumerate(bnfiles):
        fname = '_bf' + str(i)
        print '* - Adding new binary file < %s > as: %s' %(f, fname)
        glb[fname] = open(f, 'rb')

    ''' Execute python files '''
    for f in pyfiles:
        print '* - Executing python file: %s' %f
        execfile(f, glb)


def parse_arguments():
    '''
    Parse the arguments. Use "isispy.py -h" for more details. As input it takes a
    filtering function to avoid the built-in variables.
    '''
    import argparse
    parser = argparse.ArgumentParser(description = 'Initialize < isispy.py >')
    
    parser.add_argument('files', type = str, default = '', nargs = '*',
                        help = 'Files/arguments to be parsed if no other option is '\
                        'specified')
    
    parser.add_argument('-a', '--arguments', type = str, default = '', nargs = '+',
                        help = 'Arguments for the main script')

    parser.add_argument('-e', '--executable', type = str, default = '',
                        help = 'Executable script')

    parser.add_argument('-l', '--load', type = str, default = '', nargs = '+',
                        help = 'Files to load')

    parser.add_argument('-g', '--show_globals', default = False, action = 'store_true',
                        help = 'Show or hide the new global variables added to the '\
                        'python shell after the main call')

    args = parser.parse_args()
    
    ''' If no options are specified, builds them using the argument < files > '''

    if not args.executable and args.files:

        preex = args.files[0]
        
        if preex.endswith('.py'):
            args.executable = preex
            args.arguments  = args.files[1:]
        else:
            args.load = args.files

    elif args.files:
        
        args.load = args.files

        
    ''' The arguments are replaced by those specified by the user '''
    import sys
    sys.argv = []
    if args.executable:
        sys.argv.append(args.executable)
    sys.argv += args.arguments

    return args


if __name__ == '__main__':

    ''' Startup message '''
    from Isis.iboost.general import ISIS_VERSION
    start = '* Setting up the Isis environment (v%s) *' %ISIS_VERSION
    print len(start)*'*'
    print start
    
    ''' The imports must be written here since otherwise ROOT complains '''

    import Isis
    
    from Isis import *
    
    print '* Imports to the python shell'
    
    print '* - All packages and modules from the Isis project'

    import ROOT as rt
    print '* - Set ROOT as < rt >'    

    import numpy as np
    print '* - Set numpy as < np >'
    
    import scipy as sc
    print '* - Set scipy as < sc >'
    
    initialize()
