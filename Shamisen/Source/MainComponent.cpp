#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Moved setSize() (which calls resized) to prepareToPlay as our components need a sample rate before they can get initialised.
    
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
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    //// Set the paramters ///
    NamedValueSet parameters;
    
    // parameters you'll use to initialise more than one other parameter should be defined here
    double r = 4.15e-4;
    
    parameters.set ("L", 1);
    parameters.set ("rho", 1156.481);
    parameters.set ("A", r * r * double_Pi);
    parameters.set ("T", 138.67);
    parameters.set ("E", 9.9e9);
    parameters.set ("I", r * r * r * r * double_Pi * 0.25);
    parameters.set ("sigma0", 1.378);
    parameters.set ("sigma1", 3.57e-3);
    
    parameters.set ("LB", 1);
    parameters.set ("HB", 0.0075);
    parameters.set ("bB", 2.69e-3);
    parameters.set ("rhoB", 1190.0);
    parameters.set ("AB", 2.69e-3 * 0.0075);
    parameters.set ("EB", 3.2e9);
    parameters.set ("sigma0B", 1.34);
    parameters.set ("sigma1B", 4.59e-3);
    
    parameters.set ("LM", 1);
    parameters.set ("rhoM", 1150.0);
    parameters.set ("TM", 400.0);
    parameters.set ("EM", 3e9);
    parameters.set ("HM", 0.0002);
    parameters.set ("sigma0M", 1.37806);
    parameters.set ("sigma1M", 9.60559e-2);
    parameters.set ("nu", 0.4);
    parameters.set ("Nx", 40);
    parameters.set ("Ny", 40);
    
    
    //// Initialise an instance of the SimpleString class ////
    myShamisenString = std::make_unique<ShamisenMembrane> (parameters, 1.0 / sampleRate);
    
    addAndMakeVisible (myShamisenString.get()); // add the string to the application
    
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
        myShamisenString->calculateScheme();
        myShamisenString->updateStates();
        
        output = myShamisenString->getOutput (0.8); // get output at 0.8L of the string
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
    myShamisenString->setBounds (getLocalBounds());
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
