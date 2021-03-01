/*
  ==============================================================================

    SimpleString.cpp
    Created: 12 Feb 2021 1:10:03pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ShamisenString.h"

//==============================================================================
ShamisenString::ShamisenString (NamedValueSet& parameters, double k, String stringIndex) :
        sigma0(*parameters.getVarPointer("sigma0S")),
        sigma1(*parameters.getVarPointer("sigma1S")),
        rho (*parameters.getVarPointer ("rhoS")),
        E(*parameters.getVarPointer("ES")),
        L(*parameters.getVarPointer("L" + stringIndex)),
        A (*parameters.getVarPointer ("A" + stringIndex)),
        T (*parameters.getVarPointer ("T" + stringIndex)),
        I (*parameters.getVarPointer ("I" + stringIndex)),
        k(k)
{
    cSq = T / (rho * A);                // Calculate wave speed (squared)
    kappaSq = E * I / (rho * A);        // Calculate stiffness coefficient squared

    double stabilityTerm = cSq * k * k + 4.0 * sigma1 * k; // just easier to write down below
    
    h = sqrt ((stabilityTerm + sqrt ((stabilityTerm * stabilityTerm) + 16.0 * kappaSq * k * k))/2.0);
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
    
    for (int i = 0; i < 3; ++i)
        u[i] = &uStates[i][0];
    
    // set coefficients for update equation
    B1 = sigma0 * k;
    B2 = (2.0 * sigma1 * k) / (h * h);
    
    D = 1.0 / (1.0 + sigma0 * k);
    
    A1 = 2.0 - 2.0 * lambdaSq - 6.0 * muSq - 2.0 * B2; // u_l^n
    A2 = lambdaSq + 4.0 * muSq + B2;                   // u_{l+-1}^n
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

ShamisenString::~ShamisenString()
{
}

void ShamisenString::paint (juce::Graphics& g)
{

    float stateWidth = getWidth() / static_cast<double> (N - 4);
    float stateHeight = getHeight() ;
    int scaling = 1000;

    for (int x = 2; x < N - 2; ++x)
    {
        int cVal = clamp(255 * 0.5 * (u[1][x] * scaling + 1), 0, 255);
        g.setColour(Colour::fromRGBA(cVal, 255, cVal, 127));
        g.fillRect(  (x - 2) * stateWidth, 0 * stateHeight, stateWidth, stateHeight);
    }
}

double ShamisenString::clamp(double in, double min, double max)
{
    if (in > max)
        return max;
    else if (in < min)
        return min;
    else
        return in;
}
void ShamisenString::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void ShamisenString::calculateScheme()
{
    for (int l = 2; l < N-1; ++l) // clamped boundaries
        u[0][l] = A1 * u[1][l] + A2 * (u[1][l + 1] + u[1][l - 1]) + A3 * (u[1][l + 2] + u[1][l - 2])
                + A4 * u[2][l] + A5 * (u[2][l + 1] + u[2][l - 1]);
    
}

void ShamisenString::updateStates()
{
    double* uTmp = u[2];
    u[2] = u[1];
    u[1] = u[0];
    u[0] = uTmp;
}

void ShamisenString::excite()
{
    // Arbitrary excitation function. Just used this for testing purposes
    
    double width = 10;
    double pos = 0.3;
    int start = floor((N+1) * pos);
    int end = start+width;

    // note the addition here

    for (int l = start; l < end; ++l)
    {
        u[1][l] += 1e-50 * (1 - cos(2.0 * double_Pi * l / width));
        u[2][l] += 1e-50 * (1 - cos(2.0 * double_Pi * l / width));
    }
    
}

void ShamisenString::mouseDown (const MouseEvent& e)
{
    excite();
}
