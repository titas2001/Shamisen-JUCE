/*
  ==============================================================================

    Shamisen.cpp
    Created: 23 Feb 2021 2:11:27pm
    Author:  tlasi

  ==============================================================================
*/
#include <JuceHeader.h>
#include "Shamisen.h"

Shamisen::Shamisen (NamedValueSet& parameters, double k)  :
                    k (k),
                    rhoS (*parameters.getVarPointer ("rhoS")),
                    AS1 (*parameters.getVarPointer ("A1")),
                    TS1 (*parameters.getVarPointer ("T1")),
                    ES (*parameters.getVarPointer ("ES")),
                    sigma0S1 (*parameters.getVarPointer ("sigma01")),
                    AB (*parameters.getVarPointer ("AB")),
                    EB (*parameters.getVarPointer ("EB")),
                    HB (*parameters.getVarPointer ("HB")),
                    rhoB (*parameters.getVarPointer ("rhoB")),
                    sigma0B (*parameters.getVarPointer ("sigma0B"))
                    rhoP (*parameters.getVarPointer ("rhoP")),
                    HM (*parameters.getVarPointer ("HM")),
                    sigma0M (*parameters.getVarPointer ("sigma0M"))
{
    myShamisenString1 = std::make_shared<ShamisenString> (parameters, k);
    myShamisenBridge = std::make_shared<ShamisenBridge> (parameters, k);
    myShamisenMembrane = std::make_shared<ShamisenMembrane> (parameters, k);
    
    addAndMakeVisible (myShamisenMembrane.get());
    addAndMakeVisible (myShamisenBridge.get());
    addAndMakeVisible (myShamisenString1.get());    
    
    // Calculate the force multipliers
    Fs1bMult = 1.0/(1.0/(rhoB*AB*hB * (sigma0B + 1.0)) + 1.0/(rhoS*AS1*hS1 * (sigma0S1 + 1.0)));
    FbpMult = 1.0/(-1.0/(rhoB*AB*hB * (sigma0B + 1.0)) - 1.0/(rhoM*HM*hM*hM * (sigma0M + 1.0)));
    
    
}

Shamisen::~Shamisen()
{    
}
void Shamien::calculateUpdateEqs()
{
    trombaString->calculateUpdateEq();
    bridge->calculateUpdateEq();
    body->calculateUpdateEq();
}