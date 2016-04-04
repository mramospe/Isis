#///////////////////////////////////////////////////////////
#//                                                       //
#//  Python modules                                       //
#//                                                       //
#// ----------------------------------------------------- //
#//                                                       //
#//  AUTHOR: Miguel Ramos Pernas                          //
#//  e-mail: miguel.ramos.pernas@cern.ch                  //
#//                                                       //
#//  Last update: 04/04/2016                              //
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
        self.Options = { 'and': '&&', 'or': '||' }
        self.CutFile = []
        self.CutList = OrderedDict()
        if file_name:
            self.Open( file_name )

    def __getitem__( self, key ):
        ''' Gets the cut related to the key given '''
        return self.CutList[ key ]

    def Clear( self ):
        ''' Clears the content obtained from the file as well as the cuts booked '''
        self.CutList = OrderedDict()
        self.CutFile = []
        
    def ClearFile( self ):
        ''' Clears the content obtained from the file '''
        self.CutFile = []

    def BookCut( self, name, prt = True ):
        ''' Books a new cut and returns it '''
        if name in self.CutList:
            print 'Cut with name <', name, '> already booked'
            return False
        cut = self.GetCut( name )
        if cut:
            self.CutList[ name ] = cut
            if prt:
                print 'Booked new cut <', name, '>:', self.CutList[ name ]
        return cut

    def GetCut( self, name ):
        ''' Gets the Cut named < name > in the file attached to this class '''
        cuts = False
        for line in self.CutFile:
            first, second = line.split( '=' )
            while first.startswith( ' ' ):  first  = first[ 1: ]
            while first.endswith( ' ' ):    first  = first[ :-1 ]
            while second.startswith( ' ' ): second = second[ 1: ]
            while second.endswith( ' ' ):   second = second[ :-1 ]
            if first == name:
                cuts = second
                while cuts[ 0 ] == ' ': cuts = cuts[ 1: ]
                ifirst = 0
                while '$' in cuts:
                    ifirst = cuts.find( '$', ifirst )
                    ilast  = cuts.find( '$', ifirst + 1 )
                    newcut = cuts[ ifirst : ilast + 1 ]
                    cuts   = cuts.replace( newcut, self.GetCut( newcut[ 1:-1 ] ) )
                for el in self.Options:
                    while el in cuts:
                        cuts = cuts.replace( el, self.Options[ el ] )
                    break
        if cuts:
            return '(' + cuts + ')'
        else:
            print 'WARNING: cut with name <', name, '> does not exist.'
            return False

    def Open( self, file_name ):
        ''' Opens a new file and gets its content. In this method the lines are checked to see
        if they have the correct format. '''
        self.CutFile = []
        ifile = open(file_name)
        for il, line in enumerate( ifile ):
            if line[ -1 ] == '\n':
                line = line[ :-1 ]
            if line != '' and line[ 0 ] != '#':
                pos = line.find( '=' )
                if line.find( '=', pos + 1 ) != -1:
                    print 'ERROR: Found two assignment operators in line <', il, '>'
                    return
                if pos != -1:
                    ''' Checks that the cut name is free of whitespaces '''
                    pos = line[ :pos ]
                    while pos.startswith( ' ' ):
                        pos = pos[ 1: ]
                    while pos.endswith( ' ' ):
                        pos = pos[ :-1 ]
                    if pos.find( ' ' ) != -1:
                        print 'ERROR: The cut defined in line <', il + 1, '> has whitespaces on its name'
                        self.CutFile = []
                        return
                    ''' Checks the matching of the < $ > symbols in the expression '''
                    pos, newpos = 0, 0
                    while '$' in line[ newpos: ]:
                        pos     = line.find( '$', newpos )
                        pos    += 1
                        newpos  = line.find( '$', pos )
                        sbl     = line[ pos:newpos ]
                        if newpos == -1 or ' ' in sbl:
                            print 'ERROR: Mismatched < $ > symbol in line <', il + 1, '>'
                            self.CutFile = []
                            return
                        newpos += 1
                        
                else:
                    print 'WARNING: Line number <', il + 1, '> not a cut line; must be commented (starting by #)'
                self.CutFile.append( line )
        ifile.close()

    def Print( self ):
        ''' Prints the cuts booked in the class '''
        for key in self.CutList:
            print key, '->', self.CutList[ key ]

    def RemoveCut( self, kw ):
        ''' Removes the cut identified by the key < kw > '''
        del self.CutList[ key ]
