/*
  ==============================================================================

	Shamisen.cpp
	Created: 23 Feb 2021 2:11:27pm
	Author:  tlasi

  ==============================================================================
*/
#include <JuceHeader.h>
#include "Shamisen.h"


Shamisen::Shamisen(NamedValueSet& parameters, double k) :
	k(k),
	rhoS(*parameters.getVarPointer("rhoS")),
	ES(*parameters.getVarPointer("ES")),
	sigma0S(*parameters.getVarPointer("sigma0S")),
	AS1(*parameters.getVarPointer("A1")),
	TS1(*parameters.getVarPointer("T1")),
	AS2(*parameters.getVarPointer("A2")),
	TS2(*parameters.getVarPointer("T2")),
	AS3(*parameters.getVarPointer("A3")),
	TS3(*parameters.getVarPointer("T3")),
	AB(*parameters.getVarPointer("AB")),
	EB(*parameters.getVarPointer("EB")),
	HB(*parameters.getVarPointer("HB")),
	rhoB(*parameters.getVarPointer("rhoB")),
	sigma0B(*parameters.getVarPointer("sigma0B")),
	rhoM(*parameters.getVarPointer("rhoM")),
	HM(*parameters.getVarPointer("HM")),
	sigma0M(*parameters.getVarPointer("sigma0M"))
{
	myShamisenString1 = std::make_shared<ShamisenString>(parameters, k, "1");
	myShamisenString2 = std::make_shared<ShamisenString>(parameters, k, "2");
	myShamisenString3 = std::make_shared<ShamisenString>(parameters, k, "3");
	myShamisenBridge = std::make_shared<ShamisenBridge>(parameters, k);
	myShamisenMembrane = std::make_shared<ShamisenMembrane>(parameters, k);

	addAndMakeVisible(myShamisenMembrane.get());
	addAndMakeVisible(myShamisenBridge.get());
	addAndMakeVisible(myShamisenString1.get());
	addAndMakeVisible(myShamisenString2.get());
	addAndMakeVisible(myShamisenString3.get());
	// Get N's
	NS1 = myShamisenString1->bringN();
	NS2 = myShamisenString2->bringN();
	NS3 = myShamisenString3->bringN();
	NB = myShamisenBridge->bringN();
	NMx = myShamisenMembrane->bringNx();
	NMy = myShamisenMembrane->bringNy();
	// Get h's
	hS1 = myShamisenString1->bring_h();
	hS2 = myShamisenString2->bring_h();
	hS3 = myShamisenString3->bring_h();
	hB = myShamisenBridge->bring_h();
	hM = myShamisenMembrane->bring_h();
	//  CONNECTION LOCATIONS
	//  String
	lS1c = floor((2.0 * NS1) / (double_Pi * 7.0));
	lS2c = floor((2.0*NS2)/(double_Pi *7.0));
	lS3c = floor((2.0*NS3)/(double_Pi *7.0));
	// Bridge
	//lBc1 = floor((NB / 2.0));
	lBc1 = floor((NB / 2.0 - 1.0) / 2.0);
	lBc2 = floor(NB/2);
	lBc3 = ceil((NB/2 - 1)/2)+lBc2;
	lBcl = 0;
	lBcr = NB;
	// Membrane
	lMcl = NMx - floor(3.0 * NMx / 5.0);
	lMcr = NMx - floor(2.0 * NMx / 5.0);
	mMcl = NMy - floor(NMy / 4.0);
	mMcr = NMy - floor(NMy / 4.0);

	// Calculate the force multipliers
	Fs1bMult = 1.0 / ((-k * k / (rhoS * AS1 * hS1 * (sigma0S*k + 1.0))) - (k * k / (rhoB * AB * hB * (sigma0B*k + 1.0))));
	Fs2bMult = 1.0 / ((-k * k / (rhoS * AS2 * hS2 * (sigma0S*k + 1.0))) - (k * k / (rhoB * AB * hB * (sigma0B*k + 1.0))));
	Fs3bMult = 1.0 / ((-k * k / (rhoS * AS3 * hS3 * (sigma0S*k + 1.0))) - (k * k / (rhoB * AB * hB * (sigma0B*k + 1.0))));
	FbmMult = 1.0 / ((-k * k / (rhoB * AB * hB * (sigma0B*k + 1.0))) - (k * k / (rhoM * HM * hM * hM * (sigma0M*k + 1.0))));
	//FbmMult = FbmMult;

}

Shamisen::~Shamisen()
{
}
void Shamisen::paint(Graphics& g)
{
	g.setColour(Colours::yellow);
}

void Shamisen::calculateUpdateEqs()
{
	myShamisenString1->calculateScheme();
	myShamisenString2->calculateScheme();
	myShamisenString3->calculateScheme();
	myShamisenBridge->calculateScheme();
	myShamisenMembrane->calculateScheme();
}
void Shamisen::resized()
{
	Rectangle<int> totArea = getLocalBounds();
	myShamisenBridge->setBounds(totArea.removeFromRight(getWidth() / 8.0));
	myShamisenString1->setBounds(totArea.removeFromTop(getHeight() / 12.0));
	myShamisenString2->setBounds(totArea.removeFromTop(getHeight() / 12.0));
	myShamisenString3->setBounds(totArea.removeFromTop(getHeight() / 12.0));
	myShamisenMembrane->setBounds(totArea);
}
void Shamisen::solveSystem()
{
	Fs1b = Fs1bMult * (-myShamisenString1->getStateAt(0,lS1c) + myShamisenBridge->getStateAt(0, lBc1));
	Fs2b = Fs2bMult * (-myShamisenString2->getStateAt(0,lS2c) + myShamisenBridge->getStateAt(0, lBc2));
	Fs3b = Fs3bMult * (-myShamisenString3->getStateAt(0,lS3c) + myShamisenBridge->getStateAt(0, lBc3));

	// Force from bridge' left and right mounting points to the plate
	Fbml = FbmMult * (- myShamisenBridge->getStateAt(0, lBcl) + myShamisenMembrane->getStateAt(0, lMcl, mMcl));
	Fbmr = FbmMult * (- myShamisenBridge->getStateAt(0, lBcr) + myShamisenMembrane->getStateAt(0, lMcr, mMcr));

    // Bridge -> String
	myShamisenString1->addToStateAt(0,lS1c,-1.0*(Fs1b*k*k / (rhoS * AS1 * hS1 * (sigma0S*k + 1))));
	myShamisenString2->addToStateAt(0,lS2c,-1.0*(Fs2b*k*k / (rhoS * AS2 * hS2 * (sigma0S*k + 1))));
	myShamisenString3->addToStateAt(0,lS3c,-1.0*(Fs3b*k*k / (rhoS * AS3 * hS3 * (sigma0S*k + 1))));
    
	// String -> Bridge
	myShamisenBridge->addToStateAt(0,lBc1, (Fs1b*k*k / (rhoB * AB * hB * (sigma0B*k + 1))));
	myShamisenBridge->addToStateAt(0,lBc2, (Fs2b*k*k / (rhoB * AB * hB * (sigma0B*k + 1))));
	myShamisenBridge->addToStateAt(0,lBc3, (Fs3b*k*k / (rhoB * AB * hB * (sigma0B*k + 1))));
	
	// Membrane -> Bridge
	myShamisenBridge->addToStateAt(0, lBcl, -1.0*(Fbml*k*k / (rhoB * AB * hB * (sigma0B*k + 1))));
	myShamisenBridge->addToStateAt(0, lBcr, -1.0*(Fbmr*k*k / (rhoB * AB * hB * (sigma0B*k + 1))));
	
	// Bridge -> Membrane
	myShamisenMembrane->addToStateAt(0, lMcr, mMcr, (Fbml*k*k / (rhoM * HM * hM * hM * (sigma0M*k + 1))));
	myShamisenMembrane->addToStateAt(0, lMcr, mMcr, (Fbmr*k*k / (rhoM * HM * hM * hM * (sigma0M*k + 1))));
}
void Shamisen::updateStates() {
	myShamisenString1->updateStates();
	myShamisenString2->updateStates();
	myShamisenString3->updateStates();
	myShamisenBridge->updateStates();
	myShamisenMembrane->updateStates();
}
