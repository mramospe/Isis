#//////////////////////////////////////////////////////////////////////
#//                                                                  //
#//  Python modules                                                  //
#//                                                                  //
#// ---------------------------------------------------------------- //
#//                                                                  //
#//  AUTHOR: Miguel Ramos Pernas                                     //
#//  e-mail: miguel.ramos.pernas@cern.ch                             //
#//                                                                  //
#//  Last update: 09/10/2015                                         //
#//                                                                  //
#// ---------------------------------------------------------------- //
#//                                                                  //
#//  Description:                                                    //
#//                                                                  //
#//  Looks for differences between two files. Blank lines are        //
#//  neglected. If two lines are different in the files, a search    //
#//  is made to look if does exist another equal line in the other   //
#//  file. If true, the information is displayed and both lines are  //
#//  avoided.                                                        //
#//                                                                  //
#//  python SeeDifferences.py file1 file2                            //
#//                                                                  //
#// ---------------------------------------------------------------- //
#//////////////////////////////////////////////////////////////////////


import sys
from copy import deepcopy


#_______________________________________________________________________________
# Function to open the files
def OpenFile( name ):
    ifile = file( name, "rt" )
    rfile = ifile.readlines()
    lth   = len( rfile )
    rfile = [ line.replace( '\n', '' ) for line in rfile ]
    nblth = len( [ line for line in rfile if line.strip() ] )
    ifile.close()
    return rfile, lth, nblth

#_______________________________________________________________________________
# Main function
print "======================================"
print "This function will neglect blank lines"
print "Line numbers correspond with true lines in the files"

''' The files are opened and read. Their number of lines are calculated '''
rfile1, lf1, nblf1 = OpenFile( sys.argv[ 1 ] )
rfile2, lf2, nblf2 = OpenFile( sys.argv[ 2 ] )

print "File <", sys.argv[ 1 ], ">\t Lines:", lf1, "\tNon-blank lines:", nblf1
print "File <", sys.argv[ 2 ], ">\t Lines:", lf2, "\tNon-blank lines:", nblf2

''' Sets the variables for the main loop '''
if ( nblf1 > nblf2 ):
    blth , slth  = lf1, lf2
    bname, sname = sys.argv[ 1 ], sys.argv[ 2 ]
    bfile, sfile = rfile1, rfile2
else:
    blth , slth  = lf2, lf1
    bname, sname = sys.argv[ 2 ], sys.argv[ 1 ]
    bfile, sfile = rfile2, rfile1

scntr, bcntr = 0 , 0
sav  , bav   = [], []

print "============ "
print "Differences:"

while bcntr < blth:
    bline = bfile[ bcntr ]
    bcntr += 1
    ''' Checks if the line is non-blank, and if it must not be skiped '''
    if bline.strip() and bcntr not in bav:
        ''' Searches for a non-blanck line in the other file that must not be skiped '''
        for sline in sfile[ scntr: ]:
            scntr += 1
            if sline.strip() and scntr not in sav:
                if sline != bline:

                    bstr = bname + " in line " + str( bcntr ) + ": '" + bline + "'"
                    sstr = sname + " in line " + str( scntr ) + ": '" + sline + "'"

                    ''' Prints the lines that are not equal '''
                    print max( len( bstr ), len( sstr ) )*'-'
                    print bstr
                    print sstr

                    ''' Searches for an equal line in the other file, and adds it to the list of
                    those who must be skiped ( for both lines and files )'''
                    if bline in sfile[ scntr: ]:
                        sn = sfile.index( bline ) + 1
                        print "Line", bcntr, "(", bname, ")", "matches line", sn, "(", sname, ")"
                        sav.append( sn - 1 )
                        bav.append( bcntr - 1 )
                    if sline in bfile[ bcntr: ]:
                        bn = bfile.index( sline ) + 1
                        print "Line", scntr, "(", sname, ")", "matches line", bn, "(", bname, ")"
                        bav.append( bn )
                        sav.append( scntr - 1 )
                break
            
    ''' If it reaches the end of some file, prints the current number of the line for the
    non-finished file '''
    if bcntr == blth and scntr != slth:
        string  = "End of file < " + bname + " > reached, lines after "
        string += str( scntr ) + " in < " + sname + " > don't match"
        print len( string )*'-'
        print string
        break
    elif bcntr != blth and scntr == slth:
        string  = "End of file < " + sname + " > reached, lines after "
        string += str( bcntr ) + " in < " + bname + " > don't match"
        print len( string )*'-'
        print string
        break

print "======================================="
