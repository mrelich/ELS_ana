#ifndef MyTree_h
#define MyTree_h

#include "TObject.h"
#include "TTree.h"
#include "TFile.h"
#include "TString.h"

#include <vector>
#include <stdlib.h>
#include <iostream>

using namespace std;

typedef vector<float> fVector;

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Particle class
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
class Particle : public TObject
{
  
 public:
  
  // Constructor
  Particle(){};
  Particle(int partID, int trkID, int procID, fVector e, fVector x, fVector y, fVector z,
	   fVector dE, fVector dX){
    
    clear();

    m_partID = partID;
    m_trkID  = trkID;
    m_procID = procID;
    m_energy = e;
    m_x = x;
    m_y = y;
    m_z = z;     
    m_dE = dE;
    m_dX = dX;

  };

  // Destructor
  virtual ~Particle(){
    clear();
  };

  // Assignement operator
  //Particle& operator=(const Particle &);

  // Set Variables
  void addEnergyPos(float e, float x, float y, float z){
    m_energy.push_back(e);
    m_x.push_back(x);
    m_y.push_back(y);
    m_z.push_back(z);
  };
  
  // Get Variables
  fVector getE(){ return m_energy; };
  fVector getX(){ return m_x; };
  fVector getY(){ return m_y; };
  fVector getZ(){ return m_z; };
  int getPartID(){ return m_partID; };
  int getTrkID(){  return m_trkID; };
  int getProcID(){ return m_procID; };
  fVector getdE(){ return m_dE; };
  fVector getdX(){ return m_dX; };

  // Clear method
  void clear(){
    m_energy.clear();
    m_x.clear();
    m_y.clear();
    m_z.clear();
    m_dE.clear();
    m_dX.clear();
    m_partID = -1;
    m_trkID  = -1;
    m_procID = -1;
  };

  ClassDef(Particle, 2);
  
 protected:

  // Set Variables
  fVector m_energy;
  fVector m_x;
  fVector m_y;
  fVector m_z;
  fVector m_dE;
  fVector m_dX;

  int m_partID;
  int m_trkID;
  int m_procID;

};


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Event Class
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
class MyEvent : public TObject
{

 public:
  
  // Constructor
  MyEvent(){m_evtNum = -1;};
  MyEvent(int evtNum){
    m_evtNum = evtNum;
  };

  // Destructor
  virtual ~MyEvent(){
    clear();
  };

  // Assignement operator
  //MyEvent& operator=(const MyEvent &);

  // Add particle
  void addParticle(Particle part){
    m_particles.push_back( part );
  };

  // Set event
  void setEvtNum(int evtNum){ m_evtNum = evtNum; };

  // Get evt num and particles
  int getEvtNum(){ return m_evtNum; };
  vector<Particle> getParticles(){ return m_particles; };

  // Clear 
  void clear(){
    m_evtNum=-1;
    m_particles.clear();
  };
  
  ClassDef(MyEvent, 1);

 protected:
  
  int m_evtNum;
  vector<Particle> m_particles;

};

#endif

  
