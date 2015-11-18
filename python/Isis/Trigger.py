#///////////////////////////////////////////////////////////////////////////////
#//                                                                           //
#//  Python modules                                                           //
#//                                                                           //
#// ------------------------------------------------------------------------- //
#//                                                                           //
#//  AUTHOR: Miguel Ramos Pernas                                              //
#//  e-mail: miguel.ramos.pernas@cern.ch                                      //
#//                                                                           //
#//  Last update: 18/11/2015                                                  //
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


from Isis.DataManagement import DataManager
from Isis.Utils import LargestString
from ROOT import TGraph


#_______________________________________________________________________________
# Class Trigger
class Trigger:

    def __init__( self, mib_rate, **kwargs ):
        ''' Constructor given the minimum bias rate. Optionally there can be given the
        number of minimum bias events as well as the < event > variable, in order to
        get the proper value of the rate. If this last argument is not specified, it
        will be taken as the number of events in the associated DataManager class. If 
        < EvtVar > is not specified, the efficiencies are computated with the number
        of events in the managers. '''

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
        
    def __call__( self, mngr ):
        ''' Defines the class method to evaluate the efficiency of the cut over a given 
        manager. All cuts specified after using the BookCut and ApplyCuts methods are
        going to be applied. '''
        old_nevts = self.GetTrueEvents( mngr )
        new_nevts = old_nevts
        strsize   = LargestString( self.Cuts )
        for cut in self.CutList:
            cut       = self.Cuts[ cut ]
            toceff    = new_nevts
            mngr      = mngr.CutSample( cut )
            new_nevts = self.GetTrueEvents( mngr )
            ''' This print corresponds to the cut, the efficiency of the trigger
            after it and the efficiency of the cut '''
            nwsp = strsize - len( cut )
            print cut, nwsp*" " + "=>", new_nevts*1./old_nevts, "(", new_nevts*100./toceff, "% )"
        return mngr

    def AddManager( self, mngr, dtype ):
        ''' Adds a new manager to the class. The type has to be specified. If it is a
        minimum bias sample, it will auto-merge the sets stored. In other case the
        manager will be storaged as a signal file. '''
        self.Prepared = False
        if   dtype in ( "mib", "MiB", "MIB" ):
            if self.MiBMngr:
                self.MiBMngr += mngr
            else:
                self.MiBMngr  = mngr
            if self.SelfNorm :
                self.nMiBevts += len( mngr )
        else:
            self.SigMngr[ dtype ] = mngr

    def BookCut( self, cut_id, cut ):
        ''' Books a new cut with the id given by < cut_id > '''
        self.Prepared = False
        if cut_id in self.CutList:
            print "CutID <", cut_id, "> already used, cut <", cut, "> not booked"
        else:
            self.CutList.append( cut_id )
            self.Cuts[ cut_id ] = cut

    def GetCutLine( self ):
        ''' Gets all the cuts. Cuts located in < self.Cuts > will be concatenated with an 
        < and > '''
        cut = ""
        for cut_id in self.CutList:
            cut += self.Cuts[ cut_id ] + " and "
        cut = cut[ :-5 ]
        return cut

    def GetEfficiency( self, arg, dtype ):
        ''' Returns the efficiency for signal or minimum bias samples. This quantity is refered
        to the total number of events in the main sample. The input parameter < arg > can be
        a cut string or a list of events. '''
        if dtype in ( "mib", "MiB", "MIB" ):
            cmngr = self.CutMiBMngr
            mngr  = self.MiBMngr
        else:
            cmngr = self.CutSigMngr[ dtype ]
            mngr  = self.SigMngr[ dtype ]
        if isinstance( arg, str ):
            arg = cmngr.GetCutList( arg )
        return self.GetTrueEvents( cmngr, arg )*1./self.GetTrueEvents( mngr )

    def GetRate( self, arg = False ):
        ''' Gets the rate for a given cut string or list '''
        if isinstance( arg, str ):
            arg = self.CutMiBMngr.GetCutList( arg )
        if not arg:
            arg = self.CutMiBMngr.GetCutList( self.GetCutLine() )
        return self.MiBrate*self.GetTrueEvents( self.CutMiBMngr, arg )*1./self.nMiBevts

    def GetRejection( self, arg, dtype = "mib" ):
        ''' Gets the rejection for a given cut string or list '''
        return 1. - self.GetEfficiency( arg, dtype )

    def GetTrueEvents( self, mngr, lst = False ):
        ''' Method to get the number of true events in a given manager. If a list of
        events is specified this task is only performed on them. '''
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

    def PrepareTrigger( self ):
        ''' Applies the cuts booked in the class ( if they exist ), and prepares the
        managers to work with. This method creates two new managers, conserving the
        old ones. '''
        if self.Prepared: return
        cut = self.GetCutLine()
        if cut == "":
            self.CutMiBMngr = self.MiBMngr.Copy()
            for kw in self.SigMngr:
                self.CutSigMngr[ kw ] = self.SigMngr[ kw ].Copy()
        else:
            self.CutMiBMngr = self.MiBMngr.CutSample( cut )
            for kw in self.SigMngr:
                self.CutSigMngr[ kw ] = self.SigMngr[ kw ].CutSample( cut )
        self.Prepared = True
        print "Trigger prepared"

    def RemoveCuts( self, *args ):
        ''' Removes a cut booked in the class. After using this method, ApplyCuts has to
        be called again in order to generate the cut samples. '''
        self.Prepared = False
        if '*' in args:
            self.CutList = []
            self.Cuts    = {}
        else:
            for cut_id in args:
                del self.CutList[ self.CutList.index( cut_id ) ]
                del self.Cuts[ cut_id ]

    def ScanVariable( self, varlst, cond, first, last, npoints ):
        ''' Performs a scan over the variable < var > from < first > to < last > in
        < npoints > steps. Both end-points are included. The number of entries for
        the signal, minimum bias, cut and rate are returned in a dictionary, together
        with the initial number of minimum bias and signal events. '''
        if not self.Prepared: self.PrepareTrigger()
        step    = ( last - first )*1./( npoints - 1 )
        results = { "oldnmib" : self.GetTrueEvents( self.MiBMngr ),
                    "cut"     : npoints*[ 0 ],
                    "nmib"    : npoints*[ 0 ],
                    "rate"    : npoints*[ 0 ] }
        ''' Defines the lambda function to perform the comparison '''
        if   cond == '<' : func = lambda x: x < cut
        elif cond == '>' : func = lambda x: x > cut
        elif cond == '<=': func = lambda x: x <= cut
        elif cond == '>=': func = lambda x: x >= cut
        ''' If only a variable is provided, it makes a list with it '''
        if not type( varlst ) in ( list, tuple ):
            varlst = [ varlst ]
        for el in self.SigMngr:
            results[ "n"    + el ] = npoints*[ 0 ]
            results[ "oldn" + el ] = len( self.SigMngr[ el ] )
        ''' Performs the loop over all the cuts with the information provided '''
        sigmngrs = [ self.CutSigMngr[ el ] for el in self.CutSigMngr ]
        for i in range( npoints ):
            cut      = first + i*step
            lenlst   = len( sigmngrs )*[ 0 ]
            for j, mngr in enumerate( sigmngrs ):
                varvals  = [ mngr.Variables[ var ] for var in varlst ]
                condlist = zip( *[ map( func, vals ) for vals in varvals ] )
                for ievt in range( mngr.Nentries ):
                    if all( condlist[ ievt ] ):
                        lenlst[ j ] += 1
            varvals  = [ self.CutMiBMngr.Variables[ var ] for var in varlst ]
            condlist = zip( *[ map( func, vals ) for vals in varvals ] )
            nmib     = self.GetTrueEvents( self.CutMiBMngr,
                                               [ ievt for ievt, el in enumerate( condlist ) 
                                                 if all( el ) ] )
            results[ "cut"  ][ i ] = cut
            results[ "nmib" ][ i ] = nmib
            results[ "rate" ][ i ] = self.MiBrate*nmib*1./self.nMiBevts
            for imngr, el in enumerate( self.CutSigMngr.keys() ):
                results[ "n" + el ][ i ] = lenlst[ imngr ]
        return results

    def SetMiBevts( self, val ):
        ''' Sets the number of minimum bias events '''
        self.Prepared = False
        if val != 0:
            self.SelfNorm = False
            self.nMiBevts = val
        else:
            self.SelfNorm = True
            self.nMiBevts = self.GetTrueEvents( self.MiBMngr )

    def SetMiBrate( self, val ):
        ''' Sets the minimum bias rate '''
        self.Prepared = False
        self.MiBrate  = val
