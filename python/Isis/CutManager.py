#//////////////////////////////////////////////////////////
#//                                                      //
#//  General package                                     //
#//                                                      //
#// ---------------------------------------------------- //
#//                                                      //
#//  AUTHOR: Miguel Ramos Pernas                         //
#//  e-mail: miguel.ramos.pernas@cern.ch                 //
#//                                                      //
#//  Last update: 23/10/2015                             //
#//                                                      //
#// ---------------------------------------------------- //
#//                                                      //
#//  Description:                                        //
#//                                                      //
#//  Class to storage cuts present in a cuts-file. This  //
#//  files have to be written in the way:                //
#//                                                      //
#//  Define cut_name                                     //
#//                                                      //
#//  Cuts   some_cut and another_cut                     //
#//                                                      //
#//  or                                                  //
#//                                                      //
#//  Sets   cut_set_1 or cut_set_2                       //
#//                                                      //
#//  End                                                 //
#//                                                      //
#// ---------------------------------------------------- //
#//////////////////////////////////////////////////////////


from collections import OrderedDict


#_______________________________________________________________________________

class CutManager:

    #_______________________________________________________________________________
    # Constructor given the name of the cuts file
    def __init__( self, file_name ):

        self.Options = { "and": "&&", "or": "||" }
        self.CutFile = []
        self.CutList = OrderedDict()

        ifile = open(file_name)
        for line in ifile:
            self.CutFile += [ filter( lambda x: x != '',
                                      [ el.replace( '\n', '' ) for el in line.split( '\t' ) ] ) ]

    #_______________________________________________________________________________
    # Gets the cut related to the key given
    def __getitem__( self, key ):
        return self.CutList[ index ]

    #_______________________________________________________________________________
    # Clears the content obtained from the file as well as the cuts booked
    def Clear( self ):
        self.CutList = OrderedDict()
        self.CutFile = []
        
    #_______________________________________________________________________________
    # Clears the content obtained from the file
    def ClearFile( self ):
        self.CutFile = []

    #_______________________________________________________________________________
    # Books a new cut and returns it
    def BookCut( self, name ):
        self.CutList[ name ] = self.GetCut( name )
        print " Booked new cut <", name, ">:", self.CutList[ name ]
        return self.CutList[ name ]

    #_______________________________________________________________________________
    # Gets the Cut named < name > in the file atached to this class
    def GetCut( self, name ):

        total_cuts = ""
        condition  = False

        for line in self.CutFile:
        
            if line != []:

                if condition:

                    ''' In the < Cuts > space, the entire line is added '''
                    if line[ 0 ] == "Cuts":
                        total_cuts += line[ 1 ]

                    ''' In the < Sets > space, a search is made to find all the different sets
                    of cuts to be added. The first loop is to allow writing spaces after and
                    before parentheses. '''
                    if line[ 0 ] == "Sets":

                        lsplit = []
                        for el in line[ 1 ].split():
                            if   '(' in el[ 0 ]:
                                lsplit.append( '(' )
                                if len( el[ 1: ] ) > 0:
                                    lsplit.append( el[ 1: ] )
                            elif ')' in el[ -1 ]:
                                if len( el[ :-1 ] ) > 0:
                                    lsplit.append( el[ :-1 ] )
                                lsplit.append( ')' )
                            else:
                                lsplit.append( el )

                        for el in lsplit:
                            if el == "(":
                                total_cuts += "("
                            elif el == ")":
                                total_cuts += ")"
                            elif el in self.Options:
                                total_cuts += " " + el + " "
                            else:
                                total_cuts += "(" + self.GetCut( el ) + ")"

                    if line[ 0 ] in self.Options:
                        total_cuts += " " + line[ 0 ] + " "

                    if line[ 0 ] == "End":
                        condition = False

                elif line[ 0 ] == "Define" and line[ 1 ] == name:
                    condition = True

            for el in self.Options:
                total_cuts = total_cuts.replace( el, self.Options[ el ] )

        return total_cuts

    #_______________________________________________________________________________
    # Opens a new file and gets its content
    def Open( self, file_name ):
        self.CutFile = []
        ifile = open( file_name )
        for line in ifile:
            self.CutFile += [ filter( lambda x: x != '',
                                      [ el.replace( '\n', '' ) for el in line.split( '\t' ) ] ) ]

    #_______________________________________________________________________________
    # Prints the cuts booked in the class
    def Print( self ):
        for key in self.CutList:
            print key, "->", self.CutList[ key ]
