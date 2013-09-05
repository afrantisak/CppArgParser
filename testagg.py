#!/usr/bin/env python
import sys, os
import subprocess
import datetime
import threading
import signal
import cStringIO
import yaml

def Timeout(task, seconds):
    thread = threading.Thread(target=task.run)
    thread.start()
    thread.join(seconds)
    if thread.is_alive():
        print "TIMED OUT"
        task.kill()
    return task.returncode()
    
def TimeoutProcess(procfunc, timeout):
    class Task():
        def run(self):
            self.proc = procfunc(subprocess.PIPE, None, subprocess.STDOUT)
            if self.proc:
                out, err = self.proc.communicate()
                # if it has a postprocess attribute, it is a function to process the output
                if hasattr(self.proc, 'postprocess'):
                    out = out.split('\n')[:-1]
                    out = [line + '\n' for line in out]
                    err = err.split('\n')[:-1]
                    err = [line + '\n' for line in err]
                    out += err
                    self.ret = self.proc.postprocess(out)
                else:
                    if out:
                        print out.rstrip()
                    if err:
                        print err.rstrip()
                    self.ret = 1
        def kill(self):
            if self.proc:
                # kill with extreme prejudice
                os.killpg(os.getpgid(self.proc.pid), signal.SIGKILL)
        def returncode(self):
            if hasattr(self, 'ret'):
                return self.ret
            elif self.proc:
                return self.proc.returncode
            elif self.proc == 0:
                return 0
            else:
                return 1
    return Timeout(Task(), timeout)
    
def testProcess(cmd, ref, refop, timeout):
    def Proc(stdout, stdin, stderr):
        if refop == 'gen':
            print "Generating ref file %s" % ref,
            with open(ref, 'w') as out:
                return subprocess.Popen(cmd, stdout = out, stderr = out, preexec_fn=os.setpgrp)
        elif refop == 'none':
            return subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, preexec_fn=os.setpgrp)
        else:
            if not os.path.exists(ref):
                print "Ref file %s does not exist." % ref
                print "Command: %s" % ' '.join(cmd)
                # now print the output of the test
                return subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, preexec_fn=os.setpgrp)
            if refop == 'dump':
                print "Command: %s" % ' '.join(cmd)
                for line in open(ref, 'r'):
                    print line.rstrip()
                return 0
            proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, preexec_fn=os.setpgrp)
            def postprocess(out):
                import difflib
                diff = difflib.context_diff(open(ref, 'r').readlines(), out, "Expected", "Actual", n=2)
                ret = 0
                for line in diff:
                    print line.rstrip()
                    ret = 1
                if ret:
                    print "Command: %s" % ' '.join(cmd)
                return ret
            proc.postprocess = postprocess
            return proc
    return TimeoutProcess(Proc, timeout)

def test(name, instruction, cmd, refop, timeout, options):
    print name,
    sys.stdout.flush()
    
    # capture stdout
    old_stdout = sys.stdout
    sys.stdout = mystdout = cStringIO.StringIO()

    ref = name + ".ref"
    if 'refdir' in options:
        ref = os.path.join(options['refdir'], ref)

    try:
        # run the job
        ret = -1
        if instruction == 'cmd':
            ret = testProcess(cmd, '', 'none', timeout)
        elif instruction == 'ref':
            ret = testProcess(cmd, ref, refop, timeout)
    except OSError as e:
        print str(e)
    except Exception as e:
        print str(e)
        
    # uncapture stdout
    sys.stdout = old_stdout

    if ret == -1:
        print "Invalid instruction '%s'" % (instruction)
    elif ret:
        print "FAILED"
    else:
        print "ok"

    # if there was anything printed to stdout, print it, nicely indented
    if mystdout.getvalue():
        linenumber = 0
        for line in mystdout.getvalue().split('\n'):
            indent = "    "
            linenumber += 1
            if options['line_numbers']:
                indent = "%4d" % linenumber
            print indent, line.rstrip()
            
    if ret is None:
        return 127
    return ret
    
def translate(value, defines):
    value = value.decode('string_escape')
    if 'CC' in os.environ:
        value = value.format(**defines)
    return value
    
def split(value):
    import shlex
    tokens = shlex.split(value)
    instruction = tokens[0]
    cmd = tokens[1:]
    return instruction, cmd
    
def recurse(data, tests, refop, timeout, options, defines):
    aggregate = 0
    found = 0
    failures = 0
    for key in sorted(data.keys()):
        value = data[key]
        cwd = os.path.abspath(os.getcwd())
        if 'cd' in value:
            os.chdir(value['cd'])
        if 'alias' in value:
            for alias in value['alias']:
                for k, v in alias.iteritems():
                    defines[k] = translate(v, defines)
        if 'tests' in value:
          value = value['tests']
          if type(value) == list:
            for item in value:
                if type(item) == dict:
                    aggregateLocal, foundLocal, failuresLocal = recurse(item, tests, refop, timeout, options, defines)
                    aggregate |= aggregateLocal
                    found += foundLocal
                    failures += failuresLocal
                    continue
            continue
        value = translate(value, defines)
        instruction, cmd = split(value)
        if tests and key not in tests:
            continue
        result = test(key, instruction, cmd, refop, timeout, options)
        aggregate |= result
        if result:
            failures += 1
        found += 1
        os.chdir(cwd)
    return aggregate, found, failures
    
def parse(testfilename, tests, refop, timeout, line_numbers):
    build = os.environ['CC']
    defines = { 'build': build }
    
    abspath = os.path.abspath(testfilename)
    data = yaml.safe_load(open(abspath, 'r'))
    options = {'line_numbers': line_numbers}
    if '.global' in data:
        options = data['.global']
        data.remove('.global')
    aggregate, found, failures = recurse(data, tests, refop, timeout, options, defines)
    if not found:
        print "No tests found"
        return 1
    else:
        print "Tests:  {found}".format(found=found)
        print "Passed: {passed}".format(passed=found - failures)
        if failures:
            print "FAILED: {failed}".format(failed=failures)
            return 1
        return 0
        
if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="test aggregator")
    parser.add_argument('--testfile', default="testagg.yml",
                        help="test yaml filename")
    parser.add_argument('tests', nargs='*', 
                        help="run these tests only")
    parser.add_argument('--ref', choices=['cmp', 'gen', 'none', 'dump'], default='cmp',
                        help="ref file operations (default=cmp)")
    parser.add_argument('--deftimeout', type=float, default=20,
                        help="default timeout in seconds")
    parser.add_argument('-N', '--line-numbers', action='store_true',
                        help="show line numbers when printing output")
    args = parser.parse_args()
    
    sys.exit(parse(args.testfile, args.tests, args.ref, args.deftimeout, args.line_numbers))
