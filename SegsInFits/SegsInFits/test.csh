# Lxplus Batch Job Script
set CMSSW_PROJECT_SRC="/afs/cern.ch/user/a/asvyatko"
set CFG_FOLDER = "APEStudy2014/CMSSW_7_1_1/src/SegsInFits/SegsInFits"
set CFG_FILE="GlbAlexey_corr_APEsOFF_nophi.py"
set OUTPUT_FILE="zp2mu_histos.root"
set TOP="$PWD"

echo $CMSSW_PROJECT_SRC
echo $CFG_FOLDER
cd $CMSSW_PROJECT_SRC
cd $CFG_FOLDER
echo $PWD
ls -l
eval `scramv1 runtime -csh`
echo $CMSSW_RELEASE_BASE 
cmsRun $CFG_FILE
rfcp $OUTPUT_FILE $CMSSW_PROJECT_SRC
