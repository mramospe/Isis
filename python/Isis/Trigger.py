#///////////////////////////////////////////////////////////////////////////////
#//                                                                           //
#//  Python modules                                                           //
#//                                                                           //
#// ------------------------------------------------------------------------- //
#//                                                                           //
#//  AUTHOR: Miguel Ramos Pernas                                              //
#//  e-mail: miguel.ramos.pernas@cern.ch                                      //
#//                                                                           //
#//  Last update: 19/01/2016                                                  //
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
from Isis.Utils import LargestString, StringListFilter
from ROOT import TRandom3


#_______________________________________________________________________________
# Class to perform trigger analysis. It operates over DataManager classes.
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
        msg = "-- Efficiencies for " + mngr.Name + " [ total eff. ] [ partial eff. ] --"
        lm  = len( msg )*"-"
        print lm + "\n" + msg + "\n" + lm
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

    def ApplyTrigger( self, *args ):
        ''' Applies the give trigger to the different DataManager classes in < args >. If
        this is an empty list, then it will work with the signal and minimum bias samples. 
        It also will show the rate. The cuts are removed after applying this method.
        Finally returns the samples cut by this trigger in the same order as the those
        given as input. '''
        if not args:
            args = [ self.SigMngr, self.MiBMngr ]
        self.PrepareTrigger()
        outmngrs = []
        for mngr in args:
            outmngrs.append( self.__call__( mngr ) )
        msg = "-- Minimum bias rate: " + str( self.GetRate() ) + " Hz --"
        lm  = len( msg )*"-"
        print lm + "\n" + msg + "\n" + lm
        return outmngrs

    def BookCut( self, arg, cut = False ):
        ''' If < arg > is a name, and < cut > is its attached cut, it books a new cut with
        the id given by < arg >. Otherwise if < arg > is a variable of the class
        < TriggerVarDict > there will be booked the enabled variables with an id equal to
        the keys of it. '''
        if cut:
            self.Prepared = False
            if arg in self.CutList:
                print "CutID <", arg, "> already used, cut <", cut, "> not booked"
            else:
                self.CutList.append( arg )
                self.Cuts[ arg ] = cut
        else:
            for var in arg:
                vdict = arg[ var ]
                for el in vdict[ "vars" ]:
                    self.BookCut( var + "_" + el, el + vdict[ "cond" ] + str( vdict[ "cut" ] ) )

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

#_______________________________________________________________________________
# Class to store variables to be used by a trigger class. Every variable has
# three different
class TriggerVarDict( dict ):
    
    def __init__( self, vardict = {}, enabled_vars = [] ):
        ''' The class can be constructed giving a dictionary as an argument, containing all
        the variables with the information written with an identical format as that of the
        < AddVariable > method. The variables to be enabled can also be given in a list. By
        default all the variables given in the dictionary are enabled. '''
        if vardict:
            for el in vardict:
                self[ el ] = vardict[ el ]
            if enabled_vars:
                self.EnabledVars = enabled_vars
            else:
                self.EnabledVars = vardict.keys()
        else:
            self.EnabledVars = []
        self.Rndm = TRandom3()

    def __iter__( self ):
        ''' Definition of the iterator. The iteration is made over the enabled variables. '''
        return self.EnabledVars.__iter__()

    def next( self ):
        ''' As in the < __iter__ > method, the iteration is made over the enabled variables. '''
        return self.EnabledVars.next()
        
    def AddVariable( self, name, variables, condition, cut, minimum = False, maximum = False ):
        ''' Adds a new variable to the dictionary. All the variables that are added
        manually have to be written following this format '''
        self[ name ] = {
            "cond": condition,
            "cut" : cut,
            "min" : minimum,
            "max" : maximum,
            "vars": variables
            }
        if name not in self.EnabledVars:
            self.EnabledVars.append( name )

    def IsEnabled( self, name ):
        ''' Checks if the given variable is enabled or not '''
        if name in self.EnabledVars:
            return True
        else:
            return False

    def MakeCut( self, name ):
        ''' Constructs the string with the cut of the variable < name > '''
        vdict = self[ name ]
        cond, value = vdict[ "cond" ], vdict[ "cut" ]
        cut = vdict[ "vars" ][ 0 ] + cond + str( value )
        for el in vdict[ "vars" ][ 1: ]:
            cut += " and " + el + cond + str( value )
        return cut

    def MakeRandomCut( self, name ):
        ''' Returns a random cut for the given variable '''
        vdict = self[ name ]
        value = ( vdict[ "max" ] - vdict[ "min" ] )*self.Rndm.Rndm() + vdict[ "min" ]
        cond = vdict[ "cond" ]
        cut  = vdict[ "vars" ][ 0 ] + cond + str( value )
        for el in vdict[ "vars" ][ 1: ]:
            cut += " and " + el + cond + str( value )
        return cut

    def Print( self, arg = "*", **kwargs ):
        ''' Displays the information of the current dictionary. In < arg > a list of variables can
        be given, or a string containing the pattern that follow the variables to be used. If the
        option < enabled > is set to false, all the variables in the class will be displayed. '''
        if "enabled" in kwargs: enabled = kwargs[ "enabled" ]
        else: enabled = False
        if arg == "*":
            if enabled:
                arg = [ var for var in self ]
            else:
                arg = self.keys()
        elif isinstance( arg, str ):
            if enabled:
                arg = StringListFilter( [ var for var in self ], arg )
            else:
                arg = StringListFilter( self.keys(), arg )
        arg.sort()
        gname = max( [ len( name ) for name in arg ] )
        if gname < 4: gname = 4
        gvars = max( len( str( self[ var ][ "vars" ] ) ) for var  in arg )
        if gvars < 9: gvars = 9
        gcut = max( len( str( self[ var ][ "cut" ] ) ) for var in arg )
        if gcut < 3: gcut = 3
        gmin = max( len( str( self[ var ][ "min" ] ) ) for var in arg )
        if gmin < 4: gmin = 4
        gmax = max( len( str( self[ var ][ "max" ] ) ) for var in arg )
        if gmax < 4: gmax = 4
        separator = ( gname + gvars + 5 + gcut + gmin + gmax + 21 )*"-"
        lengths = [ gvars      , 5      , gcut , gmin  , gmax   ]
        prorder = [ "vars"     , "cond" , "cut", "min" , "max"  ]
        outlst  = [ "Variables", "Cond.", "Cut", "Min.", "Max." ]
        prout   = 3*" " + "Name" + ( gname - 4 )*" "
        for el, lgth in zip( outlst, lengths):
            prout += 3*" " + el + ( lgth - len( el ) )*" "
        print separator + "\n" + prout + "\n" + separator
        for var in arg:
            vdict = self[ var ]
            prout = 3*" " + var + ( gname - len( var ) )*" "
            for el, lgth in zip( prorder, lengths ):
                prop = str( vdict[ el ] )
                nwsp = lgth - len( prop )
                if el == "cond":
                    prout += 5*" " + prop + ( nwsp - 2 )*" "
                else:
                    prout += 3*" " + prop + nwsp*" "
            print prout
        print separator

    def SetEnabledVars( self, *args ):
        ''' Sets the list of enabled variables to perform the different operations '''
        self.EnabledVars = args

    def SetVarStatus( self, name, status ):
        ''' Sets the status of the variable < name > to the value given '''
        if status == True:
            if not name in self.EnabledVars:
                self.EnabledVars.append( name )
        else:
            if name in self.EnabledVars:
                self.EnabledVars.remove( name )
