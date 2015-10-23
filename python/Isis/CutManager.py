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
#//  cut1 = j > 0 and b > 1                              //
#//                                                      //
#//  cut2 = $cut1 or c == 0                              //
#//                                                      //
#//  The symbol < $ > after a word means that this word  //
#//  is a cut, so the class is going to read that and    //
#//  place it instead. This class is going to search     //
#//  for the given keys, so one can comment anything as  //
#//  long as no key has the same string value as it.     //
#//                                                      //
#// ---------------------------------------------------- //
#//////////////////////////////////////////////////////////


from collections import OrderedDict


#_______________________________________________________________________________

class CutManager:

    #_______________________________________________________________________________
    # Constructor given the name of the cuts file
    def __init__( self, file_name = False ):

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
        if name in self.CutList:
            print "Cut with name <", name, "> already booked"
            return False
        cut = self.GetCut( name )
        if cut:
            self.CutList[ name ] = cut
            print "Booked new cut <", name, ">:", self.CutList[ name ]
        return cut

    #_______________________________________________________________________________
    # Gets the Cut named < name > in the file atached to this class
    def GetCut( self, name ):

        total_cuts = ""
        for line in self.CutFile:
            if line != []:

                spline = line[ 0 ].split()
                if spline[ 0 ] == name:

                    ''' The cuts are taking after the second string in < spline > ( usually an
                    < = > symbol '''
                    spline = spline[ 2: ]
                    lsplit = []

                    for el in spline:
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
                        if   el in ( "(", ")" ):
                            total_cuts += el
                        elif el in self.Options:
                            total_cuts += " " + el + " "
                        else:
                            if el[ 0 ] == '$':
                                new_cut    =  self.GetCut( el[ 1: ] )
                                if new_cut:
                                    total_cuts += "(" + new_cut + ")"
                                else:
                                    return total_cuts
                            else:
                                total_cuts += " " + el + " "

            for el in self.Options:
                total_cuts = total_cuts.replace( el, self.Options[ el ] )

        if total_cuts:
            ''' Formats the output string '''
            while "  " in total_cuts: total_cuts = total_cuts.replace( "  ", " " )
            while "( " in total_cuts: total_cuts = total_cuts.replace( "( ", "(" )
            while " )" in total_cuts: total_cuts = total_cuts.replace( " )", ")" )
            if total_cuts[ 0 ]  == " ": total_cuts = total_cuts[ 1:  ]
            if total_cuts[ -1 ] == " ": total_cuts = total_cuts[ :-1 ]
        else:
            print "WARNING: cut with name <", name, "> does not exist."

        return total_cuts

    #_______________________________________________________________________________
    # Opens a new file and gets its content
    def Open( self, file_name ):
        self.CutFile = []
        ifile = open(file_name)
        for line in ifile:
            while "\t" in line: line = line.replace( "\t", ' ' )
            while "\n" in line: line = line.replace( "\n", '' )
            if line != "":
                self.CutFile.append( [ line ] )

    #_______________________________________________________________________________
    # Prints the cuts booked in the class
    def Print( self ):
        for key in self.CutList:
            print key, "->", self.CutList[ key ]
