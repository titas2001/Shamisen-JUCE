/*
  ==============================================================================

	Shamisen.cpp
	Created: 23 Feb 2021 2:11:27pm
	Author:  tlasi

  ==============================================================================
*/
#include <JuceHeader.h>
#include "Shamisen.h"
#define _USE_MATH_DEFINES // for C++
#include <cmath>


Shamisen::Shamisen(NamedValueSet& parameters, double k) :
	k(k),
	rhoS(*parameters.getVarPointer("rhoS")),
	AS1(*parameters.getVarPointer("A1")),
	TS1(*parameters.getVarPointer("T1")),
	ES(*parameters.getVarPointer("ES")),
	sigma0S(*parameters.getVarPointer("sigma01")),
	AB(*parameters.getVarPointer("AB")),
	EB(*parameters.getVarPointer("EB")),
	HB(*parameters.getVarPointer("HB")),
	rhoB(*parameters.getVarPointer("rhoB")),
	sigma0B(*parameters.getVarPointer("sigma0B")),
	rhoM(*parameters.getVarPointer("rhoM")),
	HM(*parameters.getVarPointer("HM")),
	sigma0M(*parameters.getVarPointer("sigma0M"))
{
	myShamisenString1 = std::make_shared<ShamisenString>(parameters, k);
	myShamisenBridge = std::make_shared<ShamisenBridge>(parameters, k);
	myShamisenMembrane = std::make_shared<ShamisenMembrane>(parameters, k);

	addAndMakeVisible(myShamisenMembrane.get());
	addAndMakeVisible(myShamisenBridge.get());
	addAndMakeVisible(myShamisenString1.get());
	//  CONNECTION LOCATIONS
	//  String
	lS1c = floor((2.0 * NS1) / (3.14 * 7.0));
	//lS2c = floor((2.0*NS2)/(M_PI*7.0));
	//lS3c = floor((2.0*NS3)/(M_PI*7.0));
	// Bridge
	lBc1 = floor((NB / 2.0 - 1.0) / 2.0);
	//lBc2 = floor(NB/2);
	//lBc3 = ceil((NB/2 - 1)/2)+lBc2;
	lBcl = 1;
	lBcr = NB;
	// Membrane
	lMcl = NMx - floor(3.0 * NMx / 5.0);
	lMcr = NMx - floor(2.0 * NMx / 5.0);
	mMcl = NMy - floor(NMy / 4.0);
	mMcr = NMy - floor(NMy / 4.0);


	// Calculate the force multipliers
	Fs1bMult = 1.0 / (1.0 / (rhoB * AB * hB * (sigma0B + 1.0)) + 1.0 / (rhoS * AS1 * hS1 * (sigma0S + 1.0)));
	FbmMult = 1.0 / (-1.0 / (rhoB * AB * hB * (sigma0B + 1.0)) - 1.0 / (rhoM * HM * hM * hM * (sigma0M + 1.0)));


}

Shamisen::~Shamisen()
{
}
void Shamisen::paint(Graphics& g)
{
	g.setColour(Colours::yellow);
	//trombaString->setBridgeState (bridge->getState (1));
	//bridge->setBodyState (body->getStateAt (1, cPX, cPY));
}

void Shamisen::calculateUpdateEqs()
{
	myShamisenString1->calculateScheme();
	myShamisenBridge->calculateScheme();
	myShamisenMembrane->calculateScheme();
}
void Shamisen::resized()
{
	Rectangle<int> totArea = getLocalBounds();
	myShamisenBridge->setBounds(totArea.removeFromRight(getWidth() / 8.0));
	myShamisenString1->setBounds(totArea.removeFromTop(getHeight() / 2.0));
	myShamisenMembrane->setBounds(totArea);
}
void Shamisen::solveSystem()
{
	Fs1b = Fs1bMult * (-v[0][lBc1] + u1[0][lS1c]);
	//Fs2b = Fs2bMult * (-uBNext(lBc2) + uS2Next(lS2c));
	//Fs3b = Fs3bMult * (-uBNext(lBc3) + uS3Next(lS3c));

	// Force from bridge' left and right mounting points to the plate
	Fbml = FbmMult * (-v[0][lBcl] + w[0][lMcl][mMcl]);
	Fbmr = FbmMult * (-v[0][lBcr] + w[0][lMcr][mMcr]);


	u1[0][lS1c] -= Fs1b / (rhoS * AS1 * hS1 * (sigma0S + 1));
	//u2[0][lS2c] -= Fs2b / (rhoS * AS2 * hS2 * (sigma0S + 1));
	//u3[0][lS3c] -= Fs3b / (rhoS * AS3 * hS3 * (sigma0S + 1));

	v[0][lBc1] += Fs1b / (rhoB * AB * hB * (sigma0B + 1));
	//Update Bar function at the localizer point lBr with Fbp (Force gain from the string 1)
	v[0][lBcl] -= Fbml / (rhoB * AB * hB * (sigma0B + 1));
	v[0][lBcr] -= Fbmr / (rhoB * AB * hB * (sigma0B + 1));
	// Update Plate function at the localizer points with Fbp (Force gain from the bridge)
	w[0][lMcr][mMcr] += Fbml / (rhoM * HM * hM * hM * (sigma0M + 1));
	w[0][lMcr][mMcr] += Fbmr / (rhoM * HM * hM * hM * (sigma0M + 1));
}
void Shamisen::updateStates() {
	myShamisenString1->updateStates();
	myShamisenBridge->updateStates();
	myShamisenMembrane->updateStates();
}
