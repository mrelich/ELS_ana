#ifndef ReadBack_h
#define ReadBack_h

#include "MyTree.h"
#include "TH1.h"
#include "TProfile.h"
#include "TVector3.h"
#include "TFile.h"
#include "TChain.h"

#include <stdio.h>
#include <iostream>
#include <vector>

typedef unsigned int uint;

class ReadBack
{
  
 public:
  
  // Constructor / Destructor
  ReadBack(TString inputFile);
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

  TFile* m_output;          // file to save output histograms

  // Specify histograms here
  TProfile* p_dVsAvgE;
  TProfile* p_dVsNPart;
  TProfile* p_dVsQDiff;

};

#endif
