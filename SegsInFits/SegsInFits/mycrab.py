#!/usr/bin/env python

# author: J. Tucker

import glob, os, re, sys, time
import xml.etree.cElementTree
from collections import defaultdict
from ConfigParser import ConfigParser, NoOptionError

debug = True
if os.path.exists('/uscms_data'):
    crab_working_dir_default_root = '/uscms_data/d2/tucker/crab_dirs'
else:
    print 'warning: not using crab_working_dir_default_root!'
    crab_working_dir_default_root = None

def print_run_cmd(cmd):
    print cmd
    os.system(cmd)

def bool_from_argv(s, remove=True, return_pos=False):
    val = s in sys.argv
    ret = val
    if val and return_pos:
        ret = val, sys.argv.index(s) 
    if val and remove:
        sys.argv.remove(s)
    return ret

def crabify_list(l):
    return ','.join(str(x) for x in sorted(l))

def decrabify_list(s):
    if s.lower() == 'all':
        print 'warning: decrabify_list does not support "all"'
        return []
    s = s.split(',')
    l = []
    for x in s:
        if '-' in x:
            y,z = x.split('-')
            l += range(int(y), int(z)+1)
        else:
            l.append(int(x))
    return l

def crab_jobs_from_argv():
    jobs = []
    for x in sys.argv:
        was_list = False
        try:
            l = decrabify_list(x)
            if l:
                jobs += l
                was_list = True
        except ValueError:
            pass
        if not was_list:
            try:
                j = int(x)
                jobs.append(j)
            except ValueError:
                pass
    jobs.sort()
    return jobs

def is_crab_working_dir(path):
    return os.path.isdir(path) and os.path.isdir(os.path.join(path, 'job'))

def crab_working_dirs(path=''):
    return [d for d in glob.glob(os.path.join(path, 'crab_*')) if is_crab_working_dir(d)]
    
def last_crab_working_dir(path=''):
    dirs = [(d, os.stat(d).st_mtime) for d in crab_working_dirs(path)]
    if dirs:
        dirs.sort(key=lambda x: x[1])
        return dirs[-1][0]

def crab_move_working_dirs(dirs=None, dest_dir=None):
    if dirs is None:
        dirs = crab_working_dirs()

    if dest_dir is None:
        dest_dir = crab_working_dir_default_root

    if dest_dir is None:
        print 'warning: dest_dir is None in crab_move_working_dirs(), giving up'
        return

    if not os.path.isdir(dest_dir):
        print 'making crab working dir root at', dest_dir
        os.mkdir(dest_dir) # let it throw if it didn't work

    for d in crab_working_dirs():
        # if the dir is a link, then don't try to move it.
        try:
            os.readlink(d)
        except OSError:
            print_run_cmd('mv %s %s && ln -s %s' % (d, dest_dir, os.path.join(dest_dir, d)))

def crab_dirs_from_argv():
    dirs = []

    if 'all' in sys.argv:
        dirs = crab_working_dirs()
    elif len(sys.argv) > 1:
        dirs = [d for d in sys.argv[1:] if is_crab_working_dir(d)]

    if not dirs:
        dirs = [last_crab_working_dir()]

    # make sure each entry is in only once, but can't use
    # list(set(dirs)) because we want to keep the sorted order
    r = []
    for d in dirs:
        if d not in r:
            r.append(d)
        
    return r

def crab_dir_from_argv():
    dir = crab_dirs_from_argv()
    assert len(dir) == 1
    return dir[0]

class CrabError(Exception):
    pass

def crab_cleanup(extra=[]):
    to_remove = [] #['crab.history', 'SiteDBusername.conf', 'server_legnaro.conf'] + extra #+['glite_wms_CERN.conf', 'glite.conf.CMS_CERN']
    for f in to_remove:
        if os.path.isfile(f):
            os.remove(f)
    
def crab_status(working_dir):
    d = defaultdict(list)
    
    cmd = 'crab -c %s -status' % working_dir
    if debug: print cmd
    s = os.popen4(cmd)[1].read() # subprocess more like suckprocess
    
    if 'Total Jobs' not in s:
        raise CrabError, 'unable to get status for working_dir=' + working_dir

    for x in s.split('\n'):
        x = [y.strip() for y in x.split() if y.strip()]
        if len(x) < 4: continue

        try:
            x[0] = int(x[0])
        except ValueError:
            pass
        else:
            id, end, status, action = x[:4]
            if len(x) == 7:
                exe_exit_code = int(x[4])
                job_exit_code = int(x[5])
                e_host = x[6]
            elif len(x) == 6:
                exe_exit_code = -1
                job_exit_code = int(x[4])
                e_host = x[5]
            elif len(x) == 5:
                exe_exit_code = job_exit_code = -1
                e_host = x[4]
            elif len(x) == 4:
                exe_exit_code = job_exit_code = -1
            else:
                raise ValueError('trouble parsing job line: repr(x) = %s' % repr(x))
                
            if status == 'Retrieved':
                key = '%s_%i_%i' % (status, exe_exit_code, job_exit_code)
            else:
                key = status
            d[key].append(id)

    if debug:
        print s
        for k in sorted(d.keys()):
            print '%s: %s' % (k.ljust(25), crabify_list(d[k]))
            
    return d

def crab_get_output(working_dir, l=None, n=500):
    if l is None:
        l = crab_status(working_dir)['Done']
    if l:
        for i in xrange(0, len(l), n):
            ll = l[i:i+n]
            ll = crabify_list(ll)
            print_run_cmd('crab -c %s -getoutput %s' % (working_dir, ll))

def crab_create_submit(cfg, jobs):
    print_run_cmd('crab -cfg %s -create -submit %s' % (cfg, jobs))

def crab_kill(working_dir, jobs):
    print_run_cmd('crab -c %s -kill %s' % (working_dir, crabify_list(jobs)))
    
def crab_resubmit(working_dir, jobs):
    print_run_cmd('crab -c %s -resubmit %s' % (working_dir, crabify_list(jobs)))

def crab_cfg_parser(working_dir):
    c = ConfigParser()
    c.read(os.path.join(working_dir, 'share/crab.cfg'))
    return c

def crab_get_outfiles(working_dir):
    for line in open(os.path.join(working_dir, 'job/CMSSW.sh')):
        if line.startswith('file_list'):
            line = line.split('"')[-2]
            line = line.replace('$SOFTWARE_DIR/', '')
            line = line.replace('$NJob', '%i')
            return line.split(',')
    
def crab_get_njobs(working_dir):
    log = open(os.path.join(working_dir, 'log/crab.log')).read()
    sub = 'Total of '
    try:
        ndx = log.index(sub)
    except IndexError:
        raise RuntimeError, 'could not get total # of jobs from crab.log!'
    return int(log[ndx+len(sub):].split(' ', 1)[0])

def crab_get_output_dir(working_dir):
    c = crab_cfg_parser(working_dir)
    try:
        cast_dir = c.get('USER', 'storage_path').split('=')[-1] + c.get('USER', 'user_remote_dir').split('=')[-1]
        cast_dir = cast_dir.replace('//','/')
    except NoOptionError:
        cast_dir = None
    return cast_dir

def crab_get_condor_clusters(working_dir, cluster_re=re.compile(r'\((\d+)\.')):
    clusters = set()
    for fn in glob.glob(os.path.join(working_dir, 'share/.condor_temp/CMSSW_*.log')):
        for line in open(fn):
            mo = cluster_re.search(line)
            if mo:
                clusters.add(int(mo.group(1)))
    return sorted(clusters)

def crab_get_output_files(working_dir, _re=re.compile(r'\$SOFTWARE_DIR/(.*?)[,"]')):
    c = crab_cfg_parser(working_dir)

    # Get file list from cfg file, if it's there (we may have let crab
    # determine the output files for us from TFileService and
    # PoolOutputModule, in which case the line won't be in the cfg
    # file.
    files = set()
    try:
        for x in c.get('CMSSW', 'output_file').split(','):
            x = x.strip()
            if x:
                files.add(x.replace('.','_%i.'))
    except NoOptionError:
        pass

    # Find any files that weren't in the cfg file by parsing the
    # script (this is fragile!).
    for line in open(os.path.join(working_dir, 'job/CMSSW.sh')):
        if 'file_list' in line:
            break
    for x in _re.findall(line):
        files.add(x.replace('$NJob', '%i'))
        
    return list(sorted(files))
    
def crab_check_output(working_dir):
    d = crab_status(working_dir)

    to_resub = []
    
    if d.has_key('Aborted'):
        # crab now requires killing jobs first!
        crab_kill(working_dir, d['Aborted'])
        to_resub.extend(d['Aborted'])

    if os.environ.has_key('MYCRAB_RESUB_ANY'):
        for k in d.keys():
            if 'Retrieved' in k and not k.endswith('0_0'):
                raw_input('i found this key %s and these jobs %s to resubmit, ok?' % (k, crabify_list(d[k])))
                to_resub.extend(d[k])
            
    #for code in [8001, 8020, ('10020',), ('0','60307')]:
    #    if type(code) != type(()):
    #        code = str(code)
    #        code = code, code
    #    key = 'Retrieved_%s' % '_'.join(code)
    #    if d.has_key(key):
    #        to_resub.extend(d[key])

    if to_resub:
        for job in to_resub:
            crab_cleanup_aborted_job(working_dir, job)
        crab_resubmit(working_dir, to_resub)

    if d.has_key('Done'):
        crab_get_output(working_dir, d['Done'])
        
    return d

def crab_status_dict(dirs=None):
    if dirs is None:
        dirs = crab_dirs_from_argv()
    results = {}
    for d in dirs:
        results[d] = crab_status(d)
    return results

def crab_arguments_xml(path, jobs_into_dicts=True):
    if os.path.isdir(path):
        filename = os.path.join(path, 'share/arguments.xml')
    else:
        filename = path
    ret = []
    for event, elem in xml.etree.cElementTree.iterparse(filename):
        if elem.tag == 'Job':
            if jobs_into_dicts:
                ret.append(dict(elem.items()))
            else:
                ret.append(elem.items()[:])
        elif elem.tag in ['arguments']:
            pass
        else:
            raise ValueError('tag %s not handled' % elem.tag)
    return dict((int(j['JobID']), j) for j in ret)

def crab_lumis_from_arguments_xml(working_dir):
    from condor import parse_good_lumis
    args = crab_arguments_xml(working_dir)
    jobs = args.values()
    lumis = defaultdict(list)
    for job in jobs:
        these_lumis = job['Lumis'].split(',')
        parsed = parse_good_lumis(these_lumis)
        for r,l in parsed.iteritems():
            lumis[r] += l
    return lumis

def crab_runs_from_arguments_xml(working_dir):
    return sorted(crab_lumis_from_arguments_xml(working_dir).keys())

def crab_cleanup_aborted_job(working_dir, job):
    d = os.path.join(working_dir, 'res')
    files = glob.glob(os.path.join(d, '*_%i.*' % job)) # this won't get e.g. ntuple_%(job)i_%(try)i_%(rand)s.root
    print 'cleanup_aborted_job for %s, job %i' % (working_dir, job)
    if not files:
        print 'nothing to do!'
        return
    print 'files:', ' '.join(files)
    new_d = os.path.join(working_dir, 'res/job_%i_cleanup_%i' % (job, time.time()))
    os.mkdir(new_d)
    print 'move to:', new_d
    for f in files:
        os.system('mv %s %s/' % (f, new_d))

def crab_get_input_files(working_dir, jobs):
    args = crab_arguments_xml(working_dir)
    return dict((j, args[j]['InputFiles'].split(',')) for j in jobs)
        
if __name__ == '__main__':
    import sys

    from pprint import pprint
    from FWCore.PythonUtilities.LumiList import LumiList

    if bool_from_argv('-cleanUp'):
        print 'executing crab_cleanup()'
        crab_cleanup()

    elif bool_from_argv('-expectedLumis'):
        print 'writing out JSONs for expected lumis:'
        for d in crab_dirs_from_argv():
            print d
            ll = LumiList(runsAndLumis=crab_lumis_from_arguments_xml(d))
            ll.writeJSON(os.path.join(d, 'res/expectedLumis.json'))

    elif bool_from_argv('-condorClusters'):
        pprint([(d, crab_get_condor_clusters(d)) for d in crab_dirs_from_argv()])

    elif bool_from_argv('-matchCondorClusters'):
        # Get current clusters.
        clusters = defaultdict(list)
        for line in os.popen('condor_q -submitter tucker'):
            line = line.split()
            if len(line) < 2 or line[1] != 'tucker':
                continue
            cluster, job = line[0].split('.')
            cluster, job = int(cluster), int(job)
            clusters[cluster].append(job)
        print 'current clusters:', sorted(clusters)

        dirs = [(d, crab_get_condor_clusters(d)) for d in crab_working_dirs()]
        print 'crab dirs in current directory and their clusters:'
        pprint(dirs)
        print '\nthese dirs have currently running clusters:'

        # Print sorting by most jobs to least.
        x = []
        for d, dclusters in dirs:
            cs = sorted(set(clusters.keys()).intersection(dclusters))
            if cs:
                x.append((d, cs, sum(len(clusters[c]) for c in cs)))
        x.sort(key=lambda x: x[-1], reverse=True)
        for d, cs, ltot in x:
            print d
            for c in cs:
                l = len(clusters[c])
                print '%6i: %i job%s' % (c, l, 's' if l > 1 else '')

    elif bool_from_argv('-filesForJobs'):
        dir = crab_dir_from_argv()
        print dir
        d = crab_get_input_files(dir, crab_jobs_from_argv())
        for k,v in d.iteritems():
            print k
            for f in v:
                print f

