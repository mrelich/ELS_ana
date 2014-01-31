#include "MakeTree.h"

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Create the Tree
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
void MakeTree::CreateTree(TString inName, TString outname)
{
  
  // Input file name
  TString inDir = "/home/mrelich/workarea/ara/ELS_mrelich/sh/demo/ELS_output/"; 
  TString infname = inDir + inName;
  ifstream input(infname.Data(), ifstream::in);

  // first two lines dummy
  //char dummy[200];
  //input.getline(dummy,200);
  //input.getline(dummy,200);

  // Create Tree
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
      if( nEvents % 100 == 0 ) 
	cout<<"*** Getting Event: "<<nEvents<<" *** "<<endl;
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
  
  clear();  
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
      nEvt++;
      if( particle ) 
	m_event->addParticle( *particle );
      return;
    }

    // Do this twize
    input.getline(line, 256);
    ss << line;
    strEvent = ss.str();
    found = strEvent.find("End");
    if( found != string::npos ){
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
      part_z.push_back( zpos );
      part_e.push_back( energy );
    }
    else{ // new particle
      particle = new Particle(prev_partID,
			      prev_trkID,
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
      part_z.push_back( zpos );
      part_e.push_back( energy );
    }
			      

  }

  // Add final particle
  if(particle)
    m_event->addParticle( *particle );
  clear();
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Main
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
void help()
{
  cout<<endl;
  cout<<endl;
  cout<<"---------------------------------------------"<<endl;
  cout<<"-i <string> "<<endl;
  cout<<"\tSpecify input file name"<<endl;
  cout<<"-o <string> "<<endl;
  cout<<"\tSpecify output file name"<<endl;
  cout<<"-h"<<endl;
  cout<<"\tThis help menu"<<endl;
  cout<<"---------------------------------------------"<<endl;
  cout<<endl;
  cout<<endl;
    
}


int main(int argc, char** argv)
{

  // Need to pass the input and output file name
  TString inputName  = ""; // .dat
  TString outputName = ""; // .root

  // loop over inputs
  for(int i=1; i<argc; ++i){
    if( strcmp(argv[i], "-i") == 0 )
      inputName = TString( argv[++i] );
    else if( strcmp(argv[i], "-o") == 0 )
      outputName = TString( argv[++i] );
    else{
      help();
      return 0;
    }
  }// end loop over options

  // Have check that input and output aren't empty
  if( inputName.Length() == 0 || outputName.Length() == 0 ){
    cout<<"InputName and OutputName not specified."<<endl;
    cout<<"Not running..."<<endl;
    cout<<"run -h for help menu"<<endl;
    return 0;
  }

  // Add arguments later
  MakeTree* tree = new MakeTree();
 
  tree->CreateTree(inputName, outputName);

  delete tree;

  return 0;

}
