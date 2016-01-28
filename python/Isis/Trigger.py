#///////////////////////////////////////////////////////////////////////////////
#//                                                                           //
#//  Python modules                                                           //
#//                                                                           //
#// ------------------------------------------------------------------------- //
#//                                                                           //
#//  AUTHOR: Miguel Ramos Pernas                                              //
#//  e-mail: miguel.ramos.pernas@cern.ch                                      //
#//                                                                           //
#//  Last update: 28/01/2016                                                  //
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
from ROOT import TCanvas, TLine, TMarker, TRandom3, kBlue, kRed
from math import sqrt


#_______________________________________________________________________________
# This function set the cuts of a given trigger, so they minimize the ROC
# distances obtained with the signal and minimum bias samples. For each loop,
# a scan over all the variables enabled in < vardict > is performed, selecting
# that who generates the best ROC performance. Afterwards the cut is made and
# the process is repeated till the rate is smaller than the maximum allowed, or
# since the maximum number of loops is reached ( in this case a warning message
# will appear ). A total number of < npoints > will be computed to make the ROC
# curves. The maximum number of loops is specified via < maxloop >. If the
# < display > option is set to True, every time a ROC distance minimization is
# done, the output canvas will show the curve. If a file is given in the
# < save > argument, the graphs will be saved on it.
def MinimizeROCdistances( trigger, sigsamples, vardict, maxrate, **kwargs ):
    if "display" in kwargs: display = kwargs[ "display" ]
    else: display = False
    if "maxloop" in kwargs: maxloop = kwargs[ "maxloop" ]
    else: maxloop = 10
    if "npoints" in kwargs: npoints = kwargs[ "npoints" ]
    else: npoints = 100
    if "save" in kwargs:
        save = kwargs[ "save" ]
        save.cd()
    else:
        save = False
    ''' Since the calculations are made using the squared distance the weights are also squared '''
    if "weights" in kwargs: weights = [ el**2 for el in kwargs[ "weights" ] ]
    else: weigths = len( sigsamples )*[ 1. ]

    sigzip = zip( sigsamples, weights )

    print "\n**************************************************"
    print "*** Starting ROC distance minimization process ***"
    print "**************************************************"
    print "Infomation:"
    print "- Signal managers:", [ trigger.SigMngr[ mgr ].Name for mgr in trigger.SigMngr ]
    print "- Minimum bias manager:", trigger.MiBMngr.Name
    if save:
        print "- Results will be written in file <", save.GetName(), ">"
    
    trigger.PrepareTrigger()
    if display:
        canvas = TCanvas( "ROC", "ROC" )
    makegraph = display or save
    nmib      = trigger.GetTrueEvents( trigger.MiBMngr )

    ''' Works only with the enabled variables in the dictionary '''
    variables = [ var for var in vardict ]
    variables.sort()
    toteff, totrej = 1, 0
    print "- Working variables:", variables
    for iloop in xrange( maxloop ):
        bestdst2, cut, cvar = 1, 0, ""
        trigger.PrepareTrigger()

        ''' If the rate obtained is smaller than the maximum allowed displays the trigger
        information and returns it '''
        if trigger.GetRate() < maxrate:
            print "\nSuccessful process"
            trigger.Print()
            return trigger

        print "______________________" + len( str( iloop ) )*"_"
        print "*** Starting loop", iloop + 1, "***"

        for var in variables:
            vdict = vardict[ var ]
            lists = trigger.ScanVariable( vdict[ "vars" ],
                                          vdict[ "cond" ],
                                          vdict[ "min" ],
                                          vdict[ "max" ],
                                          npoints )

            ''' Calculates the normalized rejections for the minimum bias '''
            rej    = [ el*1./nmib for el in lists[ "nmib" ] ]
            maxrej = max( rej )
            if maxrej == 0:
                print "Cuts on variable <", var, "> give a null rejection. Removed from list."
                variables.remove( var )
                continue
            rej = [ 1. - el/maxrej for el in rej ]

            ''' Calculates the normalized efficiencies for each sample '''
            for smp, w2 in sigzip:
                nsig   = trigger.SigMngr[ smp ].Nentries
                eff    = [ el*1./nsig for el in lists[ "n" + smp ] ]
                maxeff = max( eff )
                eff    = [ el/maxeff for el in eff ]
                dst2   = w2
                for i, ( ieff, irej ) in enumerate( zip( eff, rej ) ):
                    newdst2 = w2*( ( 1. - ieff )**2 + ( 1. - irej )**2 )
                    if newdst2 < dst2:
                        index, dst2 = i, newdst2
                if dst2 < bestdst2:
                    cvar  = var
                    value = vdict[ "min" ] + index*( vdict[ "max" ] -
                                                     vdict[ "min" ] )*1./( len( eff ) - 1 )
                    besteff, bestrej, bestdst2 = eff[ index ], rej[ index ], dst2
                    print "New smallest squared distance ( %s ):" % cvar, bestdst2
                    if makegraph:
                        graph = ( rej, eff, smp )

        ''' Constructs the cut to be applied '''
        vdict = vardict[ cvar ]
        cond = vdict[ "cond" ]
        cut  = vdict[ "vars" ][ 0 ] + cond + str( value )
        for el in vdict[ "vars" ][ 1: ]:
            cut += " and " + el + cond + str( value )
        if cvar in trigger.CutList:
            trigger.RemoveCuts( cvar )
        trigger.BookCut( cvar, cut )
        toteff *= besteff
        totrej += bestrej*( 1. - totrej )
        print "== Results =="
        print "- Cut variable:", cvar
        print "- Cut line:    ", cut
        print "- Cut results [ total ] [ partial ]:"
        print "   Efficiencies:", toteff, "(", besteff, ")"
        print "   Rejections:  ", totrej, "(", bestrej, ")"
        print "   ROC distance:", sqrt( bestdst2 )
        print "   Rate:        ", trigger.GetRate()
        ''' Depending on the specified options for the graphs and the canvas it displays and/or
        saves them '''
        if makegraph:
            graph = MakeScatterPlot( graph[ 0 ],
                                     graph[ 1 ],
                                     xtitle = "Minimum bias rejection",
                                     ytitle = "Signal efficiency ( " + graph[ 2 ] + " )" )
            graph.SetTitle( cvar )
            graph.GetXaxis().SetRangeUser( 0, 1 )
            graph.GetYaxis().SetRangeUser( 0, 1 )
            if display:
                if not canvas:
                    canvas = TCanvas( "ROC", "ROC" )
                marker = TMarker( bestrej, besteff, 20 )
                hline  = TLine( 0      , besteff, 1      , besteff )
                vline  = TLine( bestrej, 0      , bestrej, 1       )
                pline  = TLine( bestrej, besteff, 1      , 1       )
                graph.Draw( "AP" )
                pline.SetLineColor( kBlue )
                pline.SetLineStyle( 2 )
                marker.SetMarkerColor( kRed )
                for el in ( hline, vline ):
                    el.SetLineColor( kRed )
                    el.SetLineStyle( 2 )
                for el in ( pline, hline, vline, marker ):
                    el.Draw( "SAME" )
                canvas.Update()
                raw_input( "Introduce any expression to continue: " )
            if save:
                graph.Write( "Loop_" + len( str( maxloop - iloop - 1 ) )*"0" +
                             str( iloop ) + "_" + cvar )
    print "WARNING: The ROC distance minimization process has not converged"
    return trigger

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
                self.nMiBevts += mngr.Nentries
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
                print "WARNING: CutID <", arg, "> already used, cut <", cut, "> not booked"
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

    def GetEfficiency( self, dtype, arg = [] ):
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
        elif arg == []:
            arg = xrange( cmngr.Nentries )
        return self.GetTrueEvents( cmngr, arg )*1./self.GetTrueEvents( mngr )

    def GetRate( self, arg = False ):
        ''' Gets the rate for a given cut string or list '''
        if isinstance( arg, str ):
            arg = self.CutMiBMngr.GetCutList( arg )
        if not arg:
            cutline = self.GetCutLine()
            if cutline:
                arg = self.CutMiBMngr.GetCutList( self.GetCutLine() )
            else:
                arg = xrange( self.CutMiBMngr.Nentries )
        return self.MiBrate*self.GetTrueEvents( self.CutMiBMngr, arg )*1./self.nMiBevts

    def GetRejection( self, dtype, arg ):
        ''' Gets the rejection for a given cut string or list '''
        return 1. - self.GetEfficiency( dtype, arg )

    def GetTrueEvents( self, mngr, lst = False ):
        ''' Method to get the number of true events in a given manager. If a list of
        events is specified this task is only performed on them. '''
        if self.EvtVar:
            event_list = mngr.Variables[ self.EvtVar ]
            ''' If < lst > is a void list it is maintained as it is '''
            if lst == False:
                lst = xrange( mngr.Nentries )
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

    def Print( self ):
        ''' Displays the information of the current trigger '''
        lines  = [ "Trigger features", "- Rate:" + str( self.GetRate() ), "- Efficiencies:" ]
        maxstr = max( len( mngr ) for mngr in self.SigMngr )
        for mngr in self.SigMngr:
            lines.append( "   " + mngr + ( maxstr - len( mngr ) )*" " +
                          " => " + str( self.GetEfficiency( mngr ) ) )
        maxstr = max( len( line ) for line in lines )
        decor  = maxstr*"*"
        for line in [ decor ] + lines + [ decor ]:
            print line

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
        ''' If only one variable is provided, it makes a list with it '''
        if not type( varlst ) in ( list, tuple ):
            varlst = [ varlst ]
        for el in self.SigMngr:
            results[ "n"    + el ] = npoints*[ 0 ]
            results[ "oldn" + el ] = self.SigMngr[ el ].Nentries
        ''' Performs the loop over all the cuts with the information provided '''
        sigmngrs = [ self.CutSigMngr[ el ] for el in self.CutSigMngr ]
        for i in range( npoints ):
            cut    = first + i*step
            lenlst = len( sigmngrs )*[ 0 ]
            for j, mngr in enumerate( sigmngrs ):
                varvals  = [ mngr.Variables[ var ] for var in varlst ]
                condlist = zip( *[ map( func, vals ) for vals in varvals ] )
                for ievt in xrange( mngr.Nentries ):
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
            elif name == "*":
                self.EnabledVars = []
            else:
                print "ERROR: Variable <", name ,"> not recognised"
