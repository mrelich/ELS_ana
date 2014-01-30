#ifndef ReadBack_h
#define ReadBack_h

#include "MyTree.h"
#include "TH1.h"
#include "TVector3.h"
#include "TFile.h"
#include "TChain.h"

#include <stdio.h>
#include <iostream>
#include <vector>

class ReadBack
{
  
 public:
  
  ReadBack();
  ~ReadBack();

  void loop();

 protected:
  
  TFile* m_input;
  TChain* m_ch;
  MyEvent* m_event;
  

};

#endif
