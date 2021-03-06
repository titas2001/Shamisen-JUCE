/*
  ==============================================================================

    ShamisenMembrane.cpp
    Created: 17 Feb 2021 2:33:05pm
    Author:  tlasi

  ==============================================================================
*/
#include "../JuceLibraryCode/JuceHeader.h"
#include "ShamisenMembrane.h"

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
ratio(*parameters.getVarPointer("ratio")),
k (k)
{
    D = E*H*H*H/(12.0*(1.0 - nu*nu));
    cSq = T / (rho * H);                // Calculate wave speed (squared)
    kappaSq = D / (rho * H);        // Calculate stiffness coefficient squared

    double stabilityTerm = cSq * k * k + 4.0 * sigma1 * k; // just easier to write down below
    h = sqrt (stabilityTerm + sqrt ((stabilityTerm * stabilityTerm) + 16.0 * kappaSq * k * k));
    
    if (h < 0.03) // makes the membrane 33x33 size
        h = 0.03;
        
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
    
    // set coefficients for update equation
    h4 = h*h*h*h; // h^4
    
    D1 = 1.0 / (1.0 + sigma0 * k);                                         // u{l,m}^{n+1}
    
    A1 = (-20.0*kappaSq/h4 - 4.0*cSq/(h*h) - 8.0*sigma1/(k*h*h))*(k*k) + 2.0;  // u_{l,m}^n
    A2 = (8.0*kappaSq/h4 + cSq/(h*h) + 2.0*sigma1/(k*h*h))*(k*k);              // u_{l+-1 || m+-1}^n
    A3 = (-2.0*kappaSq*(k*k))/(h4);                                          // u_{l+-2 && m+-2}^n
    A4 = (-1.0*kappaSq*(k*k))/(h4);                                          // u_{l+-2 || m+-2}^{n}
    A5 = ((8.0*sigma1*(k*k))/(k*h*h) + k*sigma0 - 1.0);                        // u_{l,m}^{n-1}
    A6 = ((-2.0*sigma1*(k*k))/(k*h*h));                                      // u_{l+-1 || m+-1}^{n-1}
    
    // Divide by u_{l,m}^{n+1} term
    A1 *= D1;
    A2 *= D1;
    A3 *= D1;
    A4 *= D1;
    A5 *= D1;
    A6 *= D1;
    //A6 = A6;
}

ShamisenMembrane::~ShamisenMembrane()
{
}
double ShamisenMembrane::clamp(double in, double min, double max)
{
    if (in > max)
        return max;
    else if (in < min)
        return min;
    else
        return in;
}

void ShamisenMembrane::paint(Graphics& g)
{

    float stateWidth = getWidth() / static_cast<double> (Nx - 4);
    float stateHeight = getHeight() / static_cast<double> (Ny - 4);
    int scaling = 10000;

    for (int x = 2; x < Nx - 2; ++x)
    {
        for (int y = 2; y < Ny - 2; ++y)
        {
            int cVal = clamp(255 * 0.5 * (u[1][x][y] * scaling + 1), 0, 255);
            g.setColour(Colour::fromRGBA(cVal, cVal, 255, 127));
            g.fillRect((x - 2) * stateWidth, (y - 2) * stateHeight, stateWidth, stateHeight);
        }
    }
}
/*
void ShamisenMembrane::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText ("SimpleMembrane", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}
*/
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
 
    int width = floor(Nx/2);
    double pos = 0.23;
    int startL = floor((Nx+1) * pos);
    int endL = startL+width;
    int startM = floor((Ny+1) * pos);
    int endM = startM + width;

    // note the addition here
    for (int l = startL; l < endL; ++l)
    {   
        for (int m = startM; m < endM; ++m)
        {
            u[1][l][m] += 0.001 * (1 - cos(2.0 * 3.14 * (l - startL) / (endL-startL))) *(1 - cos(2.0 * double_Pi * (m - startM) / (endM - startM)));
            u[2][l][m] += 0.001 * (1 - cos(2.0 * 3.14 * (l - startL) / (endL - startL))) *(1 - cos(2.0 * double_Pi * (m - startM) / (endM - startM)));
        }
    }
}

void ShamisenMembrane::mouseDown (const MouseEvent& e)
{
    excite();
}
