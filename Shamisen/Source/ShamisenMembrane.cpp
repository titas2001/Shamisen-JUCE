/*
  ==============================================================================

    ShamisenMembrane.cpp
    Created: 17 Feb 2021 2:33:05pm
    Author:  tlasi

  ==============================================================================
*/
#include "../JuceLibraryCode/JuceHeader.h"
#include "ShamisenMembrane.h"
#include <math.h>  

//==============================================================================
ShamisenMembrane::ShamisenMembrane (NamedValueSet& parameters, double k) :
Lx (*parameters.getVarPointer ("Lx")),
Ly (*parameters.getVarPointer ("Ly")),
rho (*parameters.getVarPointer ("rhoM")),
T (*parameters.getVarPointer ("TM")),
E (*parameters.getVarPointer ("EM")),
H (*parameters.getVarPointer ("HM")),
sigma0 (*parameters.getVarPointer ("sigma0M")),
sigma1 (*parameters.getVarPointer ("sigma1M")),
nu (*parameters.getVarPointer ("nu")),
k (k)
{
    D = E*H*H*H/(12.0*(1.0 - nu*nu));
    cSq = T / (rho * H);                // Calculate wave speed (squared)
    kappaSq = D / (rho * H);        // Calculate stiffness coefficient squared

    double stabilityTerm = cSq * k * k + 4.0 * sigma1 * k; // just easier to write down below
    
    h = sqrt (stabilityTerm + sqrt ((stabilityTerm * stabilityTerm) + 16.0 * kappaSq * k * k));
    
    if (h < 0.09)
        h = 0.09;
        
    Nx = floor(Lx/h);
    Ny = floor(Ly/h);

    
    // initialise vectors
    uStates.reserve (3); // prevents allocation errors

    

    for (int i = 0; i < 3; ++i)

        uStates.push_back (std::vector<std::vector<double>> (Nx+1,

                           std::vector<double> (Ny+1, 0.0)));

    

    u.reserve (3);

    

    for (int i = 0; i < 3; ++i)

        u.push_back (std::vector<double*> (Nx + 1, nullptr));

    

    /*  Make u pointers point to the first index of the state vectors.

        To use u (and obtain a vector from the state vectors) use indices like u[n][l] where,

             - n = 0 is u^{n+1},

             - n = 1 is u^n, and

             - n = 2 is u^{n-1}.

        Also see calculateScheme()

     */

     

    

    for (int n = 0; n < 3; ++n)
        for (int l = 0; l < Nx + 1; ++l)
            u[n][l] = &uStates[n][l][0];
    //===========================================================================================================
  
    /*
    uStates.reserve (3); // prevents allocation errors
    
    for (int i = 0; i < 3; ++i)
        uStates.push_back (std::vector<std::vector<double>>(Nx+1, std::vector<double> (Ny+1, 0.0)));
    
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
        u[i][0] = &uStates[i][0][0];
    */
    
    
    // set coefficients for update equation
    h4 = h*h*h*h; // h^4
    
    D1 = 1.0 / (1.0 + sigma0 * k);                                         // u{l,m}^{n+1}
    
    A1 = (-20.0*kappaSq/h4 - 4.0*cSq/h*h - 8.0*sigma1/(k*h*h))*k*k + 2.0;  // u_{l,m}^n
    A2 = (8.0*kappaSq/h4 + cSq/h*h + 2.0*sigma1/(k*h*h))*k*k;              // u_{l+-1 || m+-1}^n
    A3 = (-2.0*kappaSq*k*k)/(h4);                                          // u_{l+-2 && m+-2}^n
    A4 = (-1.0*kappaSq*k*k)/(h4);                                          // u_{l+-2 || m+-2}^{n}
    A5 = ((8.0*sigma1*k*k)/(k*h*h) + k*sigma0 - 1.0);                        // u_{l,m}^{n-1}
    A6 = ((-2.0*sigma1*k*k)/(k*h*h));                                      // u_{l+-1 || m+-1}^{n-1}
    
    // Divide by u_{l,m}^{n+1} term
    A1 *= D1;
    A2 *= D1;
    A3 *= D1;
    A4 *= D1;
    A5 *= D1;
    A6 *= D1;
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
            u[0][l][m] =
              A1 * u[1][l][m]
            + A2 * (u[1][l][m+1] + u[1][l][m-1] + u[1][l+1][m] + u[1][l-1][m])
            + A3 * (u[1][l+1][m+1] + u[1][l-1][m+1] + u[1][l+1][m-1] + u[1][l-1][m-1])
            + A4 * (u[1][l][m+2] + u[1][l][m-2] + u[1][l+2][m] + u[1][l-2][m])
            + A5 * u[2][l][m]
            + A6 * (u[2][l][m+1] + u[2][l][m-1] + u[2][l+1][m] + u[2][l-1][m]);
        }
    
}

void ShamisenMembrane::updateStates()
{
   for (int l = 0; l <= Nx; ++l)
    {
        double* uTmp = u[2][l];
        u[2][l] = u[1][l];
        u[1][l] = u[0][l];
        u[0][l] = uTmp;
    }
}

void ShamisenMembrane::excite()
{
    // Arbitrary excitation function. Just used this for testing purposes
    
    /*double width = 10;
    double pos = 0.3;
    int start = floor((Nx+1) * pos);
    int end = start+width;

    // note the addition here

    for (int l = start; l < end; ++l)
    {   
        for (int m = start; m < end; ++m)
        {
            u[1][l][m] += 0.5 * (1 - cos(2.0 * double_Pi * l / width));
            u[2][l][m] += 0.5 * (1 - cos(2.0 * double_Pi * l / width));
        }
    }*/
    u[1][4][4] = 1;
    u[2][4][4] = 1;
}

void ShamisenMembrane::mouseDown (const MouseEvent& e)
{
    excite();
}
