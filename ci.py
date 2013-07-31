#!/usr/bin/env python
import os, sys

varName = 'CC'

if varName in os.environ:
    buildDirname = os.environ['CC']
else:
    buildDirname = 'build'

def _mkdir(newdir):
    """works the way a good mkdir should :)
        - already exists, silently complete
        - regular file in the way, raise an exception
        - parent directory(ies) does not exist, make them as well
    """
    if os.path.isdir(newdir):
        pass
    elif os.path.isfile(newdir):
        raise OSError("a file with the same name as the desired " \
                      "dir, '%s', already exists." % newdir)
    else:
        head, tail = os.path.split(newdir)
        if head and not os.path.isdir(head):
            _mkdir(head)
        #print "_mkdir %s" % repr(newdir)
        if tail:
            os.mkdir(newdir)
            
originalDirname = os.getcwd()            
_mkdir(buildDirname)
os.chdir(buildDirname)

returnValue = os.system('cmake .. && make')
if returnValue:
    sys.exit(returnValue)
    
os.chdir(originalDirname)

returnValue = os.system(sys.executable + ' testagg.py')
sys.exit(returnValue)
