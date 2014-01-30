
#include "TH1.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TString.h"
#include "TFile.h"

#include <vector>

// Specify some constants here
TString directory = "/home/mrelich/workarea/ara/ELS_ana/python/";

// Number of files
const int nFiles = 1;

// Files
TFile* m_files[nFiles];

// Titles
TString m_labels[nFiles] = {"E_{e-} = 1 TeV"};
  //"E_{e-} = 1 GeV",
  //"E_{e-} = 40 MeV",};

// Colors
int m_colors[nFiles] = {kBlack}; //, kBlue};

// Markers
int m_markers[nFiles] = {20}; //, 25};

//------------------------------------------------------------//
// Main
//------------------------------------------------------------//
void formatPlots()
{
  
  // Specify files
  //m_files[0] = new TFile((directory + "output_1GeV.root").Data());
  //m_files[1] = new TFile((directory + "output_40MeV.root").Data());
  m_files[0] = new TFile((directory + "output_1TeV.root").Data());

  radiationLength();
  
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
  
  // Get TLegend
  TLegend* legend = makeLegend(0.65, 0.8, 0.8, 0.9);

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
    pair<float,float> radAndErr = getRadLength(profs[f], fits[f]);
    rads[f] = radAndErr.first;
    radsErr[f] = radAndErr.second;
  }
  

  
  // Set maximum so both fit
  profs[0]->SetMaximum(1.2 * maximum);

  // Loop and plot
  profs[0]->Draw();
  fits[0]->Draw("same");
  for(int f=1; f<nFiles; ++f)
    profs[f]->Draw("same");
  
  // Draw Legend
  legend->Draw("same");

  // Add latex labels
  TLatex* lat = makeLatex();
  for(int f=0; f<nFiles; ++f)
    lat->DrawLatex(0.65,0.6-0.1*f,
		   Form("#color[%i]{#chi_{0} = %4.2f +/- %4.2f}", m_colors[f], rads[f], radsErr[f]));

  c->SaveAs("/home/mrelich/workarea/ara/ELS_ana/plots/radiationLength.eps");

}

//------------------------------------------------------------//
// Fit for radiation length
//------------------------------------------------------------//
pair<float,float> getRadLength(TProfile* prof, TF1* &fit)
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
  fit->SetLineColor(kBlue);

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
  
  cout<<"name: "<<pname<<endl;
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

