#///////////////////////////////////////////////////////////
#//                                                       //
#//  Python modules                                       //
#//                                                       //
#// ----------------------------------------------------- //
#//                                                       //
#//  AUTHOR: Miguel Ramos Pernas                          //
#//  e-mail: miguel.ramos.pernas@cern.ch                  //
#//                                                       //
#//  Last update: 16/11/2015                              //
#//                                                       //
#// ----------------------------------------------------- //
#//                                                       //
#//  Description:                                         //
#//                                                       //
#//  Class to storage cuts present in a cuts-file. This   //
#//  files have to be written in the way:                 //
#//                                                       //
#//  cut1 = j > 0 and b > 1                               //
#//                                                       //
#//  cut2 = $cut1$ or c == 0                              //
#//                                                       //
#//  The symbol < $ > before and after a word means that  //
#//  is a cut, so the class is going to get that and      //
#//  place it instead. This class is going to search      //
#//  long as no key has the same string value as it,      //
#//  being situated at the beginning of the line.         //
#//                                                       //
#// ----------------------------------------------------- //
#///////////////////////////////////////////////////////////


from collections import OrderedDict


#_______________________________________________________________________________

class CutManager:

    def __init__( self, file_name = False ):
        ''' Constructor given the name of the cuts file '''
        self.Options = { "and": "&&", "or": "||" }
        self.CutFile = []
        self.CutList = OrderedDict()
        if file_name:
            ifile = open(file_name)
            for line in ifile:
                while "\t" in line: line = line.replace( "\t", ' ' )
                while "\n" in line: line = line.replace( "\n", '' )
                if line != "":
                    self.CutFile.append( [ line ] )

    def __getitem__( self, key ):
        ''' Gets the cut related to the key given '''
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
        if name in self.CutList:
            print "Cut with name <", name, "> already booked"
            return False
        cut = self.GetCut( name )
        if cut:
            self.CutList[ name ] = cut
            print "Booked new cut <", name, ">:", self.CutList[ name ]
        return cut

    def GetCut( self, name ):
        ''' Gets the Cut named < name > in the file attached to this class '''
        cuts = False
        for line in self.CutFile:
            if line != []:
                if line[ 0 ].split()[ 0 ] == name:
                    cuts = line[ 0 ]
                    cuts = cuts[ cuts.find( '=' ) + 1: ]
                    while cuts[ 0 ] == " ": cuts = cuts[ 1: ]
                    ifirst = 0
                    while '$' in cuts:
                        ifirst = cuts.find( '$', ifirst )
                        ilast  = cuts.find( '$', ifirst + 1 )
                        newcut = cuts[ ifirst : ilast + 1 ]
                        cuts   = cuts.replace( newcut, "(" + self.GetCut( newcut[ 1:-1 ] ) + ")" )
                    for el in self.Options:
                        while el in cuts:
                            cuts = cuts.replace( el, self.Options[ el ] )
                    break
        if not cuts:
            print "WARNING: cut with name <", name, "> does not exist."

        return cuts

    def Open( self, file_name ):
        ''' Opens a new file and gets its content '''
        self.CutFile = []
        ifile = open(file_name)
        for line in ifile:
            while "\t" in line: line = line.replace( "\t", ' ' )
            while "\n" in line: line = line.replace( "\n", '' )
            if line != "":
                self.CutFile.append( [ line ] )

    def Print( self ):
        ''' Prints the cuts booked in the class '''
        for key in self.CutList:
            print key, "->", self.CutList[ key ]
