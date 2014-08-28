#!/usr/bin/env python

import sys, os, glob
import array
sys.argv.append('-b')
import ROOT
from math import sqrt

#FUNCTION DEFINITIONS

def rainbow_palette(num_colors=500):
    r = array.array('d', [0, 0, 0, 1, 1])
    g = array.array('d', [0, 1, 1, 1, 0])
    b = array.array('d', [1, 1, 0, 0, 0])
    stops = array.array('d', [float(i)/4 for i in xrange(5)])
    ROOT.TColor.CreateGradientColorTable(5, stops, r, g, b, num_colors)
    ROOT.gStyle.SetNumberContours(num_colors)
    ROOT.gStyle.SetOptStat(1000000001);

def clopper_pearson(n_on, n_tot, alpha=0.05, equal_tailed=True):
    if equal_tailed:
        alpha_min = alpha/2
    else:
        alpha_min = alpha

    lower = 0
    upper = 1

    if n_on > 0:
        lower = ROOT.Math.beta_quantile(alpha_min, n_on, n_tot - n_on + 1)
    if n_tot - n_on > 0:
        upper = ROOT.Math.beta_quantile_c(alpha_min, n_on + 1, n_tot - n_on)

    return lower, upper

binomial_interval = lambda n,s,t: clopper_pearson(s,t)

def binomial_divide(h1, h2):
    nbins = h1.GetNbinsX()
    xax = h1.GetXaxis()
    if h2.GetNbinsX() != nbins: # or xax2.GetBinLowEdge(1) != xax.GetBinLowEdge(1) or xax2.GetBinLowEdge(nbins) != xax.GetBinLowEdge(nbins):
        raise ValueError, 'incompatible histograms to divide'
    x = []
    y = []
    exl = []
    exh = []
    eyl = []
    eyh = []
    xax = h1.GetXaxis()
    for ibin in xrange(1, nbins+1):
        s,t = h1.GetBinContent(ibin), h2.GetBinContent(ibin)
        # alexey addition
        if (t != 0): 
            p_hat = float(s)/t
        else: 
            p_hat = 1.
        if s > t:
            print 'warning: bin %i has p_hat > 1, in interval forcing p_hat = 1' % ibin
            s = t
        a,b = binomial_interval('Clopper-Pearson', s, t)
        #print ibin, s, t, a, b

        _x  = xax.GetBinCenter(ibin)
        _xw = xax.GetBinWidth(ibin)/2
        
        x.append(_x)
        exl.append(_xw)
        exh.append(_xw)

        y.append(p_hat)
        eyl.append(p_hat - a)
        eyh.append(b - p_hat)
    return ROOT.TGraphAsymmErrors(len(x), *[array.array('d', obj) for obj in (x,y,exl,exh,eyl,eyh)])

class hurr:
    whiches = ['GLB','Picky'] #['STA'] #,'GLB','FMS']
    
    def __init__(self, fn, which):
        self.which = which
        self.file = ROOT.TFile(sys.argv[1])
        self.hists = getattr(self.file, '%ssegsInFits' % which)

        self.plots_dir = os.path.join('plots'+str(sys.argv[1]).rstrip('.root'), {'STA': 'Standalone', 'FMS': 'TPFMS', 'GLB': 'Global','Picky':'Picky'}[which])
        os.system('mkdir -p %s' % self.plots_dir)

        rainbow_palette()
        self.canvas = ROOT.TCanvas("c","", 500, 500)
        self.canvas.SetRightMargin(0.13)

        self.hacky_crap = False

    def plot_fn(self, name):
        return os.path.join(self.plots_dir, name)

    def save_it(self, name):
#histogram names (almost)
        gagas = [('csc', 'dt', 'rpc'), 'matched', 'vs_eta', 'diff_charge', 'relres_qinvpt','absres_eta', 'absres_phi', 'pull_qinvpt', 'pull_eta', 'pull_phi','res_pt_vs_phi','pt','p','eta','phi','relres_qinvpt_phi']

        for i,g in enumerate(gagas):
            if type(g) != type(()):
                g = (g,)
            if [x for x in g if x in name]:
                name = '%02i_%s' % (i, name)
        fn = self.plot_fn('%s.png' % name)
        if self.hacky_crap:
            fn = fn.replace('.png', '.pdf')
            self.hacky_crap = False
        self.canvas.SaveAs(fn)

    @classmethod
    def make_title(cls, name, which):
        if which == 'FMS':
            t = 'TPFMS'
        elif which == 'GLB':
            t = 'Global'
        elif which == 'STA':
            t = 'Standalone'
        elif which == 'Picky':
            t = 'Picky'
        else:
            t = '????????'

        if 'hits' in name:
            t += ' hits fit'
        else:
            t += ' segments fit'

        if 'pT0010' in name:
            t += ', pT = 10 GeV'
        elif 'pT0100' in name:
            t += ', pT = 100 GeV'
        elif 'pT1000' in name:
            t += ', pT = 1 TeV'

        if 'barrel24d' in name:
            t += ', barrel (2&4D seg.)'
        elif 'barrel2d' in name:
            t += ', barrel (2D seg.)'
        elif 'barrel4d' in name:
            t += ', barrel (4D seg.)'
        elif 'endcap' in name:
            t += ', endcap'
        elif 'overlap' in name:
            t += ', overlap'

        if 'dt_hits' in name:
            t = t.replace('hits fit', 'DT hits/segments')
        elif 'csc_hits' in name:
            t = t.replace('hits fit', 'CSC hits/segments')
        elif 'rpc_hits' in name:
            t = t.replace('hits fit', 'RPC hits')

        return t
        
    def analyze(self):
        for k in self.hists.GetListOfKeys():
            name = k.GetName()
             #skip empty histograms 

            #if 'barrel24d' not in name or 'segs' not in name: continue

            if 'pT1000' in str(sys.argv[1]) and '0100' in name: continue
            elif 'pT1000' in str(sys.argv[1]) and '0010' in name: continue
            elif 'pT0100' in str(sys.argv[1]) and '1000' in name: continue
            elif 'pT0100' in str(sys.argv[1]) and '0010' in name: continue
            elif 'pT0010' in str(sys.argv[1]) and '1000' in name: continue
            elif 'pT0010' in str(sys.argv[1]) and '0100' in name: continue

            h = self.hists.Get(name)
            #if 'whoknows' in name or ('barrel' in name and not '24' in name): continue
            #if 'vs_eta' not in name: continue
            #if 'vs_eta' in name: continue
            #if 'diff_charge' not in name: continue
            #if self.which == 'STA' and 'barrel24d' in name: continue
            print h, h.GetEntries()

            if 'correlations' not in name and 'res_pt_vs_phi' not in name:              
                h.SetTitle(self.make_title(name, self.which) + '  ')

            #if 'correlations' in name or 'res_pt_vs_phi' in name:
            if 'relres_qinvpt_ebye' in name or 'res_pt_vs_phi' in name:
                h.SetStats(0)
                if h.GetEntries() !=0: 
                     if 'endcap' in name: 
                         print "fractions endcap ", h.GetName(), " ", float(h.GetEntries() - h.GetBinContent(0) - h.GetBinContent(h.GetXaxis().GetNbins()+1))/h.GetEntries()
                         print h.GetBinCenter(1), " ", h.GetBinCenter(h.GetXaxis().GetNbins())
                     elif 'barrel24' in name:   
                         print "fractions barrel24d ", h.GetName(), " ", float(h.GetEntries() - h.GetBinContent(0) - h.GetBinContent(h.GetXaxis().GetNbins()+1))/h.GetEntries()
 

            log_scale = False     
  
            #if ( ('pT1000_p' in name and 'pT1000_pt' not in name and 'pT1000_pull' not in name) or 'pull_eta' in name or 'phi' in name or 'pT1000_p.' in name or 'pT1000_absres_eta' in name or 'chi2dof_eta' in name or 'diff_charge_phi' in name or 'absres_phi' in name or 'pull_phi' in name or 'pull_qinvpt_phi' in name or 'res_pt_vs_phi' in name or 'pull_pt_phi' in name or 'relres_pt_phi' in name or 'res_qpt_vs_phi' in name or 'chi2dof_phi' in name or 'relres_qinvpt_phi' in name ):  continue

            if 'matched' in name:
                h.Draw('text')
                h.GetXaxis().SetTitle('# reconstructed muons')
                h.GetYaxis().SetTitle('# matched to gen. muons')
            elif 'vs_eta' in name:
                if 'gen' in name:
                    continue
                gen_name = 'gen_' + name.split('_',1)[1]
                gen_h = self.hists.Get(gen_name)
                e = binomial_divide(h, gen_h)
                e.SetTitle(h.GetTitle())
                e.GetXaxis().SetTitle('gen. #eta')
                e.GetYaxis().SetTitle('efficiency')
                e.SetMarkerSize(0.2)
                e.Draw('AP')
                # alexey addition
                tot_eff_pct = 0.
                tot_eff_pct_barrel = 0.
                if (gen_h.GetEntries() > 0): 
                    #split by barrel and endcap
                    tot_eff_pct = float(h.Integral())/gen_h.Integral()*100
                    tot_eff_pct_barrel = float(h.Integral(7,19))/gen_h.Integral(7,19)*100
                    #print float(h.Integral())/gen_h.Integral(1,25)*100, " ", float(h.Integral(7,19))/gen_h.Integral(7,19)*100, " ", float(h.Integral(1,7)+h.Integral(1,7))/(gen_h.Integral(19,25)+h.Integral(19,25))*100
                    #print h.GetBinCenter(7), " ", h.GetBinCenter(19),
                else: 
                    tot_eff_pct = 0.
                #ltx = ROOT.TLatex(0.3, 0.3, 'total #varepsilon = %.1f #pm %.1f%%' % (tot_eff_pct,tot_eff_pct_err))
                ltx = ROOT.TLatex(0.3, 0.43, '#varepsilon_{tot} = %.1f #pm 0.1%%' % tot_eff_pct)
                ltx.SetTextSize(0.06)
                #print 'total #varepsilon = %.1f #pm %.1f%%' % (tot_eff_pct,tot_eff_pct_err)
                #print 'total #varepsilon = %.1f%%' % tot_eff_pct 
                ltx.SetNDC()
                ltx.Draw()
                ltx1 = ROOT.TLatex(0.3, 0.37, '#varepsilon_{barrel} = %.1f #pm 0.1%%' % tot_eff_pct_barrel)
                ltx1.SetTextSize(0.06)
                #print 'total #varepsilon = %.1f #pm %.1f%%' % (tot_eff_pct,tot_eff_pct_err)
                #print 'total #varepsilon = %.1f%%' % tot_eff_pct 
                ltx1.SetNDC()
                ltx1.Draw()

                #self.hacky_crap = True
            #alexey add 
            elif 'res_pt_vs_phi' in name:
                if 'pT1000' in str(sys.argv[1]): h.GetXaxis().SetRangeUser(-500,500) 
                elif 'pT0100' in str(sys.argv[1]): h.GetXaxis().SetRangeUser(-50,50)
                elif 'pT0010' in str(sys.argv[1]): h.GetXaxis().SetRangeUser(-5,5)
                else: print 'WRONG FILE NAME'
                h.SetTitle(h.GetTitle())
                h.GetXaxis().SetTitle('#Delta p_{T}')
                h.GetYaxis().SetTitle('#phi')
                h.Draw('colz')
            elif 'res_pt_vs_phi' in name or 'relres_qinvpt_phi' in name:
                if 'pT1000' in str(sys.argv[1]): h.GetXaxis().SetRangeUser(-0.6,0.6)
                elif 'pT0100' in str(sys.argv[1]): h.GetXaxis().SetRangeUser(-0.1,0.1)
                elif 'pT0010' in str(sys.argv[1]): h.GetXaxis().SetRangeUser(-0.01,0.01)
                else: print 'WRONG FILE NAME'
                h.SetTitle(h.GetTitle())
                h.GetXaxis().SetTitle('q/pT rel. res.')
                h.GetYaxis().SetTitle('#phi')
                h.Draw('colz')
            elif 'p_correlation' in name:
                if 'pT1000' in str(sys.argv[1]): 
                    h.GetXaxis().SetRangeUser(400,1600)    
                    h.GetYaxis().SetRangeUser(400,1600)
                elif 'pT0100' in str(sys.argv[1]): 
                    h.GetXaxis().SetRangeUser(60,140)
                    h.GetYaxis().SetRangeUser(60,140)
                elif 'pT0010' in str(sys.argv[1]): 
                    h.GetXaxis().SetRangeUser(8,12)
                    h.GetYaxis().SetRangeUser(8,12)
                else: print 'WRONG FILE NAME'
                h.GetXaxis().SetTitle('p from segments')
                h.GetYaxis().SetTitle('p from hits')
                h.Draw('colz')
            elif 'pt_correlation' in name:
                if 'pT1000' in str(sys.argv[1]): 
                    h.GetXaxis().SetRangeUser(400,1600)
                    h.GetYaxis().SetRangeUser(400,1600)
                elif 'pT0100' in str(sys.argv[1]): 
                    h.GetXaxis().SetRangeUser(60,140)
                    h.GetYaxis().SetRangeUser(60,140)
                elif 'pT0010' in str(sys.argv[1]): 
                    h.GetXaxis().SetRangeUser(8,12)
                    h.GetYaxis().SetRangeUser(8,12)
                else: print 'WRONG FILE NAME'
                h.GetXaxis().SetTitle('p_{T} from segments')
                h.GetYaxis().SetTitle('p_{T} from hits')
                h.Draw('colz')
            elif 'eta_correlation' in name:
                h.GetXaxis().SetTitle('#eta from segments')
                h.GetYaxis().SetTitle('#eta from hits')
                h.Draw('colz')
            elif 'phi_correlation' in name:
                h.GetXaxis().SetTitle('#phi from segments')
                h.GetYaxis().SetTitle('#phi from hits')
                h.Draw('colz')
            elif 'csc' in name or 'dt' in name or 'rpc' in name:
                h.Draw('colz')
                h.SetStats(0)
                h.GetXaxis().SetTitle('# muon hits')
                if 'rpc' in name:
                    h.GetYaxis().SetTitle('# muon hits (fit using segments)')
                else:
                    h.GetYaxis().SetTitle('# muon segments')
            elif 'charge' in name:
                h.GetXaxis().SetTitle('#Delta q (rec - MC)')
                h.SetMarkerSize(2)
                h.Draw('hist text00')

                if h.GetEntries() > 0:
                    wrong_q = h.GetBinContent(1) + h.GetBinContent(5)
                    right_q = h.GetBinContent(3)
                    
                    intv = binomial_interval('Clopper-Pearson', wrong_q, wrong_q + right_q)
                    intv = intv[0]*100, intv[1]*100
                
                    ltx = ROOT.TLatex(0.11, 0.3, 'q miss.: %.2f%% - %.2f%% (68%%CL)' % intv)
                    ltx.SetNDC()
                    ltx.Draw()
                
                log_scale = True
            else:
                #ALL THAT IS FIT
                #h.GetXaxis().SetLabelSize(3)
                if 'absres_eta' in name:
                    h.GetXaxis().SetTitle('#eta abs. res, rec - MC')
                elif 'absres_phi' in name:
                    h.GetXaxis().SetTitle('#phi abs. res, rec - MC [rad]')
                elif 'relres_qinvpt' in name:
                    print "RIGHT ANSWER"
                    h.GetXaxis().SetTitle('q/pT rel. res, (rec - MC)/MC')
                    h.GetXaxis().SetRangeUser(-0.6,0.6) #FIXME
                    if 'ebye' in name: h.GetXaxis().SetTitle('#Delta pT, p_{T}^{hits}-p_{T}^{segs}') 
                elif 'pull_eta' in name:
                    h.GetXaxis().SetTitle('#eta pull, (rec - MC)/#sigma_{#eta}')
                elif 'pull_phi' in name:
                    h.GetXaxis().SetTitle('#phi pull, (rec - MC)/#sigma_{#phi}')
                elif 'pull_qinvpt' in name:
                    h.GetXaxis().SetTitle('q/pT pull, (rec - MC)/#sigma_{q/pT}')
                    if 'ebye' in name: h.GetXaxis().SetTitle('q/pT pull, (hits - segs)/#sigma_{q/pT}')
                if 'ebye' not in name:
                    if 'pull' in name:
                        if which == 'GLB' or which == 'Picky' or which == 'FMS':
                            h.SetStats(0)
                            #FIXME test 
                            if 'endcap' in name:
                                h.Fit('gaus', 'R','',-5,5)
                            else:
                                h.Fit('gaus', 'q')
                            #print 'BLAAAA ', h.GetName(), ' ', h.GetFunction('gaus').GetParameter(0), ' ', h.GetFunction('gaus').GetParameter(1),' ', h.GetFunction('gaus').GetParameter(2)
                            ltx = ROOT.TLatex(0.62, 0.83, '#sigma = %.7f' % h.GetFunction('gaus').GetParameter(2))
                            ltxe = ROOT.TLatex(0.62, 0.77, '#pm %.7f' % h.GetFunction('gaus').GetParError(2))
                            ltx1 = ROOT.TLatex(0.62, 0.71, '#mu = %.7f' % h.GetFunction('gaus').GetParameter(1))
                            ltx1e = ROOT.TLatex(0.62, 0.65, '#pm %.7f' % h.GetFunction('gaus').GetParError(1))
                    elif 'relres' in name: 
                        if which == 'GLB' or which == 'Picky' or which == 'FMS':
                            if 'pT1000' in str(sys.argv[1]):
                                if 'endcap' in name: 
                                    h.SetStats(0)
                                    h.Fit('gaus', 'R','',-0.2,0.2)
                                    ltx = ROOT.TLatex(0.62, 0.83, 'RMS = %.4f' % h.GetRMS())
                                    ltx1 = ROOT.TLatex(0.62, 0.77, '#sigma = %.7f' % h.GetFunction('gaus').GetParameter(2))
                                    ltx1e = ROOT.TLatex(0.62, 0.71, '#pm %.7f' % h.GetFunction('gaus').GetParError(2))
                                elif 'overlap' in name: 
                                    h.SetStats(0)
                                    h.Fit('gaus', 'R','',-0.2,0.2)
                                    ltx = ROOT.TLatex(0.62, 0.83, 'RMS = %.4f' % h.GetRMS())
                                    ltx1 = ROOT.TLatex(0.62, 0.77, '#sigma = %.7f' % h.GetFunction('gaus').GetParameter(2))
                                    ltx1e = ROOT.TLatex(0.62, 0.71, '#pm %.7f' % h.GetFunction('gaus').GetParError(2))
                                elif 'barrel' in name: #h.Fit('gaus', 'R','',-0.3,0.3)
                                    h.SetStats(0)
                                    h.Fit('gaus', 'R','',-0.3,0.3)
                                    ltx = ROOT.TLatex(0.62, 0.83, 'RMS = %.4f' % h.GetRMS())
                                    ltx1 = ROOT.TLatex(0.62, 0.77, '#sigma = %.7f' % h.GetFunction('gaus').GetParameter(2))
                                    ltx1e = ROOT.TLatex(0.62, 0.71, '#pm %.7f' % h.GetFunction('gaus').GetParError(2))
                            elif 'pT0100' in str(sys.argv[1]):
                                if 'endcap' in name: #h.Fit('gaus', 'R','',-0.45,0.45) 
                                    h.SetStats(0)
                                    h.Fit('gaus', 'R','',-0.1,0.1)
                                    ltx = ROOT.TLatex(0.62, 0.83, 'RMS = %.4f' % h.GetRMS())
                                    ltx1 = ROOT.TLatex(0.62, 0.77, '#sigma = %.7f' % h.GetFunction('gaus').GetParameter(2))
                                    ltx1e = ROOT.TLatex(0.62, 0.71, '#pm %.7f' % h.GetFunction('gaus').GetParError(2))
                                elif 'overlap' in name: 
                                    h.SetStats(0)
                                    h.Fit('gaus', 'R','',-0.05,0.05)
                                    ltx = ROOT.TLatex(0.62, 0.83, 'RMS = %.4f' % h.GetRMS())
                                    ltx1 = ROOT.TLatex(0.62, 0.77, '#sigma = %.7f' % h.GetFunction('gaus').GetParameter(2))
                                    ltx1e = ROOT.TLatex(0.62, 0.71, '#pm %.7f' % h.GetFunction('gaus').GetParError(2))
                                elif 'barrel' in name: #h.Fit('gaus', 'R','',-0.3,0.3)
                                    h.SetStats(0)
                                    h.Fit('gaus', 'R','',-0.05,0.05)
                                    ltx = ROOT.TLatex(0.62, 0.83, 'RMS = %.4f' % h.GetRMS())
                                    ltx1 = ROOT.TLatex(0.62, 0.77, '#sigma = %.7f' % h.GetFunction('gaus').GetParameter(2))
                                    ltx1e = ROOT.TLatex(0.62, 0.71, '#pm %.7f' % h.GetFunction('gaus').GetParError(2))
                            elif 'pT0010' in str(sys.argv[1]):
                                if 'endcap' in name: #h.Fit('gaus', 'R','',-0.45,0.45) 
                                    h.SetStats(0)
                                    h.Fit('gaus', 'R','',-0.05,0.05)
                                    ltx = ROOT.TLatex(0.62, 0.83, 'RMS = %.4f' % h.GetRMS())
                                    ltx1 = ROOT.TLatex(0.62, 0.77, '#sigma = %.7f' % h.GetFunction('gaus').GetParameter(2))
                                    ltx1e = ROOT.TLatex(0.62, 0.71, '#pm %.7f' % h.GetFunction('gaus').GetParError(2))
                                elif 'overlap' in name: 
                                    h.SetStats(0)
                                    h.Fit('gaus', 'R','',-0.045,0.045)
                                    ltx = ROOT.TLatex(0.62, 0.83, 'RMS = %.4f' % h.GetRMS())
                                    ltx1 = ROOT.TLatex(0.62, 0.77, '#sigma = %.7f' % h.GetFunction('gaus').GetParameter(2))
                                    ltx1e = ROOT.TLatex(0.62, 0.71, '#pm %.7f' % h.GetFunction('gaus').GetParError(2))
                                elif 'barrel' in name: 
                                    h.SetStats(0)
                                    h.Fit('gaus', 'R','',-0.02,0.02)
                                    ltx = ROOT.TLatex(0.62, 0.83, 'RMS = %.4f' % h.GetRMS())
                                    ltx1 = ROOT.TLatex(0.62, 0.77, '#sigma = %.7f' % h.GetFunction('gaus').GetParameter(2))
                                    ltx1e = ROOT.TLatex(0.62, 0.71, '#pm %.7f' % h.GetFunction('gaus').GetParError(2))
                    #elif '_chi2dof' in name:
                    #    h.GetXaxis().SetRangeUser(0,10)
                    #    h.GetXaxis().SetTitle('#chi^{2}/ndf')
                    else:
                        print "NOT SURE WHO AM I ", h.GetName()
                        #FIXME Alexey
                        if 'pT1000_pt' in name: 
                            h.Rebin(8)
                            h.GetXaxis().SetTitle('p_T [GeV]')
                            h.SetStats(0)
                            h.Fit('gaus', 'q')
                            ltx = ROOT.TLatex(0.62, 0.83, '#sigma = %.7f' % h.GetFunction('gaus').GetParameter(2))
                            ltxe = ROOT.TLatex(0.62, 0.77, '#pm %.7f' % h.GetFunction('gaus').GetParError(2))
                            ltx1 = ROOT.TLatex(0.62, 0.71, '#mu = %.7f' % h.GetFunction('gaus').GetParameter(1))
                            ltx1e = ROOT.TLatex(0.62, 0.65, '#pm %.7f' % h.GetFunction('gaus').GetParError(1))
                        else:
                            h.SetStats(0)
                            h.Fit('gaus', 'q')
                            ltx = ROOT.TLatex(0.62, 0.83, '#sigma = %.7f' % h.GetFunction('gaus').GetParameter(2))
                            ltxe = ROOT.TLatex(0.62, 0.77, '#pm %.7f' % h.GetFunction('gaus').GetParError(2))
                            ltx1 = ROOT.TLatex(0.62, 0.71, '#mu = %.7f' % h.GetFunction('gaus').GetParameter(1))
                            ltx1e = ROOT.TLatex(0.62, 0.65, '#pm %.7f' % h.GetFunction('gaus').GetParError(1))
                  
                else:
                    log_scale = True
                    if 'pull' not in name: 
                        if 'pT1000' in str(sys.argv[1]):
                            print "b4 ", h.Integral()
                            h.GetXaxis().SetRangeUser(-1,1)
                            print "after ", h.Integral()
                        elif 'pT0100' in str(sys.argv[1]):
                            print "b4 ", h.Integral()
                            h.GetXaxis().SetRangeUser(-0.1,0.1)
                            print "after ", h.Integral()
                        elif 'pT0010' in str(sys.argv[1]):
                            h.GetXaxis().SetRangeUser(-0.01,0.01) 
                    h.Draw('')

                if 'ebye' not in name:
                    if 'pull' in name or 'pT1000_pt' in name:
                        ltx.SetNDC()
                        ltx.Draw()
                        ltx1.SetNDC()
                        ltx1.Draw()
                        ltxe.SetNDC()
                        ltxe.Draw() 
                        ltx1e.SetNDC()
                        ltx1e.Draw()
                    elif 'relres' in name:
                        ltx.SetNDC()
                        ltx.Draw()
                        ltx1.SetNDC()
                        ltx1.Draw()
                        ltx1e.SetNDC()
                        ltx1e.Draw()
                     

            self.canvas.SetLogy(log_scale)
            self.canvas.Update()
            self.save_it(name)

        #self.htmlize(self.plots_dir, self.which)

    @classmethod
    def htmlize(cls, plot_dir, which):
        html = ''
        anchors = []

        files = glob.glob(os.path.join(plot_dir, '*.png'))
        comp_files = glob.glob(os.path.join(plot_dir, 'hits_*'))
        comp_files.sort()

        for fn in files:
            fn = os.path.basename(fn)
            if fn.startswith('hits_') or fn.startswith('segs_'):
                continue
            html += '%(fn)s<br>\n<img src="%(fn)s"><br>\n' % locals()

        html += '\n\n'
        
        for fn in comp_files:
            if 'whoknows' in fn or 'overlap' in fn:
                continue
            
            fn = os.path.basename(fn)

            anchor = fn.replace('hits_', '').replace('.png', '')
            anchors.append(anchor)

            segsfn = fn.replace('hits_', 'segs_')
            
            html += '<a name=%(anchor)s>%(anchor)s</a><br>\n<img src="%(fn)s"><img src="%(segsfn)s"><br>\n' % locals()

        #anchors = ''.join(['<a href="#%s">%s</a><br>\n' % (x, cls.make_title(x, which)) for x in anchors]) + '<br><br>'
        anchors = ''
        
        html = '<html><body>' + anchors + html + '</body></html>'
        open(os.path.join(plot_dir, 'index.html'), 'wt').write(html)

import re
def htmlit(plot_dir, pair_ups=[], skips=[]):
    img_re = re.compile('png|gif|jpg|bmp|jpeg')

    for root, dirs, files in os.walk(plot_dir):
        files = [fn for fn in files if img_re.search(fn)]
        files.sort()

        for fn in files[:]:
            for s in skips:
                if s in fn:
                    files.remove(fn)

        for a,b in pair_ups:
            for fn in files[:]:
                if b in fn and [f for f in files if a in f]:
                    files.remove(fn)
        
        html = '<html><body>'
        
        for fn in files:
            anchor = os.path.splitext(fn)[0]
            to_pair = False
            for a,b in pair_ups:
                if a in fn:
                    to_pair = True
                    anchor = anchor.replace(a, '')
            html += '<a name=%(anchor)s>%(anchor)s</a><br>\n<img src="%(fn)s">' % locals()
            if to_pair:
                pfn = fn
                for a,b in pair_ups:
                    pfn = pfn.replace(a,b)
                html += '<img src="%(pfn)s">' % locals()
            html += '<br>\n'

        html += '</body></html>'
        open(os.path.join(root, 'index.html'), 'wt').write(html)

if __name__ == '__main__':
    if os.path.isdir(sys.argv[1]):
        hurr.htmlize(sys.argv[1], sys.argv[2])
    elif os.path.isfile(sys.argv[1]):
        for which in hurr.whiches:
            hurr(sys.argv[1], which).analyze()
    else:
        raise 'haha'

    htmlit('plots'+str(sys.argv[1]).rstrip('.root'), [('hits_', 'segs_')], ['whoknows'])
