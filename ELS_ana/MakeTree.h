#ifndef MakeTree_h
#define MakeTree_h

#include "MyTree.h"
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

class MakeTree
{

 public:
  
  MakeTree(){};
  ~MakeTree(){};

  // Create tree
  void CreateTree(TString inName, TString outname);

  // loop over intput file
  void loop(ifstream &input);

  // Set the next event
  void setNextEvent(ifstream &input, int &nEvt);

  void clear(){
    part_e.clear();
    part_x.clear();
    part_y.clear();
    part_z.clear();
    part_ID = -1;
    part_trkID = -1;
    prev_trkID = -1;
  }

  void fill(){ m_tree->Fill(); };
  void finalize(){
    m_outfile->Write();
    m_outfile->Close();
  };

  fVector part_e;
  fVector part_x;
  fVector part_y;
  fVector part_z;
  int part_ID;
  int part_trkID;
  int prev_trkID;

  TFile* m_outfile;
  TTree* m_tree;
  MyEvent* m_event;
  
};

#endif
