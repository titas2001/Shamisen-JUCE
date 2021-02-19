/*
  ==============================================================================

    ShamisenMembrane.h
    Created: 17 Feb 2021 2:33:23pm
    Author:  tlasi

  ==============================================================================
*/

#pragma once


class ShamisenMembrane : public juce::Component 
{
public:
    ShamisenMembrane (NamedValueSet& parameters, double k);
    ~ShamisenMembrane() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void calculateScheme();
    void updateStates();
    
    double getOutput (double Lratio) { 
        for(int i = 2; i<Nx-1; ++i){
            for(int j = 2; j<Ny-1; ++j){
                sum+=u[1][i][j];
            }
        }
        return sum;
    } //return the sum of the vector values
    
    void excite();
    
    void mouseDown (const MouseEvent& e) override;
    
private:
    double T, Lx, Ly, H,  rho, E, cSq, kappaSq, sigma0, sigma1, h, k, nu, D, ratio;
    int Nx, Ny; // number of intervals (N+1 is number of points including boundaries)
    
    // An (N+1) * 3 vector containing the state of the system at all time-steps
    std::vector<std::vector<std::vector<double>>> uStates;
    std::vector<std::vector<double*>> u; // vector of pointers to state vectors
     
    
    // Scheme variables
    double A1, A2, A3, A4, A5, A6, h4, B2, D1;
    // virtual grid points
    double um1 , um2, uPm1, up1, up2, uPp1;
    double sum;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShamisenMembrane)    
};

