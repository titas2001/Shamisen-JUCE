/*
  ==============================================================================

    ShamisenMembrane.cpp
    Created: 17 Feb 2021 2:33:05pm
    Author:  tlasi

  ==============================================================================
*/

#include "ShamisenMembrane.h"
/*
//==============================================================================
ShamisenMembrane::ShamisenMembrane (NamedValueSet& parameters, double k) :
L (*parameters.getVarPointer ("LM")),
rho (*parameters.getVarPointer ("rhoM")),
A (*parameters.getVarPointer ("A")),
T (*parameters.getVarPointer ("TM")),
E (*parameters.getVarPointer ("EM")),
I (*parameters.getVarPointer ("I")),
sigma0 (*parameters.getVarPointer ("sigma0M")),
sigma1 (*parameters.getVarPointer ("sigma1M")),
k (k)
{
    cSq = T / (rho * A);                // Calculate wave speed (squared)
    kappaSq = E * I / (rho * A);        // Calculate stiffness coefficient squared

    double stabilityTerm = cSq * k * k + 4.0 * sigma1 * k; // just easier to write down below
    
    h = sqrt (stabilityTerm + sqrt ((stabilityTerm * stabilityTerm) + 16.0 * kappaSq * k * k));
    N = floor (L / h);
    h = 1.0 / N; // recalculate h
    
    lambdaSq = cSq * k * k / (h * h);
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
     /*
    
    for (int i = 0; i < 3; ++i)
        u[i] = &uStates[i][0];
    
    // set coefficients for update equation
    h4 = h*h*h*h; \\ h^4
    
    D = 1.0 / (1.0 + sigma0 * k);                                          // u{l,m}^{n+1}
    
    A1 = (-20.0*kappaSq/h4 - 4.0*cSq/h*h - 8.0*sigma1/(k*h*h))*k*k + 2;    // u_{l,m}^n
    A2 = (8.0*kappaSq/h4 + cSq/h*h + 2.0*sigma1/(k*h*h))*k*k;              // u_{l+-1 || m+-1}^n
    A3 = (-2.0*kappaSq*k*k)/(h4);                                          // u_{l+-2 && m+-2}^n
    A4 = (-1.0*kappaSq*k*k)/(h4);                                          // u_{l+-2 || m+-2}^{n}
    A5 = ((8.0*sigma1*k*k)/(k*h*h) + k*sigma0 - 1);                        // u_{l,m}^{n-1}
    A6 = ((-2.0*sigma1*k*k)/(k*h*h));                                      // u_{l+-1 || m+-1}^{n-1}
    
    // Divide by u_{l,m}^{n+1} term
    A1 *= D;
    A2 *= D;
    A3 *= D;
    A4 *= D;
    A5 *= D;
    A6 *= D;
}

ShamisenMembrane::~ShamisenMembrane()
{
}

void ShamisenMembrane::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */
/*
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText ("SimpleMembrane", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void ShamisenMembrane::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void ShamisenMembrane::calculateScheme()
{
    for (int l = 2; l < Nx-1; ++l) // clamped boundaries
        for (int m = 2; m < Ny - 1; ++m)
        {
            u[0][l+m * Nx] =
            A1 * u[1][l+m * Nx]
            + A2 * (u[1][l + (m+1) * Nx] + u[1][l + (m-1) * Nx] + u[1][l+1 + m * Nx] + u[1][l-1 + m * Nx] )
            + A3 * (u[1][l+1 + (m+1) * Nx] + u[1][l-1 + (m+1) * Nx] + u[1][l+1 + (m-1) * Nx] + u[1][l-1 + (m-1) * Nx])
            + A4 * (u[1][l + (m+2) * Nx] + u[1][l + (m-2) * Nx] + u[1][l+2 + m * Nx] + u[1][l-2 + m * Nx])
            + A5 * u[2][l + m * Nx]
            + A6 * (u[2][l + (m+1) * Nx] + u[2][l + (m-1) * Nx] + u[2][l+1 + m * Nx] + u[2][l-1 + m * Nx]);
        }
    
}

void ShamisenMembrane::updateStates()
{
    double* uTmp = u[2];
    u[2] = u[1];
    u[1] = u[0];
    u[0] = uTmp;
}

void ShamisenMembrane::excite()
{
    // Arbitrary excitation function. Just used this for testing purposes
    
    double width = 10;
    double pos = 0.3;
    int start = floor((N+1) * pos);
    int end = start+width;

    // note the addition here

    for (int l = start; l < end; ++l)
    {   
        for (int m = start; m < end; ++m)
        {
            u[1][l] += 0.5 * (1 - cos(2.0 * double_Pi * l / width));
            u[2][l] += 0.5 * (1 - cos(2.0 * double_Pi * l / width));
        }
    }
}

void ShamisenMembrane::mouseDown (const MouseEvent& e)
{
    excite();
}
*/
