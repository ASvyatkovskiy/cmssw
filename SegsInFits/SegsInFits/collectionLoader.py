from ROOT import *
from DataFormats.FWLite import Events, Handle

ROOT.gSystem.Load("libFWCoreFWLite.so");
AutoLibraryLoader.enable()

events = Events([
"GlbSTART_APEsOFF_RECO/res/rcandidates_100_1_fPo.root",
])

events2 = Events([
"GlbSTART_APEsON_RECO/res/rcandidates_100_1_Dyj.root",
])

globalMuonsh=Handle("std::vector<reco::Track>")
globalMuonsh2=Handle("std::vector<reco::Track>")

#loop over the events
it1=events.__iter__()
it2=events2.__iter__()


#allocate histograms
pt_correlations = TH2F("pt_correlations","pt_correlations",1000,500,1500,1000,500,1500)
pt_correlations_phiin = TH2F("pt_correlations_phiin","pt_correlations_phiin",1000,500,1500,1000,500,1500)  #use absphi < 0.5
pt_correlations_phiout = TH2F("pt_correlations_phiout","pt_correlations_phiout",1000,500,1500,1000,500,1500) #use absphi > 0.5
relres_qinvpt_ebye = TH2F("relres_qinvpt_ebye","relres_qinvpt_ebye",200,-0.6,0.6,200,-0.6,0.6)

for i in xrange(events.size()):
  e1=it1.next()
  e1.getByLabel ('globalMuons', globalMuonsh)
  globalMuons1 = globalMuonsh.product()
  e2=it2.next()
  e2.getByLabel ('globalMuons', globalMuonsh2)
  globalMuons2 = globalMuonsh2.product()
  #print len(globalMuons1),len(globalMuons2)
  for track1,track2 in zip(globalMuons1,globalMuons2):
    #print track1.eta(),track2.eta()
    pt_correlations.Fill(track1.pt(),track2.pt()) 
    relres_qinvpt_ebye.Fill(track1.charge()/track1.pt(),track2.charge()/track2.pt())
    if fabs(track1.phi()) < 0.5: pt_correlations_phiin.Fill(track1.pt(),track2.pt())
    else: pt_correlations_phiout.Fill(track1.pt(),track2.pt()) 
   

f = TFile('correlationHistos.root','recreate')
f.cd()
pt_correlations.Write()
pt_correlations_phiin.Write()
pt_correlations_phiout.Write()
relres_qinvpt_ebye.Write()
f.Close()

#events = Events("1.root")
#events2 = Events("2.root")  
#for e1 in events:
#  e1.getByLabel ('globalMuons', globalMuonsh)
#  globalMuons1 = globalMuonsh.product()
#  print len(globalMuons1)
# 
#for e1 in events2:
#  e1.getByLabel ('globalMuons', globalMuonsh)
#  globalMuons1 = globalMuonsh.product()
#  print len(globalMuons1)
