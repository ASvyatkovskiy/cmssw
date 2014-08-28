#!/usr/bin/env python

from subprocess import Popen
import sys

#configs = ['crab_0_110805_225119','crab_0_110805_225138','crab_0_110805_225206','crab_0_110811_195508']

for cfg in sys.argv[1:]:
    #print 'cd '+cfg+'/res/ \n hadd '+cfg+'.root *.root'
    Popen('rm '+cfg+'/res/'+cfg+'.root',shell=True).wait()
    Popen('rm '+cfg,shell=True).wait()
    Popen('hadd '+cfg+'.root '+cfg+'/res/*.root',shell=True).wait()
