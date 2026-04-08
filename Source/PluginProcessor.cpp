#include "PluginProcessor.h"
#include "PluginEditor.h"

TripleBiquadAudioProcessor::TripleBiquadAudioProcessor()
    : AudioProcessor (BusesProperties()
                      .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", createParameterLayout())
{
}

TripleBiquadAudioProcessor::~TripleBiquadAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout TripleBiquadAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>("cutoff1", "Cutoff 1",
        juce::NormalisableRange<float>(20.f, 20000.f, 0.1f, 0.3f), 1000.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("cutoff2", "Cutoff 2",
        juce::NormalisableRange<float>(20.f, 20000.f, 0.1f, 0.3f), 1000.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("cutoff3", "Cutoff 3",
        juce::NormalisableRange<float>(20.f, 20000.f, 0.1f, 0.3f), 1000.f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("q1", "Q 1",
        juce::NormalisableRange<float>(0.5f, 12.f, 0.01f), 0.707f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("q2", "Q 2",
        juce::NormalisableRange<float>(0.5f, 12.f, 0.01f), 0.707f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("q3", "Q 3",
        juce::NormalisableRange<float>(0.5f, 12.f, 0.01f), 0.707f));

    return { params.begin(), params.end() };
}

void TripleBiquadAudioProcessor::prepareToPlay (double, int)
{
    x1PrevL=x1PrevR=x1PrevPrevL=x1PrevPrevR=0;
    y1PrevL=y1PrevR=y1PrevPrevL=y1PrevPrevR=0;
    x2PrevL=x2PrevR=x2PrevPrevL=x2PrevPrevR=0;
    y2PrevL=y2PrevR=y2PrevPrevL=y2PrevPrevR=0;
    x3PrevL=x3PrevR=x3PrevPrevL=x3PrevPrevR=0;
    y3PrevL=y3PrevR=y3PrevPrevL=y3PrevPrevR=0;
}

void TripleBiquadAudioProcessor::releaseResources() {}

void TripleBiquadAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    float cutoff1 = apvts.getRawParameterValue("cutoff1")->load();
    float cutoff2 = apvts.getRawParameterValue("cutoff2")->load();
    float cutoff3 = apvts.getRawParameterValue("cutoff3")->load();
    float Q1      = apvts.getRawParameterValue("q1")->load();
    float Q2      = apvts.getRawParameterValue("q2")->load();
    float Q3      = apvts.getRawParameterValue("q3")->load();

    double sr = getSampleRate();
    const float pi = 3.1415926535f;

    float w1   = 2*pi*(cutoff1/sr);
    float a1   = std::sin(w1)/(2*Q1);
    float b0_1 = (1-std::cos(w1))/2, b1_1 = 1-std::cos(w1), b2_1 = (1-std::cos(w1))/2;
    float A0_1 = 1+a1, A1_1 = -2*std::cos(w1), A2_1 = 1-a1;

    float w2   = 2*pi*(cutoff2/sr);
    float a2   = std::sin(w2)/(2*Q2);
    float b0_2 = (1-std::cos(w2))/2, b1_2 = 1-std::cos(w2), b2_2 = (1-std::cos(w2))/2;
    float A0_2 = 1+a2, A1_2 = -2*std::cos(w2), A2_2 = 1-a2;

    float w3   = 2*pi*(cutoff3/sr);
    float a3   = std::sin(w3)/(2*Q3);
    float b0_3 = (1-std::cos(w3))/2, b1_3 = 1-std::cos(w3), b2_3 = (1-std::cos(w3))/2;
    float A0_3 = 1+a3, A1_3 = -2*std::cos(w3), A2_3 = 1-a3;

    auto* leftChannel  = buffer.getWritePointer(0);
    auto* rightChannel = buffer.getWritePointer(1);

    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        float xL = leftChannel[i], xR = rightChannel[i];

        float yL1 = (b0_1/A0_1*xL)+(b1_1/A0_1*x1PrevL)+(b2_1/A0_1*x1PrevPrevL)
                   -(A1_1/A0_1*y1PrevL)-(A2_1/A0_1*y1PrevPrevL);
        float yR1 = (b0_1/A0_1*xR)+(b1_1/A0_1*x1PrevR)+(b2_1/A0_1*x1PrevPrevR)
                   -(A1_1/A0_1*y1PrevR)-(A2_1/A0_1*y1PrevPrevR);
        x1PrevPrevL=x1PrevL; x1PrevPrevR=x1PrevR;
        x1PrevL=xL; x1PrevR=xR;
        y1PrevPrevL=y1PrevL; y1PrevPrevR=y1PrevR;
        y1PrevL=yL1; y1PrevR=yR1;

        float yL2 = (b0_2/A0_2*yL1)+(b1_2/A0_2*x2PrevL)+(b2_2/A0_2*x2PrevPrevL)
                   -(A1_2/A0_2*y2PrevL)-(A2_2/A0_2*y2PrevPrevL);
        float yR2 = (b0_2/A0_2*yR1)+(b1_2/A0_2*x2PrevR)+(b2_2/A0_2*x2PrevPrevR)
                   -(A1_2/A0_2*y2PrevR)-(A2_2/A0_2*y2PrevPrevR);
        x2PrevPrevL=x2PrevL; x2PrevPrevR=x2PrevR;
        x2PrevL=yL1; x2PrevR=yR1;
        y2PrevPrevL=y2PrevL; y2PrevPrevR=y2PrevR;
        y2PrevL=yL2; y2PrevR=yR2;

        float yL3 = (b0_3/A0_3*yL2)+(b1_3/A0_3*x3PrevL)+(b2_3/A0_3*x3PrevPrevL)
                   -(A1_3/A0_3*y3PrevL)-(A2_3/A0_3*y3PrevPrevL);
        float yR3 = (b0_3/A0_3*yR2)+(b1_3/A0_3*x3PrevR)+(b2_3/A0_3*x3PrevPrevR)
                   -(A1_3/A0_3*y3PrevR)-(A2_3/A0_3*y3PrevPrevR);
        x3PrevPrevL=x3PrevL; x3PrevPrevR=x3PrevR;
        x3PrevL=yL2; x3PrevR=yR2;
        y3PrevPrevL=y3PrevL; y3PrevPrevR=y3PrevR;
        y3PrevL=yL3; y3PrevR=yR3;

        leftChannel[i]  = yL3;
        rightChannel[i] = yR3;
    }
}

juce::AudioProcessorEditor* TripleBiquadAudioProcessor::createEditor()
{
    return new TripleBiquadAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TripleBiquadAudioProcessor();
}
