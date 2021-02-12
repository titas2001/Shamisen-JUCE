/*
  ==============================================================================

    ShamisenString.cpp
    Created: 10 Feb 2021 3:33:24pm
    Author:  tlasi

  ==============================================================================
*/
#include "../JuceLibraryCode/JuceHeader.h"

#include "ShamisenString.h"

void updateBuffers(){
    u1Next(lS1) = u1(lS1)*u1lMult + (u1(lS1-1) + u1(lS1+1))*u1l1Mult + (u1(lS1-2) + u1(lS1+2))*u1l2Mult + u1Prev(lS1)*u1PrevlMult + (u1Prev(lS1-1)+ u1Prev(lS1+1))*u1Prevl1Mult;
    
    u1Prev  = u1;
    u1 = u1Next;  
}
void allocateBuffers(int& lS1){
    for (int i =2; i<88; i++){
        lS1[i] = i;
    }
    stringSize = 91;
    u1[]= new float[stringSize];
    u1Prev[]= new float[stringSize];
    u1Next[]= new float[stringSize];
}
float getOutput(float[] u1Next){
    return output = u1Next[10];
}