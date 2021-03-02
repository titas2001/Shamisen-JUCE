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
H(*parameters.getVarPointer("HB")),
rho (*parameters.getVarPointer ("rhoB")),
A (*parameters.getVarPointer ("AB")),
E (*parameters.getVarPointer ("EB")),
sigma0 (*parameters.getVarPointer ("sigma0B")),
sigma1 (*parameters.getVarPointer ("sigma1B")),
k (k)
{
    kappaSq = E * H * H / (12 * rho);        // Calculate stiffness coefficient squared

    double stabilityTerm = 4.0 * sigma1 * k; // just easier to write down below
    
    h = sqrt ((stabilityTerm + sqrt ((stabilityTerm * stabilityTerm) + 16.0 * kappaSq * k * k))/2.0);
    N = floor (L / h);
    h = 1.0 / N; // recalculate h
    //Logger::getCurrentLogger()->outputDebugString("samples: "+String(N));
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
    
    D = 1.0 / (h*h*h*h*(1.0 + sigma0 * k));                        // u_l^{n+1}
        
    A1 = 2.0*h*h*h*h - 4.0*h*h*k*sigma1 - 6.0*k*k*kappaSq;         // u_l^n
    A2 = 2.0*h*h*k*sigma1 + 4.0*k*k*kappaSq;                       // u_{l+-1}^n
    A3 = -1.0*k*k*kappaSq;                                         // u_{l+-2}^n
    A4 = h*h*h*h*k*sigma0 - h*h*h*h + 4.0*h*h*k*sigma1;            // u_l^{n-1}
    A5 = -2.0*h*h*k*sigma1;                                        // u_{l+-1}^{n-1}
    
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
    float stateWidth = getWidth() ;
    float stateHeight = getHeight() / static_cast<double> (N-4);
    int scaling = 1000;

    for (int x = 2; x < N - 2; ++x)
    {
        int cVal = clamp(255 * 0.5 * (u[1][x] * scaling + 1), 0, 255);
        g.setColour(Colour::fromRGBA(255, cVal, cVal, 127));
        g.fillRect(0 * stateWidth,(x - 2) * stateHeight, stateWidth, stateHeight);
    }
}

double ShamisenBridge::clamp(double in, double min, double max)
{
    if (in > max)
        return max;
    else if (in < min)
        return min;
    else
        return in;
}

void ShamisenBridge::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void ShamisenBridge::calculateScheme()
{
    
	/// Calculate virtual grid points
	um1 = 2.0 * u[1][0] - u[1][1];                      // uB(-1) good
	um2 = 4.0 * (u[1][0] - u[1][1]) + u[1][2];          // uB(-2) basically the same
	uPm1 = 2.0 * u[2][0] - u[2][1];                     // uBPrev(-1) good
	up1 = 2.0 * u[1][N] - u[1][N - 1];                  // uB(N+1) good
	up2 = 4.0 * (u[1][N] - u[1][N - 1]) + u[1][N - 2];  // uB(N+2) basically the same
	uPp1 = 2.0 * u[2][N] - u[2][N - 1];                 // uBPrev(N+1) good
    
    
    u[0][1] = A1 * u[1][1] + A2 * (u[1][2] + u[1][0]) + A3 * (u[1][3] + um1) + A4 * u[2][1] + A5 * (u[2][2] + u[2][0]);
    u[0][0] = A1 * u[1][0] + A2 * (u[1][1] + um1) + A3 * (u[1][2] + um2) + A4 * u[2][0] + A5 * (u[2][1] + uPm1);
    for (int l = 2; l < N-1; ++l) // clamped boundaries
    {
        u[0][l] = A1 * u[1][l] + A2 * (u[1][l + 1] + u[1][l - 1]) + A3 * (u[1][l + 2] + u[1][l - 2]) + A4 * u[2][l] + A5 * (u[2][l + 1] + u[2][l - 1]);
    }
    u[0][N-1] = A1 * u[1][N-1] + A2 * (u[1][N] + u[1][N-2]) + A3 * (up1 + u[1][N-3]) + A4 * u[2][N-1] + A5 * (u[2][N] + u[2][N-2]);
    u[0][N] = A1 * u[1][N] + A2 * (up1 + u[1][N - 1]) + A3 * (up2 + u[1][N - 2]) + A4 * u[2][N] + A5 * (uPp1 + u[2][N - 1]);
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
        u[1][l] += 0.01 * (1 - cos(2.0 * double_Pi * l / width));
        u[2][l] += 0.01 * (1 - cos(2.0 * double_Pi * l / width));
    }
    
}

void ShamisenBridge::mouseDown (const MouseEvent& e)
{
    excite();
}
