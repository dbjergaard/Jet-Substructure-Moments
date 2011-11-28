import sys
sys.argv.append('-b-')
from ROOT import TCanvas, TROOT, TLegend, TSystem, TFile
from array import *


dir     = '' # should be directory in root file where histos are stored

histos = [
['h_clu', 'Number of clusters in a jet', False, -1],
['h_pull_mag', 'Pull Magnitude',False,-1],
['h_theta', 'Pull Angle (#Theta_r)',False,-1],
['h_eta_phi', 'Cluster Density in #eta-#phi',False,-1],
['h_pull_eta_phi', 'Pull Density in #eta-#phi',False,-1],
]

samples = ['Generic Monte Carlo','histos', kOrange -2]

for sample in samples:
    rootfile = 'histos.root' #can be extended for more complicated directory structures 
    sample.append(TFile(rootfile))

for histo in histos :
    title = histo[1]
    c = TCanvas(histo[0],title,600,600)
    leg = TLegend(0.86,0.4,1.0,0.8)
    leg.SetFillColor(0)
    leg.SetBorderSize(0)
    for sample in samples :
        h = sample[len(sample)-1].Get(dir + histo[0])
        # stack signal and background normally, right now we only have total 
        if histo[3] > 0 :
            h.Rebin(histo[3]) 
        h.SetTitle(title);
        h.SetLineColor(sample[4])
        h.SetLineWidth(3)
        hlist.append(h)
        leg.AddEntry(h,sample[0],'f')
        h.Draw()
    c.SetLogy(histo[2])
    leg.Draw()
    c.Print('plots/gifs/' + histo[0] + '.gif')
    c.Print('plots/' + histo[0] + '.eps')
    
