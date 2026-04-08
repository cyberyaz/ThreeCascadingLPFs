#pragma once
#include <JuceHeader.h>

class TripleBiquadAudioProcessor : public juce::AudioProcessor
{
public:
    TripleBiquadAudioProcessor();
    ~TripleBiquadAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "TripleBiquad"; }
    bool acceptsMidi() const override  { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock&) override {}
    void setStateInformation (const void*, int) override {}

    juce::AudioProcessorValueTreeState apvts;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Biquad state variables
    float x1PrevL=0, x1PrevR=0, x1PrevPrevL=0, x1PrevPrevR=0;
    float y1PrevL=0, y1PrevR=0, y1PrevPrevL=0, y1PrevPrevR=0;
    float x2PrevL=0, x2PrevR=0, x2PrevPrevL=0, x2PrevPrevR=0;
    float y2PrevL=0, y2PrevR=0, y2PrevPrevL=0, y2PrevPrevR=0;
    float x3PrevL=0, x3PrevR=0, x3PrevPrevL=0, x3PrevPrevR=0;
    float y3PrevL=0, y3PrevR=0, y3PrevPrevL=0, y3PrevPrevR=0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TripleBiquadAudioProcessor)
};
