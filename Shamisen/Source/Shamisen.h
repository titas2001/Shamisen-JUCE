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

    double getOutput(double ratioB, double ratioS1, double ratioS2, double ratioS3) {
        return myShamisenBridge->getOutput(ratioB)+ 
            myShamisenString1->getOutput(ratioS1) + myShamisenString2->getOutput(ratioS2) + myShamisenString3->getOutput(ratioS3)
            + myShamisenMembrane->getOutput();
        //Logger::getCurrentLogger()->outputDebugString("String1 = "+String(myShamisenString1->getOutput(ratioS1)));
        //Logger::getCurrentLogger()->outputDebugString("String2 = " + String(myShamisenString2->getOutput(ratioS2)));
        //Logger::getCurrentLogger()->outputDebugString("String3 = " + String(myShamisenString3->getOutput(ratioS3)));
    };
    

    std::shared_ptr<ShamisenString> getString1() { return myShamisenString1; };
    std::shared_ptr<ShamisenString> getString2() { return myShamisenString2; };
    std::shared_ptr<ShamisenString> getString3() { return myShamisenString3; };
    std::shared_ptr<ShamisenBridge> getBridge() { return myShamisenBridge; };
    std::shared_ptr<ShamisenMembrane> getMembrane() { return myShamisenMembrane; };

private:
    double k;
    // Instrument components (String, body and bridge)
    std::shared_ptr<ShamisenString> myShamisenString1;
    std::shared_ptr<ShamisenString> myShamisenString2;
    std::shared_ptr<ShamisenString> myShamisenString3;
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
};