#include "MyTree.h"

MyEvent& MyEvent::operator=(const MyEvent &rhs)
{
  if( this != &rhs ){
    m_evtNum = rhs.m_evtNum;
    m_particles = rhs.m_particles;
  }
  return *this;
}

Particle& Particle::operator=(const Particle &rhs){
  
  if( this != &rhs ){
    m_energy = rhs.m_energy;
    m_x = rhs.m_x;
    m_y = rhs.m_y;
    m_z = rhs.m_z;
    m_partID = rhs.m_partID;
  }
  
  return *this;
}
