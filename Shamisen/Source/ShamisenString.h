/*
  ==============================================================================

    ShamisenString.h
    Created: 10 Feb 2021 3:52:11pm
    Author:  tlasi

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

//====================================================

class ShamisenString
{
public: 
    ShamisenString(NamedValueSet& parameters, double k);
    ~ShamisenString();
    
    
    float getOutput();
    void allocateBuffers();
    void updateBuffers();
    
private:
    double k, h;
    int N;
    // Physical parameters
    double L1=1, rho=1156.48151991993, r1=4.15088*1e-4, A1=r1*r1*3.14, T1=14.15*9.8, E=9.9e+9, sigma0=1.378027748373650, s1=3.570213734102943e-03, c1Sq, kappa1Sq, lambda1Sq, mu1Sq;
    int lS1[86];

    //double L2, rho, A2, T2, E, Iner, s0, s1, c2Sq, kappa2Sq, lambda2Sq, mu2Sq;
    //double L3, rho, A3, T3, E, Iner, s0, s1, c3Sq, kappa3Sq, lambda3Sq, mu3Sq;
    // update equation constants
    double uS1lMult, uS1l1Mult, uS1l2Mult, uS1PrevlMult, uS1Prevl1Mult;
    //double uS2lMult, uS2l1Mult, uS2l2Mult, uS2PrevlMult, uS2Prevl1Mult;
    //double uS3lMult, uS3l1Mult, uS3l2Mult, uS3PrevlMult, uS3Prevl1Mult;
    // Newton Variables
    float u1[91], u1Prev[91], u1Next[91];
    //double u2, u2Prev, u2Next;
    //double u3, u3Prev, u3Next;
    
};