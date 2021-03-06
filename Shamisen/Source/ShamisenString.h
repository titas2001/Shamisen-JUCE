/*
  ==============================================================================

    SimpleString.h
    Created: 12 Feb 2021 1:10:03pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class ShamisenString  : public juce::Component
{
public:
    ShamisenString (NamedValueSet& parameters, double k, String stringIndex);
    ~ShamisenString() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void calculateScheme();
    void updateStates();
    
    double getOutput (double Lratio) { return u[1][static_cast<int> (round((N+1) * Lratio))]; } //return u at the current sample at a location given by the length ratio
    double getStateAt (int time, int lc) { return u[time][lc]; };
    void setStateAt (int time, int lc, double val) { u[time][lc] = val; }; 
    void addToStateAt (int time, int lc, double val) { u[time][lc] += val; }; 
    int bringN() { return N; };
    double bring_h() { return h; };
    double clamp(double in, double min, double max);
    void excite();
    void mouseDown (const MouseEvent& e) override;
    
private:
    double L, rho, A, T, E, I, cSq, kappaSq, sigma0, sigma1, lambdaSq, muSq, h, k;
    int N; // number of intervals (N+1 is number of points including boundaries)
    
    // An (N+1) * 3 vector containing the state of the system at all time-steps
    std::vector<std::vector<double>> uStates;
    std::vector<double*> u; // vector of pointers to state vectors
    
    
    // Scheme variables
    double A1, A2, A3, A4, A5, B1, B2, D;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShamisenString)
};
