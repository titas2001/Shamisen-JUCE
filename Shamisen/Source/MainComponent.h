#pragma once

#include <JuceHeader.h>
#include "Shamisen.h"
//#include "ShamisenString.h"
//#include "ShamisenBridge.h"
//#include "ShamisenMembrane.h"
//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent,
                       public juce::Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void timerCallback() override;
    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

    double limit (double val); // limiter for your ears
    
private:
    //==============================================================================
    // Your private member variables go here...
    std::unique_ptr<Shamisen> myShamisen;
    std::shared_ptr<ShamisenString> myShamisenString1;
    std::shared_ptr<ShamisenString> myShamisenString2;
    std::shared_ptr<ShamisenString> myShamisenString3;
    std::shared_ptr<ShamisenBridge> myShamisenBridge;
    std::shared_ptr<ShamisenMembrane> myShamisenMembrane;

    bool graphicsToggle = true;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
