#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#
# Attempting to make first set of plots from raw data #
# for geant4.  This will be the shower profile which  #
# will contain the shape and then later energy info.  #
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#

import os
import ROOT
from ROOT import *
import array

##################################
# Specify some generic information
# associated with this entry
##################################

# File location
directory = "/home/mrelich/workarea/ara/ELS_mrelich/sh/demo/ELS_output/"

# beam energy
#beamE = "40MeV"
#beamE = "30MeV"
#beamE = "1GeV"
beamE = "1TeV"

# base filename
#fnameBase = "dump_1k_" # 1k events simulated
fnameBase = "dump_100_" # 100 events simulated

# output root tree name
output = TFile("output_" + beamE + ".root","recreate") 

# Energy Threshold
minKinE    = 0.0015 # 1.5 MeV in GeV
RadLength  = 42.0 # cm


##################################
# Specify the histograms to fill
##################################

dVsAvgE  = TProfile("dVsAvgE", "Average Energy vs. Depth", 200, 0, 200);
dVsNPart = TProfile("dVsNPart", "Number of particles", 40, 0, 20)
dVsQDiff = TProfile("dVsQDiff", "N(e-p)", 40, 0, 20)

##################################
# Particle variables loaded per
# event.  Right now just store
# them as simple data objects.
# Maybe make classes later...
##################################

x_pos      = []    #cm
y_pos      = []    #cm
z_pos      = []    #cm
steplength = []    #cm
energy     = []    #GeV
energyDep  = []    #GeV
pdg        = []    #Particle ID Number following PDG
trkID      = []    #Track ID. Important one is initial particle trkID == 1
parentID   = []    #Parent track ID

##################################
# Open file and process the events
##################################

input = open(directory + fnameBase + beamE + ".dat", "r")

def loadEvent():
    event_found = False
    for line in input:

        # reached end of event
        if "End" in line and event_found: 
            return True

        # Start the event
        if "Event" in line: 
            event_found = True
            continue

        # Haven't found line
        if "Event" not in line and not event_found:
            continue
        broken = line.split("\n")
        broken = (broken[0]).split()
        x_pos.append(float(broken[0]))
        y_pos.append(float(broken[1]))
        z_pos.append(float(broken[2]))
        steplength.append(float(broken[3]))
        energy.append(float(broken[4]))
        energyDep.append(float(broken[5]))
        pdg.append(int(broken[6]))
        trkID.append(int(broken[7]))
        parentID.append(int(broken[8]))

    if event_found: return True
    return False

def clear():
    del x_pos[:]
    del y_pos[:]
    del z_pos[:]
    del steplength[:]
    del energy[:]
    del energyDep[:]
    del pdg[:]
    del trkID[:]
    del parentID[:]


##################################
# Methods to keep track of particles
##################################
def getCharge(alreadyCounted, trkId, pdgId):
    
    if abs(pdgId) != 11: return 0 # not electron
    
    for i in range(len(alreadyCounted)):
        if trkId == alreadyCounted[i]:
            return 0 # already included
    
    alreadyCounted.append(trkId)
    if pdg > 0: return -1
    return 1


##################################
# Event Loop
##################################
nEvent = 0
while( loadEvent() ):

    if nEvent % 100 == 0 :
        print "**** Events Processed: ", nEvent, " ****"

    if len(z_pos) == 0: continue

    # Remember origin position to measure
    # distance travelled
    initPos = z_pos[0];

    # Count electrons
    partPerStep    = array.array('i',(0,)*40)
    qDiffPerStep   = array.array('i',(0,)*40)
    previousTrkIds = []

    # Loop over entries in vector
    for i in range( len(z_pos) ):

        # Fill avg distance traveled by initial electron
        if trkID[i] == 1 and parentID[i] == 0:
            dVsAvgE.Fill(z_pos[i]-initPos, energy[i])

        # Deal with particle count
        q = getCharge(previousTrkIds, trkID[i], pdg[i])
        if q != 0 and energy[i] > minKinE:
            pos = int((z_pos[i] - initPos)/(0.5*RadLength))
            partPerStep[pos] += 1
            qDiffPerStep[pos] += q
    
    # After done looping, fill in some histograms
    for i in range(len(partPerStep)):
        dVsNPart.Fill(i/2., partPerStep[i])
        dVsQDiff.Fill(i/2., abs(qDiffPerStep[i])) # take abs to get elec-positron

    del previousTrkIds[:]
    

    # Clear all vectors
    clear()
    nEvent+= 1


output.Write()
output.Close()
