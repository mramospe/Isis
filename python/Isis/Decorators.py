#///////////////////////////////////////////////////////////////////////////////
#//
#//  Python modules
#//
#// ----------------------------------------------------------------------------
#//
#//  AUTHOR: Miguel Ramos Pernas
#//  e-mail: miguel.ramos.pernas@cern.ch
#//
#//  Last update: 03/04/2017
#//
#// ----------------------------------------------------------------------------
#//
#//  Description:
#//
#//  In this file there are defined some useful decorators for the different
#//  functions and methods of the project.
#//
#// ----------------------------------------------------------------------------
#///////////////////////////////////////////////////////////////////////////////


import time
from Isis.Utils import convertArgs, formatTime, terminalSize


class decoArgBase:
    '''
    Base decorator to call a function modifying its arguments
    '''
    def __init__( self, func ):
        '''
        The function to be called is parsed
        '''
        self.func = func

    def __call__( self, *args, **kwargs ):
        '''
        Call the function
        '''
        return self.func(*args, **kwargs)


def decoClassMethod( func ):
    '''
    Thin second-order decorator designed to be used with class methods
    '''
    def wrapper( self, *args, **kwargs ):
        return func( self, *args, **kwargs )
    
    return wrapper


class _decoInputArgs( decoArgBase ):
    '''
    Base decorator to apply a function < conv > to each argument based on the
    slice < slc >. The keyword arguments can also be parsed as a list in < kvars >
    '''
    def __init__( self, conv, func, slc, kvars ):
        '''
        Call base class and set the number of arguments to convert
        '''
        decoArgBase.__init__(self, func)
        self.conv  = conv
        self.kvars = kvars
        self.slc   = slc
        
    def __call__( self, *args, **kwargs ):
        '''
        Convert arguments with indices < self.slc > and call function
        '''
        func_args = convertArgs(self.conv, args, self.slc)

        if self.kvars:
            common = set(self.kvars).intersection(kwargs.keys())
            values = [kwargs[kw] for kw in common]
            values = convertArgs(self.conv, values)
            for kw, v in zip(self.kvars, values):
                kwargs[kw] = v

        return self.func(*func_args, **kwargs)


def decoInputArgs( conv, slc = slice(0, None), kvars = None ):
    '''
    Function to extend the functionality of the class < _decoInputArgs > as a
    decorator
    '''
    kvars = kvars or []
    
    def wrapper( func ):
        return _decoInputArgs(conv, func, slc, kvars)
        
    return wrapper


def trackTime( function ):
    '''
    This decorator displays the time information about a particular job, together
    with some separators
    '''
    def wrapper( *args, **kwargs ):
        '''
        This is the wrapper function to be returned
        '''

        ''' Gets the time information '''
        stime, lctime, gmtime = time.time(), time.localtime(), time.gmtime()        
        lctstr = time.strftime( "%a, %d %b %Y %H:%M:%S", lctime )
        if time.strftime( "%d", lctime ) != time.strftime( "%d", gmtime ):
            gmtstr = time.strftime( "%a, %d %b %Y %H:%M:%S", gmtime )
        else:
            gmtstr = time.strftime( "%H:%M:%S", gmtime )

        ''' Gets the terminal size and creates the separator '''
        (height, width) = terminalSize()
        separator = width*"*"

        ''' Displays the initial information '''
        msg = lctstr + " (LOCAL) " + gmtstr + " (UTC)"
        print separator
        print "START OF NEW TRACKED JOB".center( width )
        print separator
        print msg.center( width )
        print separator

        ''' Calls the function '''
        function( *args, **kwargs )

        ''' Gets the time after the function calling '''
        etime, lctime, gmtime = time.time(), time.localtime(), time.gmtime()
        lctstr = time.strftime( "%a, %d %b %Y %H:%M:%S", lctime )
        if time.strftime( "%d", lctime ) != time.strftime( "%d", gmtime ):
            gmtstr = time.strftime( "%a, %d %b %Y %H:%M:%S", gmtime )
        else:
            gmtstr = time.strftime( "%H:%M:%S", gmtime )

        ''' Displays the ending information '''
        msg = lctstr + " (LOCAL) " + gmtstr + " (UTC)"
        print separator
        print ("TRACKED JOB FINISHED (Time elapsed: %s)"
               % formatTime( etime - stime ) ).center( width )
        print separator
        print msg.center( width )
        print separator
    
    return wrapper
