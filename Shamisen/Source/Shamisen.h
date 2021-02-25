/*
  ==============================================================================

    Shamisen.h
    Created: 23 Feb 2021 2:11:42pm
    Author:  tlasi

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "ShamisenString.h"
#include "ShamisenBridge.h"
#include "ShamisenMembrane.h"

class Shamisen : public Component
{
public:
    Shamisen(NamedValueSet& parameters, double k);
    ~Shamisen();

    void paint(Graphics&) override;
    void resized() override;

    
    void calculateUpdateEqs();
    void solveSystem();
    void updateStates();

    double getOutput(double ratioB, double ratioS1) { 
        return myShamisenBridge->getOutput(ratioB)+ myShamisenString1->getOutput(ratioS1)+ myShamisenMembrane->getOutput();
    };
    

    std::shared_ptr<ShamisenString> getString() { return myShamisenString1; };
    std::shared_ptr<ShamisenBridge> getBridge() { return myShamisenBridge; };
    std::shared_ptr<ShamisenMembrane> getBody() { return myShamisenMembrane; };

private:
    double k;
    // Instrument components (String, body and bridge)
    std::shared_ptr<ShamisenString> myShamisenString1;
    std::shared_ptr<ShamisenBridge> myShamisenBridge;
    std::shared_ptr<ShamisenMembrane> myShamisenMembrane;

    // String variables needed for calculating connections
    double rhoS, ES, sigma0S, sigma1S;
    double HS1, AS1, TS1, hS1;
    double HS2, AS2, TS2, hS2;
    double HS3, AS3, TS3, hS3;

    // Bridge variables needed for calculating connections
    double rhoB, HB, AB, EB, sigma0B, sigma1B, hB;

    // Membrane variables needed for calculating collision
    double rhoM, HM, D, TM, EM, sigma0M, sigma1M, hM;

    // Force variables
    double Fs1b, Fs2b, Fs3b, Fbml, Fbmr;

    // Length of the part
    int NS1, NS2, NS3, NB, NMx, NMy;    
    
    // Connection Points
    int lS1c, lS2c, lS3c, lBc1, lBc2, lBc3, lBcl, lBcr, lMcl, lMcr, mMcl, mMcr;

    // Connection point multipliers
    double Fs1bMult, Fs2bMult, Fs3bMult, FbmMult;

    // An (N+1) * 3 vector containing the state of the system at all time-steps
    std::vector<std::vector<double>> u1States;
    std::vector<double*> u1; // vector of pointers to state vectors
    std::vector<std::vector<double>> u2States;
    std::vector<double*> u2; // vector of pointers to state vectors
    std::vector<std::vector<double>> u3States;
    std::vector<double*> u3; // vector of pointers to state vectors

    std::vector<std::vector<double>> vStates;
    std::vector<double*> v; // vector of pointers to state vectors

    std::vector<std::vector<std::vector<double>>> wStates;
    std::vector<std::vector<double*>> w; // vector of pointers to state vectors
};