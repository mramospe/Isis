#/////////////////////////////////////////////////////////////
#//
#//  Python modules
#//
#// ----------------------------------------------------------
#//
#//  AUTHOR: Miguel Ramos Pernas
#//  e-mail: miguel.ramos.pernas@cern.ch
#//
#//  Last update: 13/09/2017
#//
#// ----------------------------------------------------------
#//
#//  Description:
#//
#//  Defines some utils to manage python objects.
#//
#// ----------------------------------------------------------
#/////////////////////////////////////////////////////////////


from Isis.iboost.general import sendErrorMsg, sendWarningMsg

import os, fcntl, math, struct, termios, sys
import __builtin__


def dl2ld( dic ):
    '''
    Given a dictionary of lists, it returns a list of dictionaries
    '''
    firstkw = dic.keys()[0]
    length  = len(dic[firstkw])
    return [{kw: vals[i] for kw, vals in dic.iteritems()}
             for i in xrange(length)]


class EnvTracker:
    '''
    Class to keep track of the variables that change in a environment. Differences
    are considered with respect to the moment when the method "changes" is called.
    Call "update" to update the variables stored withing this class. At this point
    the objects stored in the "EnvVar" classes are destroyed if no other variable
    points to them.
    '''
    def __init__( self, env, title ):
        '''
        Takes the environment as a dictionary and its title
        '''
        self._env  = env
        self.title = title
        self.update()

    def _filterKeys( self ):
        '''
        Defines the keys that are considered to be tracked
        '''
        lst = self._env.keys()
        
        fltr = lambda s: all(not s.startswith(el) and
                             not s.endswith(el) for el in ('_', '__'))
        
        return filter(fltr, lst)

    def changes( self ):
        '''
        Return the variables added, removed and with a different assignment
        '''
        new_env_vars = [EnvVar(kw, self._env[kw]) for kw in self._filterKeys()]

        added, removed, reas_keys, reas_objs = self._envVars[0].check(new_env_vars)

        for var in self._envVars[1:]:
            add, rm, rkeys, robjs = var.check(new_env_vars)
            
            added      = set(added).intersection(add)
            removed   += rm
            reas_keys += rkeys
            reas_objs += robjs

        added = list(added)
        
        return added, removed, reas_keys, reas_objs
    
    def display( self ):
        '''
        Display the changes in the environment
        '''
        chgs = self.changes()

        msgs = [
            '* Variables added to the %s scope' %self.title,
            '* Variables removed from the %s scope' %self.title,
            '* Variables taking ownership of those removed in the %s scope' %self.title,
            '* Variables which have changed their value in the %s scope' %self.title
       ]
        
        for out, lst in zip(msgs, chgs):
            if lst:
                print len(out)*'*'
                print out

                lst = sorted(lst, key=lambda var: var.name)

                sizes   = map(lambda s: len(s.name), lst)
                maxsize = max(sizes)
        
                for i, (sz, var) in enumerate(zip(sizes, lst)):
                    print '* %s %s=> %s' %(var.name, (maxsize - sz)*' ', var.obj)
                print
            
    def update( self ):
        '''
        Update the keys of the environment
        '''
        self._envVars = [EnvVar(kw, self._env[kw]) for kw in self._filterKeys()]


class EnvVar:
    '''
    Class meant to be used with "EnvTracker", storing the name of the variable and
    the object it points to
    '''
    def __init__( self, name, obj ):
        '''
        Takes the name of the variable and the object
        '''
        self.name = name
        self.obj  = obj

    def check( self, lst ):
        '''
        Check the changes on this variable with respect to a new scope, with
        variables stored in "lst"
        '''
        pos = filter(lambda x: x.name == self.name, lst)
        
        reas_obj = []
        removed  = []
        if pos:
            if pos[0].obj != self.obj:
                reas_obj = [pos[0]]
        else:
            removed = [self]

        reas_keys = []
        added     = []
        for el in lst:
            if el.name != self.name:
                added.append(el)
                if el.obj == self.obj:
                    if removed:
                        reas_keys.append(el)
                        continue

        return added, removed, reas_keys, reas_obj

        
def formatTime( itime ):
    '''
    Displays the given time in the format [w, d, h, min, s]. If one of the
    quantities is zero, it is not displayed.
    '''
    conv  = [60, 60, 24, 7]
    vlist = ['s', 'min', 'h', 'd', 'w']
    vals  = {'s': itime, 'min': 0, 'h': 0, 'd': 0, 'w': 0}
    for cval, nunit, ounit in zip(conv, vlist[1:], vlist[:-1]):
        vals[nunit], vals[ounit] = divmod(int(vals[ounit]), cval)
    vlist.reverse()
    strout = ''
    for kw in vlist:
        val = vals[kw]
        if val:
            strout += str(vals[kw]) + kw + ' '
    if strout:
        return strout[:-1]
    else:
        return '0s'


def ld2dl( lst ):
    '''
    Given a list of dictionaries, it returns a dictionary of lists
    '''
    keys = lst[0].keys()
    return {kw: [el[kw] for el in lst] for kw in keys}


class ObjRegistry:
    '''
    Class to store objects and ensure they are not destructed. This register
    can also track another, so changes in the latter are also done in the first.
    '''
    def __init__( self, name, other = None, track = False ):
        '''
        Build a new register with the given name. A copy from another register
        can be made.
        '''
        self.name     = name
        self.register = {}
        self.tracks   = []

        if other != None:
            self.update(other, track)

    def __getitem__( self, name ):
        '''
        Get the desired object
        '''
        return self.register[name]
        
    def add( self, name, obj ):
        '''
        Add a new object to this class
        '''
        if name not in self.register:
            
            self.register[name] = obj
            
        else:
            
            if self[name] is obj:
                pass
            
            elif self[name] == obj:
                
                sendWarningMsg('{} => Reference "{}" already in use, but '
                               'objects look equivalent'
                               .format(self.name, name))
                self.register[name] = obj
                
            else:
                
                sendErrorMsg('{} => Reference "{}" already in use; new object '
                             'not stored'.format(self.name, name))
                return
            
        for t in self.tracks:
            t.add(name, obj)

    def build( self, name, objconst, args = None, kwargs = None ):
        '''
        Add a new object to this class using its constructor
        '''
        if args == None:
            args = ()
        if kwargs == None:
            kwargs = {}

        obj = objconst(*args, **kwargs)

        self.add(name, obj)

        return obj

    def remove( self, name ):
        '''
        Return the desired object and remove it from the register
        '''
        for t in self.tracks:
            t.remove(name)
            
        return self.register.pop(name)

    def update( self, other, track = False ):
        '''
        Update the current register with the objects from another. If "track" is
        set to "True", the other register is set to update this class after
        any modification.
        '''
        if track:
            other.tracks.append(self)
            
        for k, obj in other.register.iteritems():
            self.add(k, obj)


class PythonEnvMgr:
    '''
    This class allows to save/read environments to/from a file, which are
    saved as classes. This is very useful when peforming an analysis with many
    results from different categories that can not be easily written into any
    other kind of file.
    '''
    def __init__( self, filename, **kwargs ):
        '''
        The name of the input/output file must be provided. The option < addbot >
        takes control of the position to insert the new lines. If the option
        < overwrite > is set to True, a new file will be created, replacing the
        possible existing one.
        '''

        addbot = kwargs.get('addbot', True)
        ovrwrt = kwargs.get('overwrite', False)

        self.ifile = filename
        
        if addbot:
            self.mode = 'at+'
        else:
            self.mode = 'wt+'

        if ovrwrt or not os.path.isfile(filename):
            f = open(filename, 'wt')
            print 'Created new python file: <', filename, '>'
            f.close()
        else:
            print 'Updating python file: <', filename, '>'
        
    def _make_env_str( self, dic ):
        '''
        Internal function to create the string to be added to the output file
        '''
        outstr = ''
        maxlen = max(len(str(it)) for it in dic)
        srtkws = dic.keys()
        srtkws.sort()
        for kw in srtkws:
            whtsp   = (maxlen - len(kw))*' '
            outstr += '\t' + kw + whtsp + ' = ' + str(dic[kw]) + '\n'
        return outstr
    
    def readEnv( self, name, **kwargs ):
        '''
        Reads the asigned file. By default the class is returned, but it can
        be given as a dictionary if specified in **kwargs.
        '''
        removepath = kwargs.get('rmpath', True)
        returndict = kwargs.get('retdict', False)

        pathtofile = self.ifile.name().split('/')[:-1]
        if pathtofile not in sys.path:
            sys.path.append(pathtofile)

        mod = getattr(__import__(self.ifile.name), name)

        if removepath:
            del sys.path[-1]
        
        if returndict:
            return {kw: el for kw, el in mod.__dict__.iteritems()
                    if not kw.startswith('__')}
        else:
            return mod

    def saveEnv( self, name, **kwargs ):
        '''
        Method to save a set of values inside a class called < name > in the file
        attached to this class.
        '''
        ofile = open(self.ifile, self.mode)
        
        ofile.seek(0)
        lines = ofile.readlines()
        ofile.seek(0)
        clname = 'class ' + name + ':\n'

        if any(line.startswith(name) for line in lines):
            sendWarningMsg('Overwriting in < %s >; a variable called < %s > '\
                           'already exists' %(ofile.name, name))

        if clname in lines:

            sendWarningMsg('Replacing existing environment < %s > in '\
                           'file < %s >' %(name, ofile.name))
        
            ofile.truncate()
            start = lines.index(clname)
            end   = start + 1
        
            length = len(lines)
            while end != length and lines[end].startswith('\t'):
                end += 1
        
            for line in lines[:start]:
                ofile.write(line)
        
            ostr = self._make_env_str(kwargs)
            ofile.write(clname + ostr)
            
            for line in lines[end:]:
                ofile.write(line)
        else:
            ostr = self._make_env_str(kwargs)
            ofile.write(clname + ostr)

        ofile.close()


def stringListFilter( lst, pattern ):
    '''
    Given a list and a pattern ( with elements separated by '*' symbols ) it
    returns another list with those that satisfy it.
    '''
    pattern = pattern.split('*')
    checkstart = not pattern[ 0] == ''
    checkend   = not pattern[-1] == ''
    while '' in pattern:
        del pattern[pattern.index('')]
    output = []
    for el in lst:
        add = True
        if checkstart:
            if not el.startswith(pattern[0]):
                add = False
        if checkend:
            if not el.endswith(pattern[-1]):
                add = False
        if add:
            pos = 0
            for pt in pattern:
                newpos = el.find(pt)
                if newpos == -1 or newpos < pos:
                    add = False
                    break
                pos = newpos
            if add:
                output.append(el)
    return output


class StrNumGenerator:
    '''
    This is a generator of numbers represented as strings. It allows to perform
    iterations over numbers from < start > to < end >, filling the left side of
    them with as much zeros as necessary to make all the numbers in the row have
    the same size.
    '''
    def __init__( self, start, end = False ):
        '''
        If only one number is specified the start will be taken as zero. Only positive
        values are allowed.
        '''

        if not end:
            start, end = 0, start
        elif end < start:
            sendErrorMsg('The starting number must be greater than the ending')
        if end < 0 or start < 0:
            sendErrorMsg('Input parameters must be both positive')
            
        self._iter      = start
        self._maxIter   = end
        self._maxStrLen = len(str(end))

    def __iter__( self ):
        ''' On the iterations it returns itself '''
        return self

    def next( self ):
        ''' Moves one position forward the iterator '''
        if self._iter == self._maxIter:
            raise StopIteration
        else:
            citer = str(self._iter)
            self._iter += 1
            return (self._maxStrLen - len(citer))*'0' + citer


def terminalSize():
    '''
    Returns the values of the height and width of the terminal
    '''
    def gwinsz( fd ):
        try:
            cr = struct.unpack('hh', fcntl.ioctl(fd, termios.TIOCGWINSZ, '1234'))
        except:
            return
        return cr
    cr = gwinsz(0) or gwinsz(1) or gwinsz(2)
    if not cr:
        try:
            fd = os.open(os.ctermid(), os.O_RDONLY)
            cr = gwinsz(fd)
            os.close(fd)
        except:
            pass
    if not cr:
        cr = (os.environ.get('LINES', 25), os.environ.get('COLUMNS', 80))
    return int(cr[0]), int(cr[1])

