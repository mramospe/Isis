#//////////////////////////////////////////////////////////
#//                                                      //
#//  Python modules                                      //
#//                                                      //
#// ---------------------------------------------------- //
#//                                                      //
#//  AUTHOR: Miguel Ramos Pernas                         //
#//  e-mail: miguel.ramos.pernas@cern.ch                 //
#//                                                      //
#//  Last update: 28/09/2015                             //
#//                                                      //
#// ---------------------------------------------------- //
#//                                                      //
#//  Description:                                        //
#//                                                      //
#//  Defines the class to manage data from/to Root files //
#//  and trees.                                          //
#//                                                      //
#// ---------------------------------------------------- //
#//////////////////////////////////////////////////////////


from ROOT import TFile, TTree, TDirectory
from copy import deepcopy
from numpy import array, bool_


#_______________________________________________________________________________
# Class DataManager
class DataManager:

    #_______________________________________________________________________________
    # Constructor given a name for the manager and arguments. The variable < args > 
    # has to contain the name of the input file and the trees inside. Attributes
    # can be added as well.
    def __init__( self, *args, **kwargs ):

        ''' For each entry in kwargs, another attribute with the same name is created '''
        for kw in kwargs:
            setattr( self, kw, kwargs[ kw ] )

        nargs = len( args )

        self.OwnsTargets = True
        self.Nentries    = 0
        self.Targets     = {}
        self.Variables   = {}

        ''' Error output for bad construction '''
        if nargs >= 2:
            self.AddTarget( *args )
        elif nargs and nargs < 2:
            print " Arguments for DataManager class are file_name and tree_name"
            exit()

    #_______________________________________________________________________________
    # Allows merging two objects of this class. The new manager owns all the targets.
    def __add__( self, other ):
        mngr          = DataManager()
        mngr.Nentries = self.Nentries + other.Nentries
        mngr.Targets.update( self.Targets  )
        mngr.Targets.update( other.Targets )
        true_vars = [ var for var in self.Variables if var in other.Variables ]
        for var in true_vars:
            mngr.Variables[ var ] = self.Variables[ var ] + other.Variables[ var ]
        self.OwnsTargets  = False
        other.OwnsTargets = False
        return mngr

    #_______________________________________________________________________________
    # Allows adding another manager variables to this class. The manager added
    # looses the possession of the targets.
    def __iadd__( self, other ):
        if self.Nentries:
            mngr = self + other
            other.OwnsTargets = False
            return self + other
        else:
            self = deepcopy( other )
            other.OwnsTargets = False

    #_______________________________________________________________________________
    # Definition of the delete destructor. If this class owns the target files they
    # are closed.
    def __del__( self ):
        if self.OwnsTargets:
            for tgt in self.Targets:
                for tree in self.Targets[ tgt ]:
                    tree = 0
                tgt.Close()

    #_______________________________________________________________________________
    # Gets the values for variable < var >
    def __getitem__( self, var ):
        return self.Variables[ var ]

    #_______________________________________________________________________________
    # Gets the number of entries in the class
    def __len__( self ):
        return self.Nentries

    #_______________________________________________________________________________
    # Adds a new target file and/or tree to the class
    def AddTarget( self, *args ):
        if args[ 0 ] not in [ ifile.GetName() for ifile in self.Targets ]:
            ifile = TFile.Open( args[ 0 ], "READ" )
            tlist = [ ifile.Get( tname ) for tname in args[ 1: ] ]
            self.Targets[ ifile ]  = tlist
            print " Added target <", args[ 0 ], "> and trees", list( args[ 1: ] )
        else:
            ifile = [ ifile for ifile in self.Targets if ifile.GetName() == args[ 0 ] ][ 0 ]
            tlist = [ ifile.Get( tname ) for tname in args[ 1: ] ]
            self.Targets[ ifile ] += tlist
            print " Added trees", list( tlist ), "to target <", ifile.GetName()

        for name in self.Variables:
            vvals = self.Variables[ name ]
            for tree in tlist:
                for ievt in range( tree.GetEntries() ):
                    tree.GetEntry( ievt )
                    vvals.append( getattr( tree, name ) )

        if not self.Nentries and len( self.Variables ):
            self.Nentries = len( vvals )

    #_______________________________________________________________________________
    # Adds a new  variable(s) to the class given a name and a list. This method
    # checks that all the variables' lists have the same lengths, otherwise it
    # exits he program. The input has to be formated as [ var1_name, list1 ],
    # [ var2_name, list2 ], ...
    def AddVariables( self, *fvars ):
        if self.Nentries == 0:
            self.Nentries = len( fvars[ 0 ][ 1 ] )
        for el in fvars:
            if len( el[ 1 ] ) == self.Nentries:
                self.Variables[ el[ 0 ] ] = el[ 1 ]
            else:
                print " Variable", el[ 0 ], "to be added to the manager has different length."
                exit()
            
    #_______________________________________________________________________________
    # Books a new variable(s) to the class. The variable's values list is filled
    # with the variable's values contained in the targets. If a variable already
    # exists in the class the process is omited. If < * > is specified, the
    # variables are set as those that are common for all the trees.
    def BookVariables( self, *var_names ):
        if len( var_names ) == 1 and '*' in var_names[ 0 ]:
            tlist     = self.GetListOfTrees()
            brlist    = tlist[ 0 ].GetListOfBranches()
            var_names = [ brlist.At( i ).GetName() for i in range( brlist.GetSize() ) ]
            for tree in tlist[ 1: ]:
                brlist        = tree.GetListOfBranches()
                new_var_names = []
                for i in range( brlist.GetSize() ):
                    brname = brlist.At( i ).GetName()
                    if brname in var_names:
                        new_var_names.append( brname )
                for name in var_names:
                    if name not in new_var_names:
                        var_names.remove( name )
            self.BookVariables( *var_names )
            return
        if len( self.Targets ):
            tlist = self.GetListOfTrees()
            for name in var_names:
                if name not in self.Variables:
                    self.Variables[ name ] = []
                    vvals = self.Variables[ name ]
                    for tree in tlist:
                        for ievt in range( tree.GetEntries() ):
                            tree.GetEntry( ievt )
                            vvals.append( getattr( tree, name ) )
                else:
                    print " Variable", name, "already booked"
            self.Nentries = len( vvals )
        else:
            print " No targets added to the manager, could not book variables:", var_names
            exit()

    #_______________________________________________________________________________
    # Returns a copy of this class that does not own the targets.
    def Copy( self ):
        mngr = deepcopy( self )
        mngr.OwnsTargets = False
        return mngr

    #_______________________________________________________________________________
    # Returns another DataManager based on the events that satisfy the cuts given.
    # The targets are not transfered to the new class.
    def CutSample( self, cut ):
        mngr     = DataManager()
        add_list = self.GetCutList( cut )
        for kwvar in self.Variables:
            vlist = self.Variables[ kwvar ]
            mngr.Variables[ kwvar ] = [ vlist[ i ] for i in add_list ]
        mngr.Nentries = len( add_list )
        return mngr

    #_______________________________________________________________________________
    # Closes all the target files owned by the class
    def CloseFiles( self ):
        for ifile in self.Targets:
            for itree in self.Targets[ ifile ]:
                itree = 0
            ifile.Close()
            del self.Targets[ ifile ]

    #_______________________________________________________________________________
    # This method allows to obtain a list with the events that satisfy the cuts given
    def GetCutList( self, cut ):
        cut       = cut.replace( "&&", " and " )
        cut       = cut.replace( "||", " or "  )
        variables = cut
        for el in [ '==', '!=', '<=', '>=', '>', '<', 'and', 'or', '(', ')' ]:
            variables = variables.replace( el, '|' )
        variables = variables.replace( ' ', '' )
        variables = variables.split( '|' )
        while '' in variables:
            variables.remove( '' )
        var_list = []
        for el in variables:
            try:
                float( el )
            except:
                if el in self.Variables:
                    var_list.append( el )
                else:
                    print " Cut in variable", el, "not valid, variable does not exist"
        values = [ self.Variables[ var ] for var in var_list ]
        nvars  = len( var_list )
        for ivar in range( nvars ):
            cut = cut.replace( var_list[ ivar ], "values[ %i ][ ievt ]" %ivar )
        return eval( "[ ievt for ievt in range( self.Nentries ) if " + cut + " ]" )

    #_______________________________________________________________________________
    # Gets the number of entries of the class. If a cut selection is given, it is
    # calculated the number of events that satisfy those cuts.
    def GetEntries( self, selection = False ):
        if selection:
            return len( self.GetCutList( selection ) )
        else:
            return self.Nentries

    #_______________________________________________________________________________
    # Gets the event at position ievt. Allows to get only the variables in < args >
    # in the order specified.
    def GetEvent( self, ievt, *args ):
        if len( args ):
            return tuple( [ self.Variables[ var ][ ievt ] for var in args ] )
        else:
            return tuple( [ self.Variables[ var ][ ievt ] for var in self.Variables ] )

    #_______________________________________________________________________________
    # Returns a list with the trees attached to the class
    def GetListOfTrees( self ):
        tlist   = []
        rawlist = [ self.Targets[ ifile ] for ifile in self.Targets ]
        for el in rawlist:
            tlist += el
        return tlist

    #_______________________________________________________________________________
    # Gets the number of variables in the class
    def GetNvars( self ):
        return len( self.Variables )

    #_______________________________________________________________________________
    # Gets the targets directory of the class. If this method is called, the class
    # automatically frees the targets, so their destruction are left to the new owner.
    def GetTargets( self ):
        self.OwnsTargets = False
        return self.Targets

    #_______________________________________________________________________________
    # Gets the list of values for a given variable. If a cut is specified it returns
    # the values concerning the sample which satisfies them.
    def GetVarEvents( self, variable, cut = False ):
        res_list = []
        if cut:
            for ievt in self.GetCutList( cut ):
                res_list.append( self.Variables[ variable ][ ievt ] )
        else:
            for ievt in range( self.Nentries ):
                res_list.append( self.Variables[ variable ][ ievt ] )
        return res_list

    #_______________________________________________________________________________
    # Gets the name of the variables in the class
    def GetVarNames( self ):
        return [ var for var in self.Variables ]

    #_______________________________________________________________________________
    # Makes another variable using those in the class. There have to be specified:
    # the new variable name, the name of the variables used by the function
    # ( ordered in a list ) and the function itself. The computation of the new
    # variable is going to be performed passing the function the variables as normal
    # entries ( *args, where args is the list of values ).
    def MakeVariable( self, var_name, arg_list, function ):
        new_variable = self.Nentries*[ 0. ]
        var_tensor   = [ self.Variables[ vname ] for vname in arg_list ]

        lvars = range( len( arg_list ) )
        for ievt in range( self.Nentries ):
            values               = [ var_tensor[ ivar ][ ievt ] for ivar in lvars ]
            new_variable[ ievt ] = function( *values )

        self.Variables[ var_name ] = new_variable

    #_______________________________________________________________________________
    # Prints the information of the class as well as the values for the first 20
    #  events. If < events > is introduced as an input, the number of events showed
    # would be that specified by the user. If < cut > is specified only will be
    # showed the events that statisfy the given cut. If < name > is specified this
    # name is going to be printed before the other information.
    def Print( self, *args, **kwargs ):

        if "cut" not in kwargs:
            kwargs[ "cut" ] = False
        if "events" not in kwargs:
            kwargs[ "events" ] = 20
        if "name" not in kwargs:
            kwargs[ "name" ] = "Manager"
        if "variables" not in kwargs:
            kwargs[ "variables" ] = [ var for var in self.Variables ]
            kwargs[ "variables" ].sort()

        max_length = 0
        for var in kwargs[ "variables" ]:
            lvar = len( var )
            if lvar > max_length:
                max_length = lvar

        ''' Prints the name of the manager '''
        lname = len( kwargs[ "name" ] ) + 2
        print "\n " + lname*'*' + "\n  " + kwargs[ "name" ] + "  \n " + lname*'*'
        
        ''' Prints the targets '''
        if len( self.Targets ):
            print "\n Files atached:"
            for ifile in self.Targets:
                print " - " + ifile.GetName() + ":"
                for tree in self.Targets[ ifile ]:
                    print "\t" + tree.GetName()

        ''' Prints the variables '''
        print "\n Variables: "
        vout      = " "
        variables = []
        if len( args ):
            variables = args
            for var in args:
                vout      += var + ", "
        else:
            for var in kwargs[ "variables" ]:
                variables.append( var )
                vout      += var + ", "
        print vout[ :-2 ] + "\n"

        ''' Prints the values of the variables '''
        vout = " "
        for var in variables:
            vout += "%.7g" + "\t|\t"
        vout = vout[ :-2 ]

        if kwargs[ "cut" ]:
            i = 0
            for ievt in self.GetCutList( kwargs[ "cut" ] ):
                if i == kwargs[ "events" ]: break
                i += 1
                print vout % self.GetEvent( ievt, *variables )
        else:
            for ievt in range( self.Nentries ):
                if ievt == kwargs[ "events" ]: break
                print vout % self.GetEvent( ievt, *variables )

    #_______________________________________________________________________________
    # Removes a booked variable
    def RemoveVariable( self, var ):
        del self.Variables[ var ]

    #_______________________________________________________________________________
    # Saves the given class values in a TTree. If args has only one name, it is
    # considered as the tree name, so its written in the external directory ( to
    # be constructed and accesible in the main program ), if two names are provided
    # the first one is considered as the file name and the second the tree name. If
    # < close > is provided, and if its value is false, this method will return
    # the output file. 
    def Save( self, *args, **kwargs ):
        
        if len( args ) == 2:
            output_file = TFile.Open( args[ 0 ], "RECREATE" )
            output_tree = TTree( args[ 1 ], args[ 1 ], 0 )
            print " Saving tree with name <", args[ 1 ], "> in <", args[ 0 ], ">"
        else:
            output_tree = TTree( args[ 0 ], args[ 0 ], 0 )
            print ( " Saving tree with name <", args[ 1 ], "> in <", gDirectory.GetName(), ">" )

        variables = [ var for var in self.Variables ].sort()
            
        ''' Constructs the list of variables to make the branches, and fills the tensor of
        variables to fill the tree '''
        var_values = []
        var_tensor = []
        for var in variables:
            if isinstance( self.Variables[ var ][ 0 ], float ):
                vtype = "/D"
                var_values.append( array( [ 0. ] ) )
            elif all( isinstance( self.Variables[ var ][ i ], bool_ ) 
                      for i in range( self.Nentries ) ):
                vtype = "/O"
                var_values.append( array( [ 0 ], dtype = bool_ ) )
            else:
                vtype = "/I"
                var_values.append( array( [ 0 ] ) )
            output_tree.Branch( var, var_values[ -1 ], var + vtype )
            var_tensor.append( self.Variables[ var ] )

        ''' Begins the loop to fill the tree '''
        print " Writing", self.Nentries, "events"
        rvars = range( len( variables ) )
        for ievt in range( self.Nentries ):
            for iv in rvars:
                var_values[ iv ][ 0 ] = var_tensor[ iv ][ ievt ]
            output_tree.Fill()

            if ievt % 10000 == 0:
                output_tree.AutoSave()

        output_tree.AutoSave()

        ''' Depending on the input conditions the output file is returned  '''
        if "close" in kwargs:
            if kwargs[ "close" ]:
                output_file.Close()
            else:
                return output_file
        else:
            output_file.Close()
