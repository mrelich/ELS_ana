#ifndef ReadBack_h
#define ReadBack_h

#include "MyTree.h"
#include "TH1.h"
#include "TProfile.h"
#include "TVector3.h"
#include "TFile.h"
#include "TChain.h"
#include "TMath.h"

#include <stdio.h>
#include <iostream>
#include <vector>

typedef unsigned int uint;

static const int m_nProc = 6;
static const TString m_procNames[m_nProc] = {"misc","eIonization","eBrem","photo","compton","conv"};

class ReadBack
{
  
 public:
  
  // Constructor / Destructor
  ReadBack(TString inputFile, float inputE);
  ~ReadBack();

  void looping(){
    while( getEntry() ) loop();
  }

  // Method to loop over input
  void loop();

  // Load entry
  bool getEntry(){
    if( m_curEvt < m_nEvents ){
      m_ch->GetEntry(m_curEvt);
      m_curEvt++;
      return true;
    }
    return false;
  };

  // Book histograms
  void bookHistos();

  // write file
  void write(){
    if(m_output){
      m_output->Write();
      m_output->Close();
    }
  };

 protected:
  
  TChain* m_ch;           // chain object
  MyEvent* m_event;       // event object
  int m_curEvt;           // current event
  int m_nEvents;          // total number events
  
  float m_radLength;      // specify radiation length

  float m_beamE;          // input electron energy, assumed as shower energy

  TFile* m_output;          // file to save output histograms

  // Specify histograms here
  TProfile* p_dVsAvgE;
  TProfile* p_dVsNPart;
  TProfile* p_dVsQDiff;

  TH1F* h_e_prodE;
  TH1F* h_p_prodE;

  TProfile* p_rVsShowerE; 

  TH1F* h_e_prodE_perProc[m_nProc];
  TH1F* h_p_prodE_perProc[m_nProc];


  TH1F* h_g_prodE;

};

#endif
