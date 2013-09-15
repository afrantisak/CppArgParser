#!/usr/bin/env python
import os, sys

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
            
def main(options):            
    varName = 'CC'

    buildDirname = 'build'
    if 'CC' in os.environ and os.environ['CC']:
        buildDirname = os.environ['CC']

    originalDirname = os.getcwd()            
    _mkdir(buildDirname)
    os.chdir(buildDirname)

    returnValue = os.system('cmake .. && make')
    if returnValue:
        return returnValue
        
    os.chdir(originalDirname)
    
    formats = {'tests': ' '.join(options.tests), 'ref': options.ref }

    ret = os.system(sys.executable + ' testagg.py {tests} --ref {ref}'.format(**formats))
    ret = ret / 256
    return ret

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="Continuous Integration Build")
    parser.add_argument('tests', nargs='*', 
                        help="run these tests only")
    parser.add_argument('--ref', choices=['cmp', 'gen', 'none', 'dump'], default='cmp',
                        help="ref file operations (default=cmp)")
    #parser.add_argument('--deftimeout', type=float, default=20,
    #                    help="default timeout in seconds")
    #parser.add_argument('-N', '--line-numbers', action='store_true',
    #                    help="show line numbers when printing output")
    args = parser.parse_args()
    
    sys.exit(main(args))

