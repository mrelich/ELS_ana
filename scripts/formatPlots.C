
#include "TH1.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TString.h"
#include "TFile.h"

#include <vector>

// Specify some constants here
//TString directory = "/home/mrelich/workarea/ara/ELS_ana/python/";
TString directory = "/home/mrelich/workarea/ara/ELS_ana/run/";

// Number of files
const int nFiles = 4;

// Files
TFile* m_files[nFiles];

// Titles
TString m_labels[nFiles];// = {"E_{e-} = 100 GeV (100MeV CO)",
			 //   "E_{e-} = 100 GeV (611keV CO)"};
  //"E_{e-} = 1 GeV",
  //"E_{e-} = 40 MeV",};

// Colors
int m_colors[] = {kBlack, kBlue, kRed, kMagenta};

// Markers
int m_markers[] = {20, 25, 22, 21};

//TString prodE = "_100MeV";
TString prodE = "_611KeV";

//------------------------------------------------------------//
// Main
//------------------------------------------------------------//
void formatPlots()
{
  
  // Specify files
  m_files[0]  = new TFile((directory + "output_1GeV_hist.root").Data());
  m_labels[0] = "E_{beam} = 1 GeV"; 
  m_files[1]  = new TFile((directory + "output_10GeV_hist.root").Data());
  m_labels[1] = "E_{beam} = 10 GeV"; 
  m_files[2]  = new TFile((directory + "output_100GeV_hist.root").Data());
  m_labels[2] = "E_{beam} = 100 GeV";
  m_files[3]  = new TFile((directory + "lead_1GeV_hist.root").Data());
  m_labels[3] = "E_{beam} = 1 GeV";  


  // Plot the radiation length for ice
  //radiationLength();
  
  // Plot number of particles for shower
  //numberParticles(m_files[1]);

  // Plot the energy of produced particles
  //plotProdE(m_files[1]);

  // Plot production energy per process producing them
  //plotProdEPerProc(m_files[1]);

  // Plot the relative energy vs. shower radius
  plotMoliereRadius();

}

//------------------------------------------------------------//
// Method to plot and fit for radiation lengths
//------------------------------------------------------------//
void radiationLength()
{
  
  // Plot to grab
  TString plot = "dVsAvgE";
  
  // Get canvas
  TCanvas* c = makeCanvas("c");
  //c->SetLogy();

  // Get TLegend
  TLegend* legend = makeLegend(0.55, 0.8, 0.7, 0.9);
  
  // Holder for maximum
  float maximum = -999;
  
  // Radiation lengths
  float rads[nFiles];
  float radsErr[nFiles];
  
  // Load profiles
  TProfile* profs[nFiles];
  TF1* fits[nFiles];
  for(int f=0; f<nFiles; ++f){
    fits[f] = NULL;
    profs[f] = getProfile(m_files[f], plot, "Distance [cm]",
			  "Average Energy [MeV]", m_colors[f], m_markers[f]);
    legend->AddEntry(profs[f], m_labels[f].Data(), "lep");
    if( maximum < profs[f]->GetMaximum() )
      maximum = profs[f]->GetMaximum();
    pair<float,float> radAndErr = getRadLength(profs[f], fits[f], m_colors[f]);
    rads[f] = radAndErr.first;
    radsErr[f] = radAndErr.second;
    legend->AddEntry(fits[f], Form("#chi_{0} = %4.2f #pm %4.2f", rads[f], radsErr[f]), "l");
  }
  
  
  
  // Set maximum so both fit
  profs[0]->SetMaximum(1.2 * maximum);
  
  // Loop and plot
  profs[0]->Draw();
  fits[0]->Draw("same");
  for(int f=1; f<nFiles; ++f){
    profs[f]->Draw("same");
    fits[1]->Draw("same");
  }

  // Draw Legend
  legend->Draw("same");

  // Add latex labels
  //TLatex* lat = makeLatex();
  //for(int f=0; f<nFiles; ++f)
  ///lat->DrawLatex(0.65,0.6-0.1*f,
  //Form("#color[%i]{#chi_{0} = %4.2f +/- %4.2f}", m_colors[f], rads[f], radsErr[f]));

  //c->SaveAs("/home/mrelich/workarea/ara/ELS_ana/plots/radiationLength.eps");
  //c->SaveAs("/home/mrelich/workarea/ara/ELS_ana/plots/radiationLength_log.eps");
  c->SaveAs("/home/mrelich/workarea/ara/ELS_ana/plots/radiationLength_100GeVComp.eps");

}

//------------------------------------------------------------//
// Plot number of particles
//------------------------------------------------------------//
void numberParticles(TFile* file)
{

  // Two histograms that we want
  vector<TString> pnames;
  pnames.push_back("dVsNPart");
  pnames.push_back("dVsQDiff");
  
  // Labels
  vector<TString> lbls;
  lbls.push_back("e+p");
  lbls.push_back("e-p");
  
  TProfile* profs[2];
  
  // Make canvas
  TCanvas* c = makeCanvas("c");

  // Make legend
  TLegend* legend = makeLegend(0.7, 0.9, 0.8, 0.9);

  int colors[] = {kBlack, kBlue};
  int markers[] = {20, 25};
  
  // Loop and load plots
  float maximum = -999;
  for(unsigned int i=0; i<pnames.size(); ++i){
    TString pname = pnames.at(i);
    profs[i] = getProfile(file, pname, "Distance (#chi_{0})", "Number of Particles", colors[i], markers[i]);
    legend->AddEntry(profs[i], lbls.at(i).Data(), "lep");
    if( maximum < profs[i]->GetMaximum() )
      maximum = profs[i]->GetMaximum();
  }

  profs[0]->SetMaximum(1.1*maximum);
  profs[0]->Draw();
  for(unsigned int i =0; i<pnames.size(); ++i)
    profs[i]->Draw("same");
  legend->Draw("same");

  
  //c->SaveAs("/home/mrelich/workarea/ara/ELS_ana/plots/chargeDiff.eps");
  c->SaveAs(("/home/mrelich/workarea/ara/ELS_ana/plots/chargeDiff"+prodE+".eps").Data());

  
}

//------------------------------------------------------------//
// Plot electron and positron production energy
//------------------------------------------------------------//
void plotProdE(TFile* file)
{

  // Two histograms that we want
  vector<TString> pnames;
  pnames.push_back("e_prodE");
  pnames.push_back("p_prodE");
  
  // Labels
  vector<TString> lbls;
  lbls.push_back("Electron");
  lbls.push_back("Positron");
  
  TH1F* hists[2];
  
  // Make canvas
  TCanvas* c = makeCanvas("c");
  c->SetLogy();
  // Make legend
  TLegend* legend = makeLegend(0.6, 0.9, 0.8, 0.9);

  int colors[] = {kBlack, kBlue};
  int markers[] = {20, 25};
  
  // Loop and load plots
  float maximum = -999;
  for(unsigned int i=0; i<pnames.size(); ++i){
    TString pname = pnames.at(i);
    hists[i] = getHist(file, pname, "Production Energy [MeV]", "Entries", colors[i], markers[i]);
    legend->AddEntry(hists[i], lbls.at(i).Data(), "lep");
    //hists[i]->Scale(1/hists[i]->Integral());
    if( maximum < hists[i]->GetMaximum() )
      maximum = hists[i]->GetMaximum();

  }

  hists[0]->SetMaximum(1.1*maximum);
  hists[0]->Draw();
  for(unsigned int i =0; i<pnames.size(); ++i)
    hists[i]->Draw("same");
  legend->Draw("same");

  
  c->SaveAs(("/home/mrelich/workarea/ara/ELS_ana/plots/EnergyDistribution"+prodE+".eps").Data());
  
  

}

//------------------------------------------------------------//
// Plot electron and positron production energy
//------------------------------------------------------------//
void plotProdEPerProc(TFile* file)
{

  // particles to plot
  vector<TString> pnames;
  pnames.push_back("e_prodE_eIonization");
  pnames.push_back("e_prodE_misc");
  pnames.push_back("e_prodE_photo");
  pnames.push_back("e_prodE_compton");
  pnames.push_back("e_prodE_conv");
  //pnames.push_back("p_prodE_eIonization");
  //pnames.push_back("p_prodE_eBrem");
  
  // Marker attributes
  int colors[] = {kBlack, kBlue, kRed, kGreen-3, kMagenta};
  int markers[] = {20, 22, 23, 24, 25};
  
  // 5 processes to consider
  TH1F* hists[5];
  
  // Make canvas
  TCanvas* c = makeCanvas("c");
  c->SetLogy();
  
  // Make legend
  TLegend* legend = makeLegend(0.6, 0.9, 0.6, 0.9);
  
  float maximum = -999;
    
  for(unsigned int j=0; j<pnames.size(); ++j){
    TString pname = pnames.at(j);
    hists[j] = getHist(file, pname, "Production Energy [MeV]", "Entries", colors[j], markers[j]);
    
    legend->AddEntry(hists[j], pname.Data(), "lep");
    
    if( maximum < hists[j]->GetMaximum() )
      maximum = hists[j]->GetMaximum();
      
    
  }// end loop over processes
  
  //hists[0]->SetMaximum(1.1*maximum);
  hists[0]->SetMaximum(10*maximum);
  hists[0]->Draw();
  for(unsigned int i=0; i<pnames.size(); ++i)
    hists[i]->Draw("same");
  legend->Draw("same");
  
  
  //c->SaveAs(("/home/mrelich/workarea/ara/ELS_ana/plots/EnergyDistribution"+prodE+".eps").Data());
  //c->SaveAs("/home/mrelich/workarea/ara/ELS_ana/plots/p_prodE_perProc.png");
  c->SaveAs("/home/mrelich/workarea/ara/ELS_ana/plots/e_prodE_perProc.png");
  
}

//------------------------------------------------------------//
// Fit for radiation length
//------------------------------------------------------------//
pair<float,float> getRadLength(TProfile* prof, TF1* &fit, int color)
{
  
  // Must pass the distance travelled vs the average energy.
  // Fit for the radiation length following:
  // <E> = E0 e^{-x/chi_0)

  //TF1 func = TF1("radLength","[0]*TMath::Exp(-x/[1]) + [2]",0,200);
  TF1 func = TF1("radLength","[0]*TMath::Exp(-x/[1])",0,200);
  func.SetParameter(0,0.5);
  func.SetParameter(1,4.0);

  
  // Fit
  prof->Fit("radLength","RQ");

  fit = prof->GetFunction("radLength");
  fit->SetLineColor(color);

  // Print info
  float E0 = fit->GetParameter(0);
  float radLength = fit->GetParameter(1);
  float radErr = fit->GetParError(1);

  cout<<"E0: "<<E0<<" chi: "<<radLength<<" +/- "<<radErr<<endl;

  //delete fit;

  pair<float,float> radPair (radLength, radErr);

  return radPair;

}

//------------------------------------------------------------//
// Make TLatex
//------------------------------------------------------------//
TLatex* makeLatex()
{

  TLatex* lat = new TLatex();
  lat->SetTextSize(0.04);
  lat->SetNDC();
  lat->SetTextFont(42);
  lat->SetTextColor(kBlack);
  return lat;
}

//------------------------------------------------------------//
// Plot to fine Moliere Radius
//------------------------------------------------------------//
void plotMoliereRadius()
{

  // The Moliere radius can be estimated by looking at the
  // ratio of the shower energy outside some radius R 
  // of a hypothetical cylinder centered on the shower max.
  // In practice, we look at the average showere energy
  // outside this radius in steps.  Then look when U/E(0) = 0.1
  // That is Moliere radius.

  // The plot name
  TString pname = "rVsShowerE";

  // Constant line
  TF1* f1 = new TF1("f1","0.1",0,100);
  f1->SetLineColor(kBlack);
  f1->SetLineStyle(2);

  // Make canvas
  TCanvas* c = makeCanvas("c");
  c->SetLogx();
  c->SetLogy();

  // Make Legend
  TLegend* leg = makeLegend(0.6, 0.8, 0.7, 0.9);

  // Loop and get profiles
  TProfile* profs[nFiles];
  for(int f=0; f<nFiles; ++f){
    profs[f] = getProfile(m_files[f], pname, "Radius [cm]", "U/E_{0}",
			  m_colors[f], m_markers[f]);
    leg->AddEntry(profs[f], m_labels[f].Data(),"l");
    
    // Just have some uniform min and max
    profs[f]->SetMinimum(1e-2);
    profs[f]->SetMaximum(1);
    int xmin = profs[f]->FindBin(1);
    int xmax = profs[f]->FindBin(50);
    profs[f]->GetXaxis()->SetRange(xmin,xmax);

    // draw
    if( f==0 ) profs[f]->Draw();
    else       profs[f]->Draw("same");
  }

  // draw legend
  leg->Draw("same");

  // Draw function
  f1->Draw("same");

  // Save
  c->SaveAs("/home/mrelich/workarea/ara/ELS_ana/plots/MoliereRadius.png");

}

//------------------------------------------------------------//
// Make legend
//------------------------------------------------------------//
TLegend* makeLegend(float x0, float x1, float y0, float y1)
{

  TLegend* leg = new TLegend(x0, y0, x1, y1);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetLineColor(0);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.04);
  return leg;

}

//------------------------------------------------------------//
// Make canvas
//------------------------------------------------------------//
TCanvas* makeCanvas(TString name)
{

  TCanvas* c = new TCanvas(name.Data(), name.Data(), 700, 500);
  c->SetTicks(1,1);
  c->SetTopMargin(0.05);
  c->SetRightMargin(0.05);
  c->SetBottomMargin(0.12);
  c->SetLeftMargin(0.12);
  
  return c;
}

//------------------------------------------------------------//
// Get TProfile
//------------------------------------------------------------//
TProfile* getProfile(TFile* file, TString pname, TString xtitle,
		     TString ytitle, int color, int marker)
{
  
  TProfile* prof = (TProfile*) (file->Get(pname.Data())->Clone(Form("%s_%i",pname.Data(),color)));
  prof->GetXaxis()->SetTitle(xtitle.Data());
  prof->GetYaxis()->SetTitle(ytitle.Data());
  prof->SetMarkerStyle(marker);
  prof->SetMarkerColor(color);
  prof->SetMarkerSize(0.5);
  prof->SetLineColor(color);
  prof->SetTitle("");
  prof->SetStats(0);
  prof->GetYaxis()->SetTitleOffset(1.5);
  
  return prof;

}

//------------------------------------------------------------//
// Get histogram
//------------------------------------------------------------//
TH1F* getHist(TFile* file, TString pname, TString xtitle,
		  TString ytitle, int color, int marker)
{
  
  TH1F* hist = (TH1F*) (file->Get(pname.Data())->Clone(Form("%s_%i",pname.Data(),color)));
  hist->GetXaxis()->SetTitle(xtitle.Data());
  hist->GetYaxis()->SetTitle(ytitle.Data());
  hist->SetMarkerStyle(marker);
  hist->SetMarkerColor(color);
  hist->SetMarkerSize(0.5);
  hist->SetLineColor(color);
  hist->SetTitle("");
  hist->SetStats(0);
  hist->GetYaxis()->SetTitleOffset(1.5);
  hist->SetLineWidth(2);
  return hist;

}

