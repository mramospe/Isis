#//////////////////////////////////////////////////////////
#//                                                      //
#//  Python modules                                      //
#//                                                      //
#// ---------------------------------------------------- //
#//                                                      //
#//  AUTHOR: Miguel Ramos Pernas                         //
#//  e-mail: miguel.ramos.pernas@cern.ch                 //
#//                                                      //
#//  Last update: 24/07/2015                             //
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

    def __init__( self, file_name ):

        ''' Constructor given the name of the cuts file '''

        self.Options = { "and": "&&", "or": "||" }
        self.CutFile = []
        self.CutList = OrderedDict()

        ifile = open("Cut.dat")

        for line in ifile:
            self.CutFile += [ filter( lambda x: x != '',
                                      [ el.replace( '\n', '' ) for el in line.split( '\t' ) ] ) ]

            
    def __getitem__( self, key ):

        ''' Gets the cut related to the key '''

        return self.CutList[ index ]

    
    def Clear( self ):

        ''' Clears the content obtained from the file as well as the cuts booked '''

        self.CutList = OrderedDict()
        self.CutFile = []
        

    def ClearFile( self ):
        
        ''' Clears the content obtained from the file '''

        self.CutFile = []


    def BookCut( self, name ):

        ''' Books a new cut and returns it '''

        self.CutList[ name ] = self.GetCut( name )

        return self.CutList[ name ]


    def GetCut( self, name ):

        ''' Gets the Cut named <name> in the file atached to this class '''

        total_cuts = ""
        condition  = False

        for line in self.CutFile:
        
            if line != []:

                if condition:

                    if line[ 0 ] == "Cuts":
                        total_cuts += line[ 1 ]

                    if line[ 0 ] == "Sets":
                        for el in line[ 1 ].split():
                            if el == "(":
                                total_cuts += "( "
                            elif el == ")":
                                total_cuts += " )"
                            elif el in self.Options:
                                total_cuts += el
                            else:
                                total_cuts += self.GetCut( el )

                    if line[ 0 ] in self.Options:
                        total_cuts += ( " " + line[ 0 ] + " " )

                    if line[ 0 ] == "End":
                        condition = False

                elif line[ 0 ] == "Define" and line[ 1 ] == name:
                    condition = True

            for el in self.Options:
                total_cuts = total_cuts.replace( el, self.Options[ el ] )

        return "( " + total_cuts + " )"


    def Open( self, file_name ):

        ''' Opens a new file and gets its content '''

        self.CutFile = []

        ifile = open( file_name )

        for line in ifile:
            self.CutFile += [ filter( lambda x: x != '',
                                      [ el.replace( '\n', '' ) for el in line.split( '\t' ) ] ) ]


    def Print( self ):

        ''' Prints the cuts booked in the class '''

        for key in self.CutList:
            print key, "->", self.CutList[ key ]
