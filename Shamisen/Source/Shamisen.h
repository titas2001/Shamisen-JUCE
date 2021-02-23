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

    void solveSystem();
    void calculateUpdateEqs();
    void updateStates();

    double getOutput(double ratioB) { return myShamisenBridge->getOutput(ratioB); };
    double getOutput(double ratioS1) { return myShamisenString1->getOutput(ratioS1); };
    double getOutput() { return myShamisenMembrane->getOutput(); };

    std::shared_ptr<ShamisenString> getString() { return myShamisenString1; };
    std::shared_ptr<ShamisenBridge> getBridge() { return myShamisenBridge; };
    std::shared_ptr<ShamisenMembrane> getBody() { return myShamisenMembrane; };


    // tromba marina had this: 
    // void setCurSample (unsigned long curSamp) { curSample = curSamp; };

    void reset();
private:
    double k;
    // Instrument components (String, body and bridge)
    std::shared_ptr<ShamisenString> myShamisenString1;
    std::shared_ptr<ShamisenBridge> myShamisenBridge;
    std::shared_ptr<ShamisenMembrane> myShamisenMembrane;

    // String variables needed for calculating connections
    double rhoS1, HS1, AS1, TS1, ES1, sigma0S1, sigma1S1, hS1;

    // Bridge variables needed for calculating connections
    double rhoB, HB, AB, EB, sigma0B, sigma1B, hB;

    // Membrane variables needed for calculating collision
    double rhoM, HM, D, TS1, EM, sigma0M, sigma1M, hM;

    // Force variables
    double Fs1b, Fs2b, Fs3b, Fbml, Fbmr;

    // Connection Points
    int lS1c, lS2c, lS3c, lBc1, lBc2, lBc3, lBcl, lBcr, lMcl, lMcr, mMcl, mMcr;

    // Connection point multipliers
    double Fs1bMult, Fs2bMult, Fs3bMult, FbmMult;
};