#!/usr/bin/python
#==============================================================
# Script to load the Isis environment together with Root on a
# python shell
#--------------------------------------------------------------
# AUTHOR: Miguel Ramos Pernas
# e-mail: miguel.ramos.pernas@cern.ch
#==============================================================


def parse_arguments():
    '''
    Parse the arguments. Use "isispy.py -h" for more details
    '''
    import argparse
    parser = argparse.ArgumentParser(description = 'Initialize < isispy.py >')
    parser.add_argument('-a', '--arguments', type=str, default = '',
                        help = 'Arguments for the main script')
    parser.add_argument('-e', '--executable', type = str, default = '',
                        help = 'Executable script')
    parser.add_argument('-l', '--load', type = str, default = '',
                        help = 'Files to load')
    args = parser.parse_args()
    
    load_files(list(filter(None, args.load.split(' '))))
    
    if args.executable:
        execfile(' '.join([args.executable, args.arguments]).strip())
    else:
        from Isis.IBoost.PyGeneral import SendWarningMsg
        if args.arguments:
            SendWarningMsg('Passing arguments without executable; not used')


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
    
    from Isis.IBoost.PyGeneral import SendInfoMsg
    for f in pyfiles:
        SendInfoMsg('Adding new python file: %s' %f)

    glb = globals()

    ''' Add root files '''
    for i, f in enumerate(rtfiles):
        fname = '_rf' + str(i)
        SendInfoMsg('Adding new root file < %s > as: %s' %(f, fname))
        glb[fname] = rt.TFile(f, 'READ')

    ''' Add text-based files '''
    for i, f in enumerate(ttfiles):
        fname = '_df' + str(i)
        SendInfoMsg('Adding new txt file < %s > as: %s' %(f, fname))
        glb[fname] = open(f, 'rt')

    ''' Add binary files '''
    for i, f in enumerate(bnfiles):
        fname = '_bf' + str(i)
        SendInfoMsg('Adding new binary file < %s > as: %s' %(f, fname))
        glb[fname] = open(f, 'rb')


if __name__ == '__main__':
    
    from Isis import *
    
    import ROOT as rt

    print '\n**********************************'
    print '** Initialized Isis environment **'
    print '**********************************\n'
    
    parse_arguments()
