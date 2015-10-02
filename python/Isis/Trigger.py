#///////////////////////////////////////////////////////////////////////////////
#//                                                                           //
#//  Python modules                                                           //
#//                                                                           //
#// ------------------------------------------------------------------------- //
#//                                                                           //
#//  AUTHOR: Miguel Ramos Pernas                                              //
#//  e-mail: miguel.ramos.pernas@cern.ch                                      //
#//                                                                           //
#//  Last update: 28/09/2015                                                  //
#//                                                                           //
#// --------------------------------------------------------------------------//
#//                                                                           //
#//  Description:                                                             //
#//                                                                           //
#//  Class to perform trigger analysis. This class uses DataManager class to  //
#//  administrate the data for signal and minimum bias events.                //
#//                                                                           //
#// ------------------------------------------------------------------------- //
#///////////////////////////////////////////////////////////////////////////////


from Isis.DataManager import DataManager
from ROOT import TGraph


#_______________________________________________________________________________
# Class Trigger
class Trigger:

    #_______________________________________________________________________________
    # Constructor given the minimum bias rate. Optionally there can be given the
    # number of minimum bias events as well as the < event > variable, in order to
    # get the proper value of the rate. If this last argument is not specified, it
    # will be taken as the number of events in the associated DataManager class. If 
    # < EvtVar > is not specified, the efficiencies are computated with the number
    # of events in the managers.
    def __init__( self, mib_rate, **kwargs ):

        for kw in kwargs:
            setattr( self, kw, kwargs[ kw ] )

        self.Cuts       = {}
        self.CutList    = []
        self.SigMngr    = False
        self.MiBMngr    = False
        self.CutSigMngr = False
        self.CutMiBMngr = False
        self.FuncList   = []
        self.MiBrate    = mib_rate

        if "nMiBevts" not in kwargs:
            self.nMiBevts = 0
        if "EvtVar" not in kwargs:
            self.EvtVar = False

        if self.nMiBevts:
            self.SelfNorm = False
        else:
            self.SelfNorm = True
        
    #_______________________________________________________________________________
    # Defines the class method to evaluate the efficiency of the cut over a given 
    # manager. All cuts specified after using the BookCut and ApplyCuts methods are
    # going to be applied.
    def __call__( self, mngr ):
        total_cut = ""
        old_nevts = self.GetTrueEvents( mngr )
        for cut in self.CutList:
            total_cut += cut
            new_mngr  = mngr.CutSample( total_cut )
            new_nevts = self.GetTrueEvents( new_mngr )
            print " Efficiency after cut:", cut, "=>", new_nevts*1./old_nevts
        return new_mngr

    #_______________________________________________________________________________
    # Internal method to get the list of events that pass a certain cut during a scan
    def get_scan_cut( self, var, cond, cut ):
        lists = []
        for mngr in [ self.CutSigMngr, self.CutMiBMngr ]:
            var_vals = mngr.Variables[ var ]
            add_list = []
            if   cond == '<':
                for ievt in range( mngr.Nentries ):
                    if var_vals[ ievt ] < cut:
                        add_list.append( ievt )
            elif cond == '>':
                for ievt in range( mngr.Nentries ):
                    if var_vals[ ievt ] > cut:
                        add_list.append( ievt )
            elif cond == '<=':
                for ievt in range( mngr.Nentries ):
                    if var_vals[ ievt ] <= cut:
                        add_list.append( ievt )
            elif cond == '>=':
                for ievt in range( mngr.Nentries ):
                    if var_vals[ ievt ] >= cut:
                        add_list.append( ievt )
            lists.append( add_list )
        return lists

    #_______________________________________________________________________________
    # Adds a new manager to the class. The type has to be specified and it will
    # auto-merge the sets stored.
    def AddManager( self, mngr, dtype ):
        if   dtype in ( "mib", "MiB", "MIB" ):
            if self.MiBMngr:
                self.MiBMngr += mngr
            else:
                self.MiBMngr  = mngr
            if self.SelfNorm :
                self.nMiBevts += len( mngr )
        elif dtype in ( "sig", "Sig", "SIG" ):
            if self.SigMngr:
                self.SigMngr += mngr
            else:
                self.SigMngr  = mngr
        else:
            print " Error when loading manager"

    #_______________________________________________________________________________
    # Books a new cut with the id given by < cut_id >
    def BookCut( self, cut_id, cut ):
        self.CutList.append( cut_id )
        self.Cuts[ cut_id ] = cut

    #_______________________________________________________________________________
    # Gets all the cuts. Cuts located in < self.Cuts > will be concatenated with an 
    # < and >
    def GetCutLine( self ):
        cut = ""
        for cut_id in self.CutList:
            cut += self.Cuts[ cut_id ] + " and "
        cut = cut[ :-5 ]
        return cut

    #_______________________________________________________________________________
    # Returns the efficiency for signal or minimum bias samples. This quantity is refered
    # to the total number of events in the main sample. The input parameter < arg > can be
    # a cut string or a list of events.
    def GetEfficiency( self, arg, dtype = "sig" ):
        if   dtype in ( "mib", "MiB", "MIB" ):
            cmngr = self.CutMiBMngr
            mngr  = self.MiBMngr
        elif dtype in ( "sig", "Sig", "SIG" ):
            cmngr = self.CutSigMngr
            mngr  = self.SigMngr
        if isinstance( arg, str ):
            arg = cmngr.GetCutList( arg )
        return self.GetTrueEvents( cmngr, arg )*1./self.GetTrueEvents( mngr )

    #_______________________________________________________________________________
    # Gets the rate for a given cut string or list
    def GetRate( self, arg = False ):
        if isinstance( arg, str ):
            arg = self.CutMiBMngr.GetCutList( arg )
        return self.MiBrate*self.GetTrueEvents( self.CutMiBMngr, arg )*1./self.nMiBevts

    #_______________________________________________________________________________
    # Gets the rejection for a given cut string or list
    def GetRejection( self, arg, dtype = "mib" ):
        return 1. - self.GetEfficiency( arg, dtype )

    #_______________________________________________________________________________
    # Method to get the number of true events in a given manager. If a list of
    # events is specified this task is only performed on them.
    def GetTrueEvents( self, mngr, lst = False ):
        if self.EvtVar:
            event_list = mngr.Variables[ self.EvtVar ]
            ''' If < lst > is a void list it is maintained as it is '''
            if lst == False:
                lst = range( mngr.Nentries )
            event_list = [ event_list[ i ] for i in lst ]
            evts       = len( set( event_list ) )
            return evts
        else:
            ''' Same as above '''
            if lst == False:
                return mngr.Nentries
            else:
                return len( lst )

    #_______________________________________________________________________________
    # Applies the cuts booked in the class ( if they exist ), and prepares the
    # managers to work with. This method creates two new managers, conserving the
    # old ones.
    def PrepareTrigger( self ):
        cut = self.GetCutLine()
        if cut == "":
            self.CutMiBMngr = self.MiBMngr.Copy()
            self.CutSigMngr = self.SigMngr.Copy()
        else:
            self.CutMiBMngr = self.MiBMngr.CutSample( cut )
            self.CutSigMngr = self.SigMngr.CutSample( cut )

    #_______________________________________________________________________________
    # Removes a cut booked in the class. After using this method, ApplyCuts has to
    # be called again in order to generate the cut samples.
    def RemoveCut( self, cut_id ):
        self.CutList.remove( cut_id )
        del self.Cuts[ cut_id ]

    #_______________________________________________________________________________
    # Performs a scan over the variable < var > from < first > to < last > in
    # < npoints > steps. Both end-points are included. The task performed during the
    # scan is defined by the functions storaged in < self.FuncList > using the
    # method < SetScanFunctions >. One can add a new function to this class ( see 
    # its structure requirements in the method description ). The graphs are
    # returned in the same order as the functions are booked.
    def ScanVariable( self, var, cond, first, last, npoints ):
        step    = ( last - first )*1./( npoints - 1 )
        graphs  = [ TGraph() for func in self.FuncList ]
        grfuncs = zip( self.FuncList, graphs )
        nsig    = self.GetTrueEvents( self.SigMngr )
        nmib    = self.GetTrueEvents( self.MiBMngr )
        for i in range( npoints ):
            cut_val   = first + i*step
            siglist, miblist = self.get_scan_cut( var, cond, cut_val )
            sig_eff   = self.GetTrueEvents( self.CutSigMngr, siglist )*1./nsig
            true_nmib = self.GetTrueEvents( self.CutMiBMngr, miblist )
            mib_eff   = true_nmib*1./nmib
            rate      = self.MiBrate*true_nmib*1./self.nMiBevts
            print true_nmib
            for func, graph in grfuncs:
                func( graph, i, cut_val, sig_eff, mib_eff, rate )
        return graphs

    #_______________________________________________________________________________
    # Definition of the different functions that can be booked in the class for the
    # scans
    def ROC_point( self, graph, ipoint, *args ):
        graph.SetPoint( ipoint, 1. - args[ 2 ], args[ 1 ] )
    def MiBVScut_point( self, graph, ipoint, *args ):
        graph.SetPoint( ipoint, args[ 0 ], 1. - args[ 2 ] )
    def SigVScut_point( self, graph, ipoint, *args ):
        graph.SetPoint( ipoint, args[ 0 ], args[ 1 ] )
    def SigVSrate_point( self, graph, ipoint, *args ):
        graph.SetPoint( ipoint, args[ 3 ], args[ 1 ] )
    def RateVScut_point( self, graph, ipoint, *args ):
        graph.SetPoint( ipoint, args[ 0 ], args[ 3 ] )

    #_______________________________________________________________________________
    # Sets the number of minimum bias events
    def SetMiBevts( self, val ):
        if val != 0:
            self.SelfNorm = False
            self.nMiBevts = val
        else:
            self.SelfNorm = True
            self.nMiBevts = self.GetTrueEvents( self.MiBMngr )

    #_______________________________________________________________________________
    # Sets the minimum bias rate
    def SetMiBrate( self, val ):
        self.MiBrate = val

    #_______________________________________________________________________________
    # Sets the scan functions. Together with the default functions specified here:
    # ROC, MiBVScut, SigVScut, ... one can add a user-defined function if it takes
    # as arguments six parameters: a TGraph, number of the point, cut value, signal
    # efficiency, minimum bias efficiency and the rate. This function has to set a
    # new point to the TGraph. The graphs are returned in the same order as they
    # are booked.
    def SetScanFunctions( self, *args ):
        self.FuncList = []
        for func in args:
            if isinstance( func, str ):
                if   func == "ROC":
                    self.FuncList.append( self.ROC_point )
                elif func == "MiBVScut":
                    self.FuncList.append( self.MiBVScut_point )
                elif func == "SigVScut":
                    self.FuncList.append( self.SigVScut_point )
                elif func == "SigVSrate":
                    self.FuncList.append( self.SigVSrate_point )
                elif func == "RateVScut":
                    self.FuncList.append( self.RateVScut_point )
                else:
                    print " Graph", func, "not valid"
                    print " Available graph functions are:"
                    print " ROC, MiBVScut, SigVScut, MiBVSrate, SigVSrate, RateVScut"
                    exit()
            else:
                self.FuncList.append( func )
