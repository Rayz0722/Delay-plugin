/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Mu45FilterCalc/Mu45FilterCalc.h"
#include "StkLite-4.6.2/Delay.h"
#include "StkLite-4.6.2/BiQuad.h"
//==============================================================================
/**
*/
class Mu45DelayAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    Mu45DelayAudioProcessor();
    ~Mu45DelayAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    void updateDelays()   
    {
        calcDelays();
    }

private:
    //==============================================================================
    float mFs; // Sampling rate
    const float maxDelayMsec = 1000.0f;
    float fb; //linear gain of feedback
    
    void calcAlgorithmParams();
    void calcDelays();
    int calcDelaySampsFromMsec(float time);
    
    //User parameters
    juce::AudioParameterFloat* DryWetMixParam; //wet and dry mix
    juce::AudioParameterFloat* fbGainParam; //feedback gain
    juce::AudioParameterFloat* delaytimeParam; //delay time
    
    stk::Delay delay1L, delay1R;
    
    juce::AudioParameterFloat* mHPFcParam; //high pass frequency parameter
    juce::AudioParameterFloat* mLPFcParam; //low pass frequency parameter
    stk::BiQuad mHPL,mHPR; //high pas
    stk::BiQuad mLPL,mLPR; // low pass
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Mu45DelayAudioProcessor)
};
