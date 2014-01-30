#include "MakeTree.h"

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Create the Tree
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
void MakeTree::CreateTree()
{

  // Input file name
  //TString infname = "/home/mrelich/workarea/ara/ELS-geant4/elsbeam-geant4-ver140116/sh/demo/energydeposit.dat";
  //TString infname = "/home/mrelich/workarea/ara/ELS_mrelich/sh/demo/dump_1k_40MeV.dat";
  TString infname = "/home/mrelich/workarea/ara/ELS_mrelich/sh/demo/ELS_output/dump_100_1TeV.dat";
  //TString infname = "/home/mrelich/workarea/ara/ELS_mrelich/sh/demo/temp.dat";
  ifstream input(infname.Data(), ifstream::in);

  // first two lines dummy
  //char dummy[200];
  //input.getline(dummy,200);
  //input.getline(dummy,200);

  // Create Tree
  TString outname = "Test.root";
  m_outfile = new TFile(outname.Data(), "recreate");
  m_tree = new TTree("tree","tree");
  m_tree->SetAutoSave(1000000);
  m_tree->SetMaxTreeSize(3000000000u);
  
  m_event = new MyEvent();
  m_tree->Branch("event",&m_event,32000,1);
  
  

  loop(input);

}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Setup the event loop
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
void MakeTree::loop(ifstream & input)
{

  int nEvents = 0;

  // Need to loop over input file
  while( input.good() ){

    m_event->clear();

    // Get a line
    char line[256];
    input.getline(line, 256);
    stringstream ss;
    ss << line;
    string strEvent = ss.str();
    size_t found = strEvent.find("Event");
    if( found != string::npos ){
      cout<<"Getting event: "<<nEvents<<endl;
      setNextEvent(input, nEvents);
    }

    // If here we are now ready to fill
    fill();

  }
      
  // Save the tree
  finalize();
  
  
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Get next event
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
void MakeTree::setNextEvent(ifstream &input, int &nEvt)
{


  // Input variables
  float xpos      = 0;
  float ypos      = 0;
  float zpos      = 0;
  float stepSize  = 0;
  float energy    = 0;
  float energyDep = 0;
  int partID      = 0;
  int trkID       = 0;
  int motherID    = 0;
  
  
  m_event->clear();
  m_event->setEvtNum(nEvt);

  Particle* particle = NULL;
  int prev_trkID     = -1;
  int prev_partID    = -1;

  while( input.good() ){
        
    // Make sure we are not at the end of the event
    
    streampos initial = input.tellg();
    char line[256];
    input.getline(line, 256);
    stringstream ss;
    ss << line;
    string strEvent = ss.str();
    size_t found = strEvent.find("End");
    if( found != string::npos ){
      cout<<"Ending Event: "<<nEvt<<endl;
      nEvt++;
      if( particle ) 
	m_event->addParticle( *particle );
      return;
    }

    // Do this twize
    input.getline(line, 256);
    ss;
    ss << line;
    strEvent = ss.str();
    found = strEvent.find("End");
    if( found != string::npos ){
      cout<<"Ending Event: "<<nEvt<<endl;
      nEvt++;
      if( particle ) 
	m_event->addParticle( *particle );
      return;
    }

    // Not at the end so load more stuff
    input.seekg(initial);
    input >> xpos >> ypos >> zpos 
	  >> stepSize >> energy >> energyDep
	  >> partID >> trkID >> motherID;

    if(prev_trkID < 0){
      prev_trkID  = trkID;
      prev_partID = partID;
    }

    // Still on same particle
    if( prev_trkID == trkID ){
      part_x.push_back( xpos );
      part_y.push_back( ypos );
      part_z.push_back( ypos );
      part_e.push_back( energy );
    }
    else{ // new particle
      particle = new Particle(prev_partID,
			      part_e,
			      part_x,
			      part_y,
			      part_z);
      m_event->addParticle( *particle );
      clear();
      prev_trkID = trkID;
      prev_partID = partID;

      // Remember to now add to this batches coords
      part_x.push_back( xpos );
      part_y.push_back( ypos );
      part_z.push_back( ypos );
      part_e.push_back( energy );
    }
			      

  }

  cout<<"Error: End not found..."<<endl;
  return;

  //MyEvent* newEvent = new MyEvent(nEvt);

  
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Main
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
int main()
{

  // Add arguments later
  MakeTree* tree = new MakeTree();

  tree->CreateTree();

  delete tree;

  return 0;

}
