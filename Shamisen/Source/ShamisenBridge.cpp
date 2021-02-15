/*
  ==============================================================================

    ShamisenBridge.cpp
    Created: 15 Feb 2021 1:36:53pm
    Author:  tlasi

  ==============================================================================
*/
#include <JuceHeader.h>
#include "ShamisenBridge.h"
//==============================================================================
ShamisenBridge::ShamisenBridge (NamedValueSet& parameters, double k) :
L (*parameters.getVarPointer ("LB")),
rho (*parameters.getVarPointer ("rhoB")),
A (*parameters.getVarPointer ("AB")),
E (*parameters.getVarPointer ("EB")),
I (*parameters.getVarPointer ("IB")),
sigma0 (*parameters.getVarPointer ("sigma0B")),
sigma1 (*parameters.getVarPointer ("sigma1B")),
H (*parameters.getVarPointer ("HB")),
k (k)
{
    kappaSq = E * H * H / (12 * rho);        // Calculate stiffness coefficient squared

    double stabilityTerm = 4.0 * sigma1 * k; // just easier to write down below
    
    h = sqrt (stabilityTerm + sqrt ((stabilityTerm * stabilityTerm) + 16.0 * kappaSq * k * k));
    N = floor (L / h);
    h = 1.0 / N; // recalculate h
    
    muSq = kappaSq * k * k / (h * h * h * h);
    
    // initialise vectors
    uStates.reserve (3); // prevents allocation errors
    
    for (int i = 0; i < 3; ++i)
        uStates.push_back (std::vector<double> (N+1, 0));
    
    u.resize (3);
    
    /*  Make u pointers point to the first index of the state vectors.
        To use u (and obtain a vector from the state vectors) use indices like u[n][l] where,
             - n = 0 is u^{n+1},
             - n = 1 is u^n, and
             - n = 2 is u^{n-1}.
        Also see calculateScheme()
     */
    
    for (int i = 0; i < 3; ++i)
        u[i] = &uStates[i][0];
    
    // set coefficients for update equation
    B1 = sigma0 * k;
    B2 = (2.0 * sigma1 * k) / (h * h);
    
    D = 1.0 / (1.0 + sigma0 * k);
    
    A1 = 2.0 - 6.0 * muSq - 2.0 * B2; // u_l^n
    A2 = 4.0 * muSq + B2;                   // u_{l+-1}^n
    A3 = -muSq;                                        // u_{l+-2}^n
    A4 = B1 - 1.0 + 2.0 * B2;                          // u_l^{n-1}
    A5 = -B2;                                          // u_{l+-1}^{n-1}
    
    // Divide by u_l^{n+1} term
    A1 *= D;
    A2 *= D;
    A3 *= D;
    A4 *= D;
    A5 *= D;
}

ShamisenBridge::~ShamisenBridge()
{
}

void ShamisenBridge::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText ("Bridge", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void ShamisenBridge::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void ShamisenBridge::calculateScheme()
{
    for (int l = 2; l < N-1; ++l) // clamped boundaries
        u[0][l] = A1 * u[1][l] + A2 * (u[1][l + 1] + u[1][l - 1]) + A3 * (u[1][l + 2] + u[1][l - 2])
                + A4 * u[2][l] + A5 * (u[2][l + 1] + u[2][l - 1]);
    
}

void ShamisenBridge::updateStates()
{
    double* uTmp = u[2];
    u[2] = u[1];
    u[1] = u[0];
    u[0] = uTmp;
}

void ShamisenBridge::excite()
{
    // Arbitrary excitation function. Just used this for testing purposes
    
    double width = 10;
    double pos = 0.3;
    int start = floor((N+1) * pos);
    int end = start+width;

    // note the addition here

    for (int l = start; l < end; ++l)
    {
        u[1][l] += 0.5 * (1 - cos(2.0 * double_Pi * l / width));
        u[2][l] += 0.5 * (1 - cos(2.0 * double_Pi * l / width));
    }
    
}

void ShamisenBridge::mouseDown (const MouseEvent& e)
{
    excite();
}
