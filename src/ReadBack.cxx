
#include "ReadBack.h"

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Constructor
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
ReadBack::ReadBack(TString inputFile) :
  m_ch(NULL),
  m_event(NULL),
  m_curEvt(0),
  m_nEvents(-1),
  m_radLength(38.25),
  m_output(NULL)
{

  // Create TChain object
  m_ch = new TChain("tree");

  // Add the root file
  //m_ch->AddFile("dump_300_100GeV.root");
  //m_ch->AddFile("dump_100_1TeV.root");
  //m_ch->AddFile("dump_300_100GeV_CutOff611KeV.root");
  m_ch->AddFile((inputFile+".root").Data());

  // Create the event object and assign
  m_event = new MyEvent();
  m_ch->SetBranchAddress("event",&m_event);

  // Set current event and number of events
  m_curEvt  = 0;
  m_nEvents = m_ch->GetEntries();

  // Name the output file
  //m_output = new TFile("loop1.root","recreate");
  //m_output = new TFile("loop0.root","recreate");
  //m_output = new TFile("loop2.root","recreate");
  m_output = new TFile((inputFile+"_hist.root").Data(),"recreate");

  // Book histograms
  bookHistos();

}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Destructor
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
ReadBack::~ReadBack()
{

  delete m_event;
  delete m_ch;

}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Event Loop
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
void ReadBack::loop()
{
  // Load the vector of particles
  vector<Particle> particles = m_event->getParticles();

  // Needed for plottingthe #RadLengths vs. <e+p>
  int nRads = 40;
  int nParticles[nRads];
  int nChargeDiff[nRads];
  for(int iR=0; iR<nRads; ++iR){
    nParticles[iR] = 0;
    nChargeDiff[iR] = 0;
  }

  // Save the initial zposition for comparison
  float initialZ = particles.at(0).getZ().at(0);

  for(uint ip=0; ip<particles.size(); ++ip){    
    Particle part = particles.at(ip);
    fVector zpos   = part.getZ();
    fVector energy = part.getE();

    // Plot z vs. <e> for leading particle
    if(ip == 0)
      for(uint iz=0; iz<zpos.size(); ++iz)
	p_dVsAvgE->Fill(zpos.at(iz) - initialZ, energy.at(iz));
			
			
    // For remaining charge plots, only plot if electron or positron
    if( abs(part.getPartID()) != 11 ) continue; // only look at e/p
    
    int prevPos = -999;
    for(uint iz=0; iz<zpos.size(); ++iz){
      if( energy.at(iz) < 0.611 ) continue;
      float depth = zpos.at(iz) - initialZ;
      int radPos = (int) depth/(0.5*m_radLength);
      if(prevPos == radPos) continue; // don't save twize
      if(radPos-1 > nRads)  continue; // make sure we are in bounds
      prevPos = radPos;
      nParticles[radPos]  += 1;
      nChargeDiff[radPos] += part.getPartID() > 0 ? 1 : -1;
    }
    
    
  }// end loop over particles
  
  // Save plot info for this event
  for(int iR=0; iR<nRads; ++iR){
    if( nParticles[iR] != 0 )
      p_dVsNPart->Fill(iR/2., nParticles[iR]);
    p_dVsQDiff->Fill(iR/2., nChargeDiff[iR]);
  }
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Book histograms
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
void ReadBack::bookHistos()
{
  // outputfile
  m_output->cd();

  // Distance in ice vs. Avg E.
  p_dVsAvgE = new TProfile("dVsAvgE","",200,0,200); // in cm.

  // Look at <e+p> vs. rad length
  p_dVsNPart = new TProfile("dVsNPart","",40,0,20);

  // Look at <e-p> vs. rad length
  p_dVsQDiff = new TProfile("dVsQDiff","",40,0,20);

}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Main
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
void help()
{
  
  cout<<endl;
  cout<<endl;
  cout<<"---------------------------------------------"<<endl;
  cout<<"-i <string> "<<endl;
  cout<<"\tSpecify input file name. Will also be output name w/ _hist.root"<<endl;
  cout<<"\tDo not need to add .root extension to input file name."<<endl;
  cout<<"-h"<<endl;
  cout<<"\tThis help menu"<<endl;
  cout<<"---------------------------------------------"<<endl;
  cout<<endl;
  cout<<endl;
  
}

int main(int argc, char** argv)
{

  // Need to pass the input and output file name                                                                                             
  TString inputName  = ""; // no .root needed

  // loop over inputs                                                                                                                        
  for(int i=1; i<argc; ++i){
    if( strcmp(argv[i], "-i") == 0 )
      inputName = TString( argv[++i] );
    else{
      help();
      return 0;
    }
  }// end loop over inputs

  // Check if inputname exists
  if( inputName.Length() == 0 ){
    cout<<"No input file specified"<<endl;
    cout<<"-h for help menu"<<endl;
    return 0;
  }

  // Create object
  ReadBack* rback = new ReadBack(inputName);

  // Loop
  rback->looping();

  // Write file
  rback->write();

  delete rback;

  return 0;
}
