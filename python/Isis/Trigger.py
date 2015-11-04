#///////////////////////////////////////////////////////////////////////////////
#//                                                                           //
#//  Python modules                                                           //
#//                                                                           //
#// ------------------------------------------------------------------------- //
#//                                                                           //
#//  AUTHOR: Miguel Ramos Pernas                                              //
#//  e-mail: miguel.ramos.pernas@cern.ch                                      //
#//                                                                           //
#//  Last update: 04/11/2015                                                  //
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
        self.SigMngr    = {}
        self.MiBMngr    = False
        self.CutSigMngr = {}
        self.CutMiBMngr = False
        self.MiBrate    = mib_rate
        self.Prepared   = False

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
        old_nevts = self.GetTrueEvents( mngr )
        for cut in self.CutList:
            cut       = self.Cuts[ cut ]
            mngr      = mngr.CutSample( cut )
            new_nevts = self.GetTrueEvents( mngr )
            print "Efficiency after cut:", cut, "=>", new_nevts*1./old_nevts
        return mngr

    #_______________________________________________________________________________
    # Internal method to get the list of events that pass a certain cut during a scan
    def get_scan_cut( self, var, cond, cut ):
        lists = []
        sigmngrs = [ self.CutSigMngr[ el ] for el in self.CutSigMngr ]
        for mngr in sigmngrs + [ self.CutMiBMngr ]:
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
    # Adds a new manager to the class. The type has to be specified. If it is a
    # minimum bias sample, it will auto-merge the sets stored. In other case the
    # manager will be storaged as a signal file.
    def AddManager( self, mngr, dtype ):
        if   dtype in ( "mib", "MiB", "MIB" ):
            if self.MiBMngr:
                self.MiBMngr += mngr
            else:
                self.MiBMngr  = mngr
            if self.SelfNorm :
                self.nMiBevts += len( mngr )
        else:
            self.SigMngr[ dtype ] = mngr

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
    def GetEfficiency( self, arg, dtype ):
        if dtype in ( "mib", "MiB", "MIB" ):
            cmngr = self.CutMiBMngr
            mngr  = self.MiBMngr
        else:
            cmngr = self.CutSigMngr[ dtype ]
            mngr  = self.SigMngr[ dtype ]
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
            for el in self.SigMngr:
                self.CutSigMngr[ el ] = self.SigMngr[ el ].Copy()
        else:
            self.CutMiBMngr = self.MiBMngr.CutSample( cut )
            for el in self.SigMngr:
                self.CutSigMngr[ el ] = self.SigMngr[ el ].CutSample( cut )
        self.Prepared = True
        print "Trigger prepared"

    #_______________________________________________________________________________
    # Removes a cut booked in the class. After using this method, ApplyCuts has to
    # be called again in order to generate the cut samples.
    def RemoveCut( self, cut_id ):
        self.CutList.remove( cut_id )
        del self.Cuts[ cut_id ]

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
    # Performs a scan over the variable < var > from < first > to < last > in
    # < npoints > steps. Both end-points are included. The number of entries for
    # the signal, minimum bias, cut and rate are returned in a dictionary, together
    # with the initial number of minimum bias and signal events.
    def ScanVariable( self, var, cond, first, last, npoints ):
        if not self.Prepared: self.PrepareTrigger()
        step    = ( last - first )*1./( npoints - 1 )
        results = { "oldnmib" : self.GetTrueEvents( self.MiBMngr ),
                    "cut"     : npoints*[ 0 ],
                    "nmib"    : npoints*[ 0 ],
                    "rate"    : npoints*[ 0 ] }
        for el in self.SigMngr:
            results[ "n"    + el ] = npoints*[ 0 ]
            results[ "oldn" + el ] = [ self.GetTrueEvents( self.SigMngr[ el ] ) for el in self.SigMngr ]
        for i in range( npoints ):
            cut = first + i*step
            sigmiblist = self.get_scan_cut( var, cond, cut )
            nmib = self.GetTrueEvents( self.CutMiBMngr, sigmiblist[ -1 ] )
            results[ "cut"  ][ i ] = cut
            results[ "nmib" ][ i ] = nmib
            results[ "rate" ][ i ] = self.MiBrate*nmib*1./self.nMiBevts
            for el, imngr in zip( self.CutSigMngr, range( len( self.CutSigMngr ) ) ):
                results[ "n" + el ][ i ] = sigmiblist[ imngr ]
        return results
