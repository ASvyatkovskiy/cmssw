#include <algorithm>
#include <sstream>
#include <string>
#include <vector>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH
#include "TH1F.h"
#include "TH2F.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/MuonDetId/interface/MuonSubdetId.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DataFormats/TrackReco/interface/HitPattern.h"

#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"

#include "DataFormats/GeometrySurface/interface/Cylinder.h"
#include "RecoMuon/TrackingTools/interface/MuonServiceProxy.h"

#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"
#include "Geometry/Records/interface/GlobalTrackingGeometryRecord.h"
#include "TrackingTools/TrajectoryState/interface/FreeTrajectoryState.h"

#include <math.h>

#define DEBUG_VERIFY false 
#define PI 3.14159265358979323846

struct match_by_deltaR_entry {
  match_by_deltaR_entry(int i_, int j_, double dR_) : i(i_), j(j_), dR(dR_) {}
  int i;
  int j;
  double dR;

  bool operator<(const match_by_deltaR_entry& other) const {
    return dR < other.dR;
  }
};

template <typename C1, typename C2>
void match_by_deltaR(const C1& src, const C2& dst, std::vector<int>& result,
		     std::ostream* debug=0) {
  std::vector<match_by_deltaR_entry> entries;
  typedef std::vector<match_by_deltaR_entry>::const_iterator entry_iterator;

  int ni = src.size();
  int nj = dst.size();

  result.clear();
  result.resize(ni, -1);

  for (int i = 0; i < ni; ++i)
    for (int j = 0; j < nj; ++j)
      entries.push_back(match_by_deltaR_entry(i, j, deltaR(src.at(i), dst.at(j))));

  std::sort(entries.begin(), entries.end());

  if (DEBUG_VERIFY)
    for (entry_iterator it = entries.begin(); it != entries.end(); ++it) 
      *debug << "match by dR entry: dR(" << it->i << ", " << it->j << ") = " << it->dR << std::endl;
  
  bool* i_used = new bool[ni];
  bool* j_used = new bool[nj];
  memset(i_used, 0, sizeof(bool)*ni);
  memset(j_used, 0, sizeof(bool)*nj);

  for (entry_iterator it = entries.begin(); it != entries.end(); ++it) {
    if (!i_used[it->i] && !j_used[it->j]) {
      result[it->i] = it->j;
      i_used[it->i] = j_used[it->j] = true;
      if (DEBUG_VERIFY) *debug << "matched src #" << it->i << " -> dst #" << it->j << " with dR = " << it->dR << std::endl;
    }
  }

  delete[] i_used;
  delete[] j_used;
}

using namespace std;
using namespace edm;
using namespace reco;

class MuonSegsInFits : public EDAnalyzer {
public:
  explicit MuonSegsInFits(const ParameterSet&);

private:
  virtual void analyze(const Event&, const EventSetup&);

  vector<InputTag> srcs;
  bool is_StA_run;
  bool doPhiCorr;

  enum { hits, segs, n_types };
  enum { barrel2d, barrel4d, barrel24d, endcap, overlap, n_loc };
  enum { pt10, pt100, pt1000, n_pt };

  int ptBin(const GenParticle& gen) {
    if (gen.pt() < 30)
      return pt10;
    else if (gen.pt() < 300)
      return pt100;
    else
      return pt1000;
  }
  
  int locBin(const GenParticle& gen) {
    float aeta = abs(gen.eta());
    if (aeta < 0.8)
      return barrel24d;
    else if (aeta < 1.24)
      return overlap;
    else
      return endcap;
  }

  TH2F* matched[n_types]; // [hits,segs]

  TH2F* dt_hits[n_loc];
  TH2F* csc_hits[n_loc];
  TH2F* rpc_hits[n_loc];

  TH1F* eff_vs_eta[n_types+1][n_pt]; // [gen,hits,segs][ptbin]

  TH1F* diff_charge[n_types][n_loc][n_pt]; // [hits,segs][locbin][ptbin]
  TH2F* diff_charge_phi[n_types][n_loc][n_pt];
  TH1F* relres_qinvpt[n_types][n_loc][n_pt];
  TH2F* relres_qinvpt_phi[n_types][n_loc][n_pt];

  TH1F* relres_pt[n_types][n_loc][n_pt];
  TH2F* relres_pt_phi[n_types][n_loc][n_pt];
  TH1F* pull_pt[n_types][n_loc][n_pt];
  TH2F* pull_pt_phi[n_types][n_loc][n_pt];

  //chi2-dofs
  TH1F* chi2dof[n_types][n_loc][n_pt];
  TH2F* chi2dof_eta[n_types][n_loc][n_pt];
  TH2F* chi2dof_phi[n_types][n_loc][n_pt];

  //residuals
  TH2F* res_pt_vs_phi[n_types][n_loc][n_pt];
  TH2F* res_qpt_vs_phi[n_types][n_loc][n_pt];

  //correlations
  TH2F* pt_correlations[n_loc][n_pt];
  TH1F* invpt_glb_trk[n_loc][n_pt];
  TH1F* qinvpt_glb_trk[n_loc][n_pt];
  TH2F* invpt_glb_trk_phi[n_loc][n_pt];
  TH2F* qinvpt_glb_trk_phi[n_loc][n_pt];

  TH2F* eta_correlations[n_loc][n_pt];
  TH2F* phi_correlations[n_loc][n_pt];

  TH1F* pull_qinvpt[n_types][n_loc][n_pt];
  TH2F* pull_qinvpt_phi[n_types][n_loc][n_pt];
  TH1F* absres_eta[n_types][n_loc][n_pt];
  TH1F* pull_eta[n_types][n_loc][n_pt];
  TH1F* absres_phi[n_types][n_loc][n_pt];
  TH1F* pull_phi[n_types][n_loc][n_pt];

  //basic
  TH1F* pt[n_types][n_loc][n_pt];
  TH1F* p[n_types][n_loc][n_pt];
  TH1F* eta[n_types][n_loc][n_pt];
  TH1F* phi[n_types][n_loc][n_pt];

  //alexey repeat e by e
  TH1F* relres_qinvpt_ebye[n_loc][n_pt];
  TH1F* pull_qinvpt_ebye[n_loc][n_pt];

  TString pt_names[n_pt];

};

MuonSegsInFits::MuonSegsInFits(const ParameterSet& cfg)
  : srcs(cfg.getParameter<vector<InputTag> >("srcs")),
  is_StA_run(cfg.getParameter<bool>("is_StA_run")),
  doPhiCorr(cfg.getParameter<bool>("doPhiCorr"))
{
  assert(srcs.size() == n_types);

  const TString type_names[n_types] = { "hits", "segs" };
  const TString loc_names[n_loc] = { "barrel2d", "barrel4d", "barrel24d", "endcap", "overlap" };
  //i defined it outside
  pt_names[0] = "pT0010";
  pt_names[1] = "pT0100";
  pt_names[2] = "pT1000";

  double invpt_scale[n_loc][n_pt] = {
    {0.1, 0.15, 0.6},
    {0.1, 0.15, 0.6},
    {0.1, 0.15, 0.6},
    {0.2, 0.5, 2},
    {0.15, 0.25, 1}
  };
 //for StA
  const double invpt_scale_StA[n_loc][n_pt] = {
    {0.1, 0.15, 1.5},
    {0.1, 0.15, 1.5},
    {0.1, 0.15, 1.5},
    {0.2, 0.5, 1.5},
    {0.15, 0.25,1.5}
  };

  if (is_StA_run) {
    for (int i = 0; i < n_loc; i++) {
       for (int j = 0; j < n_pt; j++) {
          invpt_scale[i][j] =  invpt_scale_StA[i][j];
      }
    }
  }

  const double eta_scale[n_pt] = {0.005, 0.003, 0.0025};
  const double pt_range[n_pt] = {20., 200., 2000.};
  const double eta_range[n_pt] = {5., 5., 5.};
  const double phi_range[n_pt] = {3.3, 3.3, 3.3};
  const double phi_scale[n_loc][n_pt] = {
    {0.0035, 0.0015, 0.0005},
    {0.0035, 0.0015, 0.0005},
    {0.0035, 0.0015, 0.0005},
    {0.005,  0.0015, 0.0015},
    {0.004,  0.001,  0.0004}
  };
  const double pull_scale = 10;
  const double pt_range_custom[n_pt] ={0.2,0.4,2.};
  const double pt_bins_custom[n_pt] ={100,200,1000};

  Service<TFileService> fs;

  for (int i = 0; i <= n_types; ++i) {
    TString prefix = i == n_types ? "gen" : type_names[i];
    for (int j = 0; j < n_pt; ++j)
      eff_vs_eta[i][j] = fs->make<TH1F>(prefix + "_vs_eta_" + pt_names[j], "", 25, -2.5, 2.5);
  }

  for (int i = 0; i < n_loc; ++i) {
    dt_hits[i]  = fs->make<TH2F>(loc_names[i] + "_dt_hits",  "", 48, 0, 48, 10, 0, 10);
    csc_hits[i] = fs->make<TH2F>(loc_names[i] + "_csc_hits", "", 48, 0, 48, 10, 0, 10);
    rpc_hits[i] = fs->make<TH2F>(loc_names[i] + "_rpc_hits", "", 10, 0, 10, 10, 0, 10);
  }

  //per pt_bin, per loc, but both types
  for (int j = 0; j < n_loc; ++j) {
    for (int k = 0; k < n_pt; ++k) {
         TString prefix = loc_names[j] + "_" + pt_names[k] + "_";
 
       pt_correlations[j][k] = fs->make<TH2F>(prefix + "pt_correlations", "", 1000, 0., pt_range[k],1000, 0., pt_range[k]);
       invpt_glb_trk[j][k] = fs->make<TH1F>(prefix + "invpt_glb_trk", "", 200, -5.*invpt_scale[j][k], 4.*invpt_scale[j][k]);
       qinvpt_glb_trk[j][k] = fs->make<TH1F>(prefix + "qinvpt_glb_trk", "", 200, -5.*invpt_scale[j][k], 4.*invpt_scale[j][k]);
       invpt_glb_trk_phi[j][k] = fs->make<TH2F>(prefix + "invpt_glb_trk_phi", "", 200, -5.*invpt_scale[j][k], 4.*invpt_scale[j][k], 66,-phi_range[k], phi_range[k]);
       qinvpt_glb_trk_phi[j][k] = fs->make<TH2F>(prefix + "qinvpt_glb_trk_phi", "", 200, -5.*invpt_scale[j][k], 4.*invpt_scale[j][k], 66,-phi_range[k], phi_range[k]);

       eta_correlations[j][k] = fs->make<TH2F>(prefix + "eta_correlations", "", 100, -eta_range[k], eta_range[k],100, -eta_range[k], eta_range[k]);
       phi_correlations[j][k] = fs->make<TH2F>(prefix + "phi_correlations", "", 66,-phi_range[k], phi_range[k],66,-phi_range[k], phi_range[k]);
       relres_qinvpt_ebye[j][k] = fs->make<TH1F>(prefix + "relres_qinvpt_ebye", "", pt_bins_custom[k], -pt_range_custom[k], pt_range_custom[k]); 
       pull_qinvpt_ebye[j][k] = fs->make<TH1F>(prefix + "pull_qinvpt_ebye",   "", 1000, -0.1*pull_scale, 0.1*pull_scale);
    }
  }

  for (int i = 0; i < n_types; ++i) {
    matched[i] = fs->make<TH2F>(type_names[i] + "_matched", "", 10, 0, 10, 10, 0, 10);

    for (int j = 0; j < n_loc; ++j) {
      for (int k = 0; k < n_pt; ++k) {
	TString prefix = type_names[i] + "_" + loc_names[j] + "_" + pt_names[k] + "_";
	
	diff_charge  [i][j][k] = fs->make<TH1F>(prefix + "diff_charge",   "", 5, -2, 3);
        diff_charge_phi  [i][j][k] = fs->make<TH2F>(prefix + "diff_charge_phi",   "", 5, -2, 3, 66,-phi_range[k], phi_range[k]);
	relres_qinvpt[i][j][k] = fs->make<TH1F>(prefix + "relres_qinvpt", "", 200, -5.*invpt_scale[j][k], 4.*invpt_scale[j][k]);
        relres_qinvpt_phi[i][j][k] = fs->make<TH2F>(prefix + "relres_qinvpt_phi", "", 200, -5.*invpt_scale[j][k], 4.*invpt_scale[j][k],66,-phi_range[k], phi_range[k]);
        relres_pt[i][j][k] = fs->make<TH1F>(prefix + "relres_pt", "", 400, -5.*invpt_scale[j][k], 4.*invpt_scale[j][k]);
        relres_pt_phi[i][j][k] = fs->make<TH2F>(prefix + "relres_pt_phi", "", 400, -5.*invpt_scale[j][k], 4.*invpt_scale[j][k],66,-phi_range[k], phi_range[k]);
        if (is_StA_run) {
           res_pt_vs_phi[i][j][k] = fs->make<TH2F>(prefix + "res_pt_vs_phi", "", 1000, -1.5*pt_range[k], 1.5*pt_range[k],66,-phi_range[k], phi_range[k]);
           res_qpt_vs_phi[i][j][k] = fs->make<TH2F>(prefix + "res_qpt_vs_phi", "", 1000, -1.5*pt_range[k], 1.5*pt_range[k],66,-phi_range[k], phi_range[k]);
        } else {
           res_pt_vs_phi[i][j][k] = fs->make<TH2F>(prefix + "res_pt_vs_phi", "", 1000, -pt_range[k], pt_range[k],66,-phi_range[k], phi_range[k]);
           res_qpt_vs_phi[i][j][k] = fs->make<TH2F>(prefix + "res_qpt_vs_phi", "", 1000, -pt_range[k], pt_range[k],66,-phi_range[k], phi_range[k]);
        } 
        //basic
        pt[i][j][k] = fs->make<TH1F>(prefix + "pt", "", 2000, 0, pt_range[k]);
        p[i][j][k] = fs->make<TH1F>(prefix + "p", "", 2000, 0, pt_range[k]);
        eta[i][j][k] = fs->make<TH1F>(prefix + "eta", "", 100, -eta_range[k], eta_range[k]);
        phi[i][j][k] = fs->make<TH1F>(prefix + "phi", "", 66,-phi_range[k], phi_range[k]);

	absres_eta   [i][j][k] = fs->make<TH1F>(prefix + "absres_eta",    "", 100, -eta_scale[k], eta_scale[k]);
	absres_phi   [i][j][k] = fs->make<TH1F>(prefix + "absres_phi",    "", 100, -phi_scale[j][k], phi_scale[j][k]);
	pull_qinvpt  [i][j][k] = fs->make<TH1F>(prefix + "pull_qinvpt",   "", 100, -pull_scale, pull_scale);
        pull_qinvpt_phi  [i][j][k] = fs->make<TH2F>(prefix + "pull_qinvpt_phi",   "", 100, -pull_scale, pull_scale, 66,-phi_range[k], phi_range[k]);
	pull_eta     [i][j][k] = fs->make<TH1F>(prefix + "pull_eta",      "", 100, -pull_scale, pull_scale);
	pull_phi     [i][j][k] = fs->make<TH1F>(prefix + "pull_phi",      "", 100, -pull_scale, pull_scale);
        pull_pt      [i][j][k] = fs->make<TH1F>(prefix + "pull_pt",       "", 100, -pull_scale, pull_scale);
        pull_pt_phi      [i][j][k] = fs->make<TH2F>(prefix + "pull_pt_phi",       "", 100, -pull_scale, pull_scale,66,-phi_range[k], phi_range[k]);
        //chi2dofs
        chi2dof      [i][j][k] = fs->make<TH1F>(prefix + "chi2dof",       "", 100, 0,50);
        chi2dof_eta  [i][j][k] = fs->make<TH2F>(prefix + "chi2dof_eta",   "", 100, 0,50,100, -eta_range[k], eta_range[k]);
        chi2dof_phi  [i][j][k] = fs->make<TH2F>(prefix + "chi2dof_phi",   "", 100, 0,50,66, -phi_range[k], phi_range[k]);

      }
    }
  }
}

void MuonSegsInFits::analyze(const Event& event, const EventSetup& eSetup) {
  if (DEBUG_VERIFY) printf("\nrun, event: %i, %i\n", event.id().run(), event.id().event());

  Handle<GenParticleCollection> gens;
  event.getByLabel("genParticles", gens);

  const int ptbin = ptBin(gens->at(0)); // only single muon gun, so all gen pts are same

  if (DEBUG_VERIFY) printf("gen size %i\n", int(gens->size()));
  int igen = 0;
  foreach (const GenParticle& gen, *gens) {
    if (DEBUG_VERIFY) printf("gen %i pt %f eta %f phi %f\n", igen++, gen.pt(), gen.eta(), gen.phi());
    eff_vs_eta[n_types][ptbin]->Fill(gen.eta());
  }
 
  //FIXME 1
  Handle<MuonCollection> tks[n_types];
  vector<int> gen_match[n_types];

  for (int i = 0; i < n_types; ++i) {
    event.getByLabel(srcs[i], tks[i]);
    if (DEBUG_VERIFY) printf("tks[%i] (%s) size: %i\n", i, srcs[i].encode().c_str(), int(tks[i]->size()));

    if (DEBUG_VERIFY) printf("doing matching to gen:\n");
    match_by_deltaR(*tks[i], *gens, gen_match[i], &cout);

    int matches = 0;
    for (int j = 0; j < int(tks[i]->size()); ++j) {
      if (gen_match[i][j] != -1)
	matches++;
      if (DEBUG_VERIFY) printf("tks[%i][%i] pt: %f eta: %f phi: %f match to gen: %i\n", i, j, tks[i]->at(j).pt(), tks[i]->at(j).eta(), tks[i]->at(j).phi(), gen_match[i].at(j));
      eff_vs_eta[i][ptbin]->Fill(tks[i]->at(j).eta());
    }

    matched[i]->Fill(tks[i]->size(), matches);
  }


  for (int i = 0; i < int(tks[hits]->size()); ++i) {
    for (int j = 0; j < int(tks[segs]->size()); ++j) {
      int gmi = gen_match[0][i];
      int gmj = gen_match[1][j];
      if (gmi != -1 && gmj != -1 && gmi == gmj) {
	if (DEBUG_VERIFY) printf("analysis loop found i: %i j: %i gm: %i\n", i, j, gmi);

	const GenParticle& gen = gens->at(gmi);
        //FIXME 2 - note the change of type 
        //test0 - default
	//reco::TrackRef tki = tks[hits]->at(i).globalTrack();
	//reco::TrackRef tkj = tks[segs]->at(j).globalTrack();
        //test1
        if (!tks[segs]->at(i).isGlobalMuon() || !tks[hits]->at(i).isTrackerMuon()) continue;  //|| !(!tks[hits]->at(i).isGlobalMuon() && tks[hits]->at(i).isTrackerMuon())) continue;
        //printf("is Global1 %i, is Tracker1 %i \n",tks[segs]->at(i).isGlobalMuon(),tks[segs]->at(i).isTrackerMuon()); 
        //printf("is Global2 %i, is Tracker2 %i \n",tks[hits]->at(i).isGlobalMuon(),tks[hits]->at(i).isTrackerMuon());

        reco::TrackRef tki = tks[hits]->at(i).innerTrack();
        reco::TrackRef tkj = tks[segs]->at(j).globalTrack();
        //FIXME 3
        if (!tki.isNonnull() || !tkj.isNonnull()) continue;

        const Track* ts[2] = { &*tki, &*tkj };

	const HitPattern& hpi = tki->hitPattern();
	const HitPattern& hpj = tkj->hitPattern();

	int loc = locBin(gens->at(0));

	if (loc == barrel24d) {
	  int dim[2] = { 0, 0 };
	  for (int k = 0; k < int(tkj->recHitsSize()); ++k) {
	    if (!tkj->recHit(k)->isValid())
	      continue;
	    
	    DetId did = tkj->recHit(k)->geographicalId();
	    if (did.det() != DetId::Muon || did.subdetId() != MuonSubdetId::DT)
	      continue;

	    int d = tkj->recHit(k)->dimension();
	    if (d == 2)
	      dim[0]++;
	    else if (d == 4)
	      dim[1]++;
	    else {
	      if (DEBUG_VERIFY) printf("funny dimension %i, rechit %i, segs tk %i!\n", d, k, j);
	      dim[0]++;
	      dim[1]++;
	      break;
	    }
	  }

	  if (dim[0] == 0)
	    loc = barrel4d;
	  else if (dim[1] == 0)
	    loc = barrel2d;

	  if (DEBUG_VERIFY) printf("segs track %i in barrel with loc code %i\n", j, loc);
	}

	dt_hits [loc]->Fill(hpi.numberOfValidMuonDTHits(),  hpj.numberOfValidMuonDTHits());
	csc_hits[loc]->Fill(hpi.numberOfValidMuonCSCHits(), hpj.numberOfValidMuonCSCHits());
	rpc_hits[loc]->Fill(hpi.numberOfValidMuonRPCHits(), hpj.numberOfValidMuonRPCHits());

        //const Track& tk_hit = *ts[0];
        //const Track& tk_seg = *ts[1];
        
        //correlations
        double phiMultiplicativeCorr[2] = {1.,1.};
        if (doPhiCorr) { 
            phiMultiplicativeCorr[0] = (1-(*ts[0]).charge()*cos((*ts[0]).phi())*0.25);
            phiMultiplicativeCorr[1] = (1-(*ts[1]).charge()*cos((*ts[1]).phi())*0.25);
        }

        pt_correlations[loc][ptbin]->Fill(phiMultiplicativeCorr[0]*((*ts[0]).pt()),phiMultiplicativeCorr[1]*((*ts[1]).pt()));
        //Alexey new one 
        invpt_glb_trk[loc][ptbin]->Fill(1/(phiMultiplicativeCorr[0]*((*ts[0]).pt()))-1/(phiMultiplicativeCorr[1]*((*ts[1]).pt())));
        qinvpt_glb_trk[loc][ptbin]->Fill((*ts[0]).charge()/(phiMultiplicativeCorr[0]*((*ts[0]).pt()))-(*ts[1]).charge()/(phiMultiplicativeCorr[1]*((*ts[1]).pt())));
        invpt_glb_trk_phi[loc][ptbin]->Fill(1/(phiMultiplicativeCorr[0]*((*ts[0]).pt()))-1/(phiMultiplicativeCorr[1]*((*ts[1]).pt())),(*ts[0]).phi());
        qinvpt_glb_trk_phi[loc][ptbin]->Fill((*ts[0]).charge()/(phiMultiplicativeCorr[0]*((*ts[0]).pt()))-(*ts[1]).charge()/(phiMultiplicativeCorr[1]*((*ts[1]).pt())),(*ts[0]).phi());


        eta_correlations[loc][ptbin]->Fill((*ts[0]).eta(),(*ts[1]).eta());
        phi_correlations[loc][ptbin]->Fill((*ts[0]).phi(),(*ts[1]).phi());

        relres_qinvpt_ebye[loc][ptbin]->Fill(phiMultiplicativeCorr[0]*((*ts[0]).pt()) - phiMultiplicativeCorr[1]*((*ts[1]).pt()));
        pull_qinvpt_ebye[loc][ptbin]->Fill(((*ts[0]).charge()/(phiMultiplicativeCorr[0]*((*ts[0]).pt())) - (*ts[1]).charge()/(phiMultiplicativeCorr[1]*((*ts[1]).pt())))/((*ts[0]).ptError()/(((*ts[0]).pt())*phiMultiplicativeCorr[0])/(phiMultiplicativeCorr[0]*((*ts[0]).pt()))));

	for (int k = 0; k < 2; ++k) {

	  const Track& tk = *ts[k];
          double phiMultCorr[2] = {1.,1.};
          if (doPhiCorr) {    
              phiMultCorr[0] = (1-tk.charge()*cos(tk.phi())*0.25);
              phiMultCorr[1] = (1-tk.charge()*cos(tk.phi())*0.25);
          }

          //phi mode corrected 
	  double tkqinvpt = tk.charge()/(tk.pt()*phiMultCorr[k]);
	  double tkqinvpterr = tk.ptError()/(tk.pt()*phiMultCorr[k])/(tk.pt()*phiMultCorr[k]);
	  double genqinvpt = gen.charge()/gen.pt();
          double tkpt = (phiMultCorr[k]*tk.pt());
          double genpt = gen.pt();
          double tkp = tk.p();
          double tketa = tk.eta();
          double tkphi = tk.phi();

          diff_charge[k][loc][ptbin]->Fill(tk.charge() - gen.charge());
          diff_charge_phi[k][loc][ptbin]->Fill(tk.charge() - gen.charge(),tkphi);

          //added by request of ivan
          res_pt_vs_phi[k][loc][ptbin]->Fill(tkpt-genpt,gen.phi());	 
          res_qpt_vs_phi[k][loc][ptbin]->Fill(tk.charge()*tkpt-gen.charge()*genpt,gen.phi());  
          pt[k][loc][ptbin]->Fill(tkpt); 
          p[k][loc][ptbin]->Fill(tkp); 
          eta[k][loc][ptbin]->Fill(tketa); 
          phi[k][loc][ptbin]->Fill(tkphi); 
          //printf("phi %f, eta %f, pt %f\n",tkphi,tketa,tkpt);
        
	  relres_qinvpt[k][loc][ptbin]->Fill((tkqinvpt - genqinvpt)/genqinvpt);
          relres_qinvpt_phi[k][loc][ptbin]->Fill((tkqinvpt - genqinvpt)/genqinvpt,tkphi);
	  pull_qinvpt  [k][loc][ptbin]->Fill((tkqinvpt - genqinvpt)/tkqinvpterr);
          pull_qinvpt_phi  [k][loc][ptbin]->Fill((tkqinvpt - genqinvpt)/tkqinvpterr,tkphi);
	  
	  absres_eta[k][loc][ptbin]->Fill( tk.eta() - gen.eta());
	  pull_eta  [k][loc][ptbin]->Fill((tk.eta() - gen.eta())/tk.etaError());
	
	  absres_phi[k][loc][ptbin]->Fill( tk.phi() - gen.phi());
	  pull_phi  [k][loc][ptbin]->Fill((tk.phi() - gen.phi())/tk.phiError()); 

          //pt resolruions and pulls 
          relres_pt[k][loc][ptbin]->Fill((tk.pt()-gen.pt())/tk.pt());
          relres_pt_phi[k][loc][ptbin]->Fill((tk.pt()-gen.pt())/tk.pt(),tkphi);
          pull_pt[k][loc][ptbin]->Fill((tk.pt()-gen.pt())/tk.ptError());
          pull_pt_phi[k][loc][ptbin]->Fill((tk.pt()-gen.pt())/tk.ptError(),tkphi);


          //chi2dofs
          chi2dof[k][loc][ptbin]->Fill(tk.normalizedChi2());
          chi2dof_eta[k][loc][ptbin]->Fill(tk.normalizedChi2(),gen.eta());
          chi2dof_phi[k][loc][ptbin]->Fill(tk.normalizedChi2(),gen.phi());
	}
      }
    }
  }
}
DEFINE_FWK_MODULE(MuonSegsInFits);
