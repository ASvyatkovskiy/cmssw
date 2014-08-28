#!/bin/csh
cd /afs/cern.ch/user/a/asvyatko/APEStudy2014/CMSSW_7_1_1/src/SegsInFits/SegsInFits
eval `scramv1 runtime -csh`
set CFG_FILE="GlbAlexey_corr_APEsOFF_100.py"
cmsRun $CFG_FILE
