/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
//==============================================================================
/**
*/
class Mu45DelayAudioProcessorEditor  : public juce::AudioProcessorEditor,public juce::Slider::Listener, public juce::Button::Listener
{
public:
    Mu45DelayAudioProcessorEditor (Mu45DelayAudioProcessor&);
    ~Mu45DelayAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;
    float randomGenerator(float min, float max);
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Mu45DelayAudioProcessor& audioProcessor;
    
    juce::Slider timeSlider;
    juce::Slider fbGainSlider;
    juce::Slider DryWetMixSlider;
    juce::Slider mHPFcSlider;
    juce::Slider mLPFcSlider;
    juce::TextButton randomButton {"Random"};
    

    juce::Label timeLabel;
    juce::Label fbGainLabel;
    juce::Label DryWetMixLabel;
    juce::Label mHPFcLabel;
    juce::Label mLPFcLabel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Mu45DelayAudioProcessorEditor)
};
