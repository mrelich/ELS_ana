
#include "ReadBack.h"

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Constructor
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
ReadBack::ReadBack()
{
  
  //m_input = new TFile("Test.root");
  m_ch = new TChain("tree");
  m_ch->AddFile("Test.root");
  m_event = new MyEvent();
  m_ch->SetBranchAddress("event",&m_event);
  cout<<"Number of Entries: "<<m_ch->GetEntries()<<endl;
  m_ch->GetEntry(0);
  cout<<"NParticles: "<<m_event->getParticles().size()<<endl;
  
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Destructor
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
ReadBack::~ReadBack()
{


}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Event Loop
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
void ReadBack::loop()
{


}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Main
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
int main()
{

  ReadBack* rback = new ReadBack();


}
