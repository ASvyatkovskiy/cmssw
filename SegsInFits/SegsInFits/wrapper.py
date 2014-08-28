#!/usr/bin/env python

from subprocess import Popen

#Popen('bsub -q 1nd -J job1 < run10OFF.csh',shell=True).wait()
#Popen('bsub -q 1nd -J job2 < run100OFF.csh',shell=True).wait()
#Popen('bsub -q 1nd -J job3 < run1000OFF.csh',shell=True).wait()
#Popen('bsub -q 1nd -J job4 < run10ON.csh',shell=True).wait()
#Popen('bsub -q 1nd -J job5 < run100ON.csh',shell=True).wait()
#Popen('bsub -q 1nd -J job6 < run1000ON.csh',shell=True).wait()


#families = {'200um':['0um','100um','200um','500um','1000um','2000um'],'1000um':['0um','100um','200um','500um','1000um','2000um']}
families = {'200um':['0um','200um','2000um'],'1000um':['0um','200um','2000um']}

for shift in families.keys():
    for ape in families[shift]:
        print 'bsub -q 1nd -J job'+shift+'_'+ape+' < run1000ON_'+shift+'_xAPE_'+ape+'.csh'
        Popen('bsub -q 1nd -J job'+shift+'_'+ape+' < run1000ON_'+shift+'_xAPE_'+ape+'.csh',shell=True).wait()
