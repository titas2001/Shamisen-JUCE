#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Moved setSize() (which calls resized) to prepareToPlay as our components need a sample rate before they can get initialised.
    Timer::startTimerHz(15);
    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
}

MainComponent::~MainComponent()
{
	// This shuts down the audio device and clears the audio source.
	Timer::stopTimer();

	shutdownAudio();
}
void MainComponent::timerCallback()
{
	if (graphicsToggle)
		repaint();
}
//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    //// Set the paramters ///
    NamedValueSet parameters;
    
    // parameters you'll use to initialise more than one other parameter should be defined here

    //=======STRINGs=======================================

    double r1 = 4.15e-4;
    double r2 = 2.83e-4;
    double r3 = 2.10e-4;
    
    parameters.set ("sigma01", 1.378);
    parameters.set ("sigma11", 3.57e-3);
    parameters.set ("rhoS", 1156.481);
    parameters.set ("ES", 9.9e9);
    
    parameters.set ("L1", 1);
    parameters.set ("A1", r1 * r1 * double_Pi);
    parameters.set ("T1", 138.67);
    parameters.set ("I1", r1 * r1 * r1 * r1 * double_Pi * 0.25);

    parameters.set("L2", 1);
    parameters.set("A2", r2 * r2 * double_Pi);
    parameters.set("T2", 145.53);
    parameters.set("I2", r2 * r2 * r2 * r2 * double_Pi * 0.25);

    parameters.set("L3", 1);
    parameters.set("A3", r3 * r3 * double_Pi);
    parameters.set("T3", 140.73);
    parameters.set("I3", r3 * r3 * r3 * r3 * double_Pi * 0.25);

    //=======BRIDGE======================================
    
    parameters.set ("LB", 1);
    parameters.set ("HB", 0.0075);
    parameters.set ("bB", 2.69e-3);
    parameters.set ("rhoB", 500.0);
    parameters.set ("AB", 2.69e-3 * 0.0075);
    parameters.set ("EB", 9.5e9);
    parameters.set ("sigma0B", 1.34);
    parameters.set ("sigma1B", 7.59e-2);
    
    //=======MEMBRANE====================================

    parameters.set ("Lx", 1);
    parameters.set ("Ly", 1);
    parameters.set ("rhoM", 1150.0);
    parameters.set ("TM", 4000.0);
    parameters.set ("EM", 3e9);
    parameters.set ("HM", 0.0002);
    parameters.set ("sigma0M", 1.37806 );
    parameters.set ("sigma1M", 1.539e-4);
    parameters.set ("nu", 0.4);
    
    //// Initialise an instance of the SimpleString class ////
    myShamisen = std::make_unique<Shamisen> (parameters, 1.0 / sampleRate);
    addAndMakeVisible (myShamisen.get()); // add the string to the application
    //myShamisenString1 = std::make_unique<ShamisenString> (parameters, 1.0 / sampleRate, "1");
    //addAndMakeVisible (myShamisenString1.get()); // add the string to the application
    //myShamisenString2 = std::make_unique<ShamisenString> (parameters, 1.0 / sampleRate, "2");
    //addAndMakeVisible (myShamisenString2.get()); // add the string to the application
    //myShamisenString3 = std::make_unique<ShamisenString> (parameters, 1.0 / sampleRate, "3");
    //addAndMakeVisible (myShamisenString3.get()); // add the string to the application
    //myShamisenBridge = std::make_unique<ShamisenBridge>(parameters, 1.0 / sampleRate);
    //addAndMakeVisible(myShamisenBridge.get()); // add the Bridge to the application
    //myShamisenMembrane = std::make_unique<ShamisenMembrane>(parameters, 1.0 / sampleRate);
    //addAndMakeVisible (myShamisenMembrane.get()); // add the Membrane to the application
    

    // Moved setSize() (which calls resized) from the constructor to here as our components need a sample rate before they can get initialised.
    setSize (800, 600);

}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    
    // Get pointers to output locations
    float* const channelData1 = bufferToFill.buffer->getWritePointer (0, bufferToFill.startSample);
    float* const channelData2 = bufferToFill.buffer->getWritePointer (1, bufferToFill.startSample);

    float output = 0.0;
    for (int i = 0; i < bufferToFill.numSamples; ++i)
    {
        myShamisen->calculateUpdateEqs();
        myShamisen->updateStates();
        //myShamisenString1->calculateScheme();
        //myShamisenString1->updateStates();
        //myShamisenString2->calculateScheme();
        //myShamisenString2->updateStates();
        //myShamisenString3->calculateScheme();
        //myShamisenString3->updateStates();
        /*myShamisenMembrane->calculateScheme();
        myShamisenMembrane->updateStates();*/
        //myShamisenBridge->calculateScheme();
        //myShamisenBridge->updateStates();

        output = myShamisen->getOutput(0.8, 0.8);
        //output = myShamisenString1->getOutput(0.8); // get output at 0.8L of the string 1
        //output = myShamisenString2->getOutput(0.8); // get output at 0.8L of the string 2
        //output = myShamisenString3->getOutput(0.8); // get output at 0.8L of the string 3
        //output = myShamisenBridge->getOutput (0.8); // get output at 0.8L of the bridge
        
        channelData1[i] = limit (output);
        channelData2[i] = limit (output);
    }
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
}

void MainComponent::resized()
{
    // put the string in the application
    myShamisen->setBounds(getLocalBounds());
    //myShamisenString1->setBounds(getLocalBounds());
    //myShamisenString2->setBounds(getLocalBounds());
    //myShamisenString3->setBounds(getLocalBounds());
    //myShamisenBridge->setBounds (getLocalBounds());
    //myShamisenMembrane->setBounds(getLocalBounds());
}

// limiter
double MainComponent::limit (double val)
{
	if (val < -1)
	{
		val = -1;
		return val;
	}
	else if (val > 1)
	{
		val = 1;
		return val;
	}
	return val;
}
