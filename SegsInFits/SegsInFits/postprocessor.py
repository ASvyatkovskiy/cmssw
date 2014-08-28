#!/usr/bin/env python

from subprocess import Popen

#families = {'pT1000':['OFF','ON'],'pT0100':['OFF','ON'],'pT0010':['OFF','ON']}
#
#for run in families.keys():
#    for ape in families[run]:
#        #print 'python draw_Glb.py zp2mu_histos_'+ape+'_'+run+'.root'
#        Popen('python draw_Glb.py zp2mu_histos_'+ape+'_'+run+'.root',shell=True).wait()


#families = {'200um':['0um','100um','200um','500um','1000um','2000um'],'1000um':['0um','100um','200um','500um','1000um','2000um']}
families = {'200um':['0um','200um','2000um'],'1000um':['0um','200um','2000um']}
#families = {'200um':['200um']}

for shift in families.keys():
    for ape in families[shift]:
        print 'python draw_Glb.py zp2mu_histos_ON_pT1000_'+shift+'_xAPE_'+ape+'.root'
        Popen('python draw_Glb.py zp2mu_histos_ON_pT1000_'+shift+'_xAPE_'+ape+'.root',shell=True).wait()
