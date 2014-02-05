
#include "ReadBack.h"

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Constructor
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
ReadBack::ReadBack(TString inputFile, float inputE) :
  m_ch(NULL),
  m_event(NULL),
  m_curEvt(0),
  m_nEvents(-1),
  m_radLength(38.25),
  m_beamE(-1),
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

  // Set the beam energy, which is assumed as total
  // shower energy
  m_beamE = inputE;
  

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

  //
  // Needed for plottingthe #RadLengths vs. <e+p>
  //
  int nRads = 40;
  int nParticles[nRads];
  int nChargeDiff[nRads];
  for(int iR=0; iR<nRads; ++iR){
    nParticles[iR] = 0;
    nChargeDiff[iR] = 0;
  }

  //
  // Want to plot shower energy outside the cylinder
  // for all steps in z.  So infitely long in z direction
  // but incrementing steps in x-y (r) plane
  // 
  int nRSteps = 100; // cm
  float showerEOutside[nRSteps];
  bool countedAlready[nRSteps];
  for(int is=0; is<nRSteps; ++is){
    showerEOutside[is] = 0;
    countedAlready[is] = false;
  }

  // Save the initial zposition for comparison
  float initialX = particles.at(0).getX().at(0);
  float initialY = particles.at(0).getY().at(0);
  float initialZ = particles.at(0).getZ().at(0);
  float initialE = particles.at(0).getE().at(0);

  for(uint ip=0; ip<particles.size(); ++ip){    
    Particle part = particles.at(ip);
    fVector xpos   = part.getX();
    fVector ypos   = part.getY();
    fVector zpos   = part.getZ();
    fVector energy = part.getE();


    float minProdE = 0.611; // MeV
    //if( energy.at(0) < minProdE ) continue;

    // Plot z vs. <e> for leading particle
    if(ip == 0){
      for(uint iz=0; iz<zpos.size(); ++iz)
	p_dVsAvgE->Fill(zpos.at(iz) - initialZ, energy.at(iz));
    }
    // Plot e p energy, but exclude initial
    else{
      float ep_energy = part.getE().at(0);
      int procID      = part.getProcID();
      if(ep_energy < minProdE ); // don't count
      else if( part.getPartID() == 11 ){
	h_e_prodE->Fill(ep_energy);
	if( procID >= 0 && procID < m_nProc) h_e_prodE_perProc[procID]->Fill(ep_energy);
      }
      else if( part.getPartID() == -11 ){
	h_p_prodE->Fill(ep_energy);
	if( procID >= 0 && procID < m_nProc) h_p_prodE_perProc[procID]->Fill(ep_energy);
      }

      if( part.getPartID() == 22 ){
	h_g_prodE->Fill(ep_energy);
      }
    }

    // For remaining charge plots, only plot if electron or positron
    if( abs(part.getPartID()) != 11 ) continue; // only look at e/p
    
    // Don't include the initial particle in shower calculations
    if( ip == 0 ) continue;

    // Particle counting
    int prevPos = -999;
    for(uint iz=0; iz<zpos.size(); ++iz){
      if( energy.at(iz) < minProdE ) continue;
      float depth = zpos.at(iz) - initialZ;
      int radPos = (int) depth/(0.5*m_radLength);
      if(prevPos == radPos) continue; // don't save twice
      if(radPos-1 > nRads)  continue; // make sure we are in bounds
      prevPos = radPos;
      nParticles[radPos]  += 1;
      nChargeDiff[radPos] += part.getPartID() > 0 ? 1 : -1;
    }
    
    // Shower Energy outside some radius
    for(int is=0; is<nRSteps; ++is)
      countedAlready[is] = false;

    for(uint ir=0; ir<xpos.size(); ++ir){
      float x = xpos.at(ir) - initialX;
      float y = ypos.at(ir) - initialY;
      float r = TMath::Sqrt(x*x+y*y);
      float e = energy.at(ir);
      
      for(int is = 1; is<=nRSteps; ++is){
	if( r < is )             continue; // inside cone
	if( countedAlready[is] ) continue;
	showerEOutside[is-1] += e/m_beamE;
	//showerEOutside[is-1] += e/initialE;
	countedAlready[is] = true;
      } // end loop over shower steps
      
    }// end loop over x-y pos
    
  }// end loop over particles
  
  // Save plot info for this event
  for(int iR=0; iR<nRads; ++iR){
    if( nParticles[iR] != 0 )
      p_dVsNPart->Fill(iR/2., nParticles[iR]);
    p_dVsQDiff->Fill(iR/2., nChargeDiff[iR]);
  }

  // Save Moliere radius plot
  for(int is=1; is<=nRSteps; ++is){
    p_rVsShowerE->Fill(is, showerEOutside[is-1]);

  }// end loop over imaginary cylinder radius
  
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

  // Electron and positron energy
  h_e_prodE = new TH1F("e_prodE","",100, 0, 100);
  h_p_prodE = new TH1F("p_prodE","",100, 0, 100);

  // Electron and positron energy per production
  // process
  for(int iP=0; iP<m_nProc; ++iP){
    TString proc = m_procNames[iP];
    h_e_prodE_perProc[iP] = new TH1F(("e_prodE_"+proc).Data(),"",100, 0, 100);
    h_p_prodE_perProc[iP] = new TH1F(("p_prodE_"+proc).Data(),"",100, 0, 100);
  }

  // Photon production energy
  h_g_prodE = new TH1F("g_prodE","",100,0,100);

  // Radius vs. average energy outside that radius
  //p_rVsShowerE = new TProfile("rVsShowerE","",100,0,100);
  p_rVsShowerE = new TProfile("rVsShowerE","",1000,0,100);

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
  cout<<"-e <float>"<<endl;
  cout<<"\tInput electron beam energy for this run in MeV"<<endl;
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
  float beamE = -1;        // beam energy of intial electron(s)

  // loop over inputs                                                                                                                        
  for(int i=1; i<argc; ++i){
    if( strcmp(argv[i], "-i") == 0 )
      inputName = TString( argv[++i] );
    else if( strcmp(argv[i], "-e") == 0 )
      beamE = atof( argv[++i] );
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

  // make sure we have some energy specified
  if( beamE < 0 ){
    cout<<"Electron beam energy is negative."<<endl;
    cout<<"-h for help menu"<<endl;
    return 0;
  }

  // Create object
  ReadBack* rback = new ReadBack(inputName, beamE);

  // Loop
  rback->looping();

  // Write file
  rback->write();

  delete rback;

  return 0;
}
