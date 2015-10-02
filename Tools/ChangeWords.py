#///////////////////////////////////////////////////////////////////
#//                                                               //
#//  Python modules                                               //
#//                                                               //
#// ------------------------------------------------------------- //
#//                                                               //
#//  AUTHOR: Miguel Ramos Pernas                                  //
#//  e-mail: miguel.ramos.pernas@cern.ch                          //
#//                                                               //
#//  Last update: 07/09/2015                                      //
#//                                                               //
#// ------------------------------------------------------------- //
#//                                                               //
#//  Description:                                                 //
#//                                                               //
#//  Allows to change a given word in a file or recursively in a  //
#//  directory:                                                   //
#//                                                               //
#//  python ChangeWords.py file input_word output_word            //
#//                                                               //
#// ------------------------------------------------------------- //
#///////////////////////////////////////////////////////////////////


import os, sys
from Isis.DirectoryManagement import ListAllFilePaths


#_______________________________________________________________________________
# Function to change the input words in a given file
def ChangeWords( ifile ):

    file_in  = open( ifile, "r" )
    file_out = open( "temporal_file.tmp", "w" )

    counter = 0
    for line in file_in:
        
        i = 0
        while i < len( line ):
            
            j = 0
            while ( i + j < len( line ) and
                    j < len( sys.argv[ 2 ] ) and
                    line[ i + j ] == sys.argv[ 2 ][ j ] ):
                
                j += 1
                
            if ( j == len( sys.argv[ 2 ] ) ):
                file_out.write( sys.argv[ 3 ] )
                i += len( sys.argv[ 2 ] )
                counter += 1
            
            else:
                file_out.write( line[ i ] )
                i += 1
    
    file_in.close()
    file_out.close()

    os.system( "rm " + ifile )
    os.system( "mv temporal_file.tmp " + ifile )

    print " Number of changes: ", counter

#_______________________________________________________________________________
# Main function
if len( sys.argv ) == 4: 
    if os.path.isdir( sys.argv[ 1 ] ):
        for ifile in ListAllFilePaths( sys.argv[ 1 ] ):
            print "Working with file <", ifile, ">"
            ChangeWords( ifile )
    else:
        ChangeWords( sys.argv[ 1 ] )
else:
    print "Input parameters should be: file/directory input_word output_word"

