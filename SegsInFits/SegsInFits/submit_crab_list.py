#!/usr/bin/env python

from subprocess import Popen
move_only = True

if not move_only:
    #Popen('crab -create -submit -cfg crab_GlbAlexey_corr_APEsOFF_nophi.cfg',shell=True).wait()
    #Popen('crab -create -submit -cfg crab_GlbAlexey_corr_APEsON_nophi.cfg',shell=True).wait()
    #Popen('crab -create -submit -cfg crab_muonGlbAlexey_corr_APEsOFF_nophi.cfg',shell=True).wait()
    #Popen('crab -create -submit -cfg crab_muonGlbAlexey_corr_APEsON_nophi.cfg',shell=True).wait()
    #Popen('crab -create -submit -cfg crab_GlbIDEAL_APEsOFF_nophi.cfg',shell=True).wait()
    #Popen('crab -create -submit -cfg crab_GlbIDEAL_APEsON_nophi.cfg',shell=True).wait()
    #Popen('crab -create -submit -cfg crab_muonGlbIDEAL_APEsOFF_nophi.cfg',shell=True).wait()
    #Popen('crab -create -submit -cfg crab_muonGlbIDEAL_APEsON_nophi.cfg',shell=True).wait()
    Popen('crab -create -submit -cfg crab_Glb_IDEALMS_APEsOFF_nophi.cfg',shell=True).wait()
    Popen('crab -create -submit -cfg crab_Glb_IDEALMS_APEsON_nophi.cfg',shell=True).wait()
    Popen('crab -create -submit -cfg crab_muonGlb_IDEALMS_APEsOFF_nophi.cfg',shell=True).wait()
    Popen('crab -create -submit -cfg crab_muonGlb_IDEALMS_APEsON_nophi.cfg',shell=True).wait()
else:
    Popen('mv crab_0_120828_221752.root GlbAlexey_corr_APEsOFF_nophi_1000.root',shell=True).wait()
    Popen('mv crab_0_120828_221821.root GlbAlexey_corr_APEsON_nophi_1000.root',shell=True).wait()
    Popen('mv crab_0_120828_221846.root muonGlbAlexey_corr_APEsOFF_nophi_1000.root',shell=True).wait()
    Popen('mv crab_0_120828_221913.root muonGlbAlexey_corr_APEsON_nophi_1000.root',shell=True).wait()
    Popen('mv crab_0_120828_221942.root GlbIDEAL_APEsOFF_nophi_1000.root',shell=True).wait()
    Popen('mv crab_0_120828_222008.root GlbIDEAL_APEsON_nophi_1000.root',shell=True).wait()
    Popen('mv crab_0_120828_222035.root muonGlbIDEAL_APEsOFF_nophi_1000.root',shell=True).wait()
    Popen('mv crab_0_120828_222102.root muonGlbIDEAL_APEsON_nophi_1000.root',shell=True).wait()
    Popen('mv crab_0_120828_230631.root Glb_IDEALMS_APEsOFF_nophi_1000.root',shell=True).wait()
    Popen('mv crab_0_120828_230658.root Glb_IDEALMS_APEsON_nophi_1000.root',shell=True).wait()
    Popen('mv crab_0_120828_230723.root muonGlb_IDEALMS_APEsOFF_nophi_1000.root',shell=True).wait()
    Popen('mv crab_0_120828_230750.root muonGlb_IDEALMS_APEsON_nophi_1000.root',shell=True).wait()
