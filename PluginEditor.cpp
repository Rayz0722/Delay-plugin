/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Mu45DelayAudioProcessorEditor::Mu45DelayAudioProcessorEditor (Mu45DelayAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 500);
    auto& params = processor.getParameters();
    juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(0);
    timeSlider.setBounds(150,50,120,100);
    timeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    timeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    timeSlider.setRange(audioParam->range.start, audioParam->range.end);
    timeSlider.setValue(*audioParam);
    timeSlider.setNumDecimalPlacesToDisplay(2);
    timeSlider.setTextValueSuffix ("ms");
    timeSlider.setDoubleClickReturnValue(true, 10.0f);
    timeSlider.addListener(this);
    addAndMakeVisible(timeSlider);
    
    
    audioParam = (juce::AudioParameterFloat*)params.getUnchecked(1);
    DryWetMixSlider.setBounds(30,200,120,100);
    DryWetMixSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    DryWetMixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    DryWetMixSlider.setRange(audioParam->range.start, audioParam->range.end);
    DryWetMixSlider.setValue(*audioParam);
    DryWetMixSlider.setNumDecimalPlacesToDisplay(2);
    DryWetMixSlider.setTextValueSuffix ("%");
    DryWetMixSlider.setDoubleClickReturnValue(true, 50.0f);
    DryWetMixSlider.addListener(this);
    addAndMakeVisible(DryWetMixSlider);
    
    
    audioParam = (juce::AudioParameterFloat*)params.getUnchecked(2);
    fbGainSlider.setBounds(270,200,120,100);
    fbGainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    fbGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    fbGainSlider.setRange(audioParam->range.start, audioParam->range.end);
    fbGainSlider.setValue(*audioParam);
    fbGainSlider.setNumDecimalPlacesToDisplay(2);
    fbGainSlider.setTextValueSuffix ("Db");
    fbGainSlider.setDoubleClickReturnValue(true, -1.0f);
    fbGainSlider.addListener(this);
    addAndMakeVisible(fbGainSlider);
    
    audioParam = (juce::AudioParameterFloat*)params.getUnchecked(3);
    mHPFcSlider.setBounds(70, 350, 120, 100);
    mHPFcSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mHPFcSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    mHPFcSlider.setRange(audioParam->range.start, audioParam->range.end);
    mHPFcSlider.setValue(*audioParam);
    mHPFcSlider.setNumDecimalPlacesToDisplay(2);
    mHPFcSlider.setTextValueSuffix ("Hz");
    mHPFcSlider.setDoubleClickReturnValue(true, 40.0f);
    mHPFcSlider.addListener(this);
    addAndMakeVisible(mHPFcSlider);
    
    audioParam = (juce::AudioParameterFloat*)params.getUnchecked(4);
    mLPFcSlider.setBounds(240, 350, 120, 100);
    mLPFcSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mLPFcSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    mLPFcSlider.setRange(audioParam->range.start, audioParam->range.end);
    mLPFcSlider.setValue(*audioParam);
    mLPFcSlider.setNumDecimalPlacesToDisplay(2);
    mLPFcSlider.setTextValueSuffix ("Hz");
    mLPFcSlider.setDoubleClickReturnValue(true, 20000.0f);
    mLPFcSlider.addListener(this);
    addAndMakeVisible(mLPFcSlider);
    
    randomButton.setBounds(160, 250, 100, 20);
    randomButton.addListener(this);
    addAndMakeVisible(randomButton);
    
    juce::Font customFont(15.0f);
    timeLabel.setText("Time", juce::dontSendNotification);
    timeLabel.setBounds(timeSlider.getX()+40, timeSlider.getY() - 25, timeSlider.getWidth(), 20);
    timeLabel.setFont(customFont);
    addAndMakeVisible(timeLabel);
    
    DryWetMixLabel.setText("Dry/Wet Mix", juce::dontSendNotification);
    DryWetMixLabel.setBounds(DryWetMixSlider.getX()+20, DryWetMixSlider.getY() - 25, DryWetMixSlider.getWidth(), 20);
    DryWetMixLabel.setFont(customFont);
    addAndMakeVisible(DryWetMixLabel);
    
    fbGainLabel.setText("Feedback Gain", juce::dontSendNotification);
    fbGainLabel.setBounds(fbGainSlider.getX()+20, fbGainSlider.getY() - 25, fbGainSlider.getWidth(), 20);
    fbGainLabel.setFont(customFont);
    addAndMakeVisible(fbGainLabel);
    
    mHPFcLabel.setText("HP Cutoff", juce::dontSendNotification);
    mHPFcLabel.setBounds(mHPFcSlider.getX()+25, mHPFcSlider.getY() - 25, mHPFcSlider.getWidth(), 20);
    mHPFcLabel.setFont(customFont);
    addAndMakeVisible(mHPFcLabel);
    
    mLPFcLabel.setText("LP Cutoff", juce::dontSendNotification);
    mLPFcLabel.setBounds(mLPFcSlider.getX()+25, mLPFcSlider.getY() - 25, mLPFcSlider.getWidth(), 20);
    mLPFcLabel.setFont(customFont);
    addAndMakeVisible(mLPFcLabel);
}

void Mu45DelayAudioProcessorEditor::sliderValueChanged(juce::Slider * slider)
{
    auto& params = processor.getParameters();
    if (slider == &timeSlider)
    {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(0);
        *audioParam = timeSlider.getValue();
        DBG("time slider changed");
        audioProcessor.updateDelays();
    }
    else if (slider == &DryWetMixSlider)
    {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(1);
        *audioParam = DryWetMixSlider.getValue();
        DBG("Mix slider changed");
    }
    else if (slider == &fbGainSlider)
    {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(2);
        *audioParam = fbGainSlider.getValue();
        DBG("feedback gain slider changed");
    }
    else if (slider == &mHPFcSlider)
    {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(3);
        *audioParam = mHPFcSlider.getValue();
        DBG("HP slider changed");
    }
    else if (slider == &mLPFcSlider)
    {
        juce::AudioParameterFloat* audioParam = (juce::AudioParameterFloat*)params.getUnchecked(4);
        *audioParam = mLPFcSlider.getValue();
        DBG("LP slider changed");
    }
    
}
float Mu45DelayAudioProcessorEditor::randomGenerator(float min, float max) {
    float value = juce::Random::getSystemRandom().nextFloat() * (max - min) + min;
    return std::round(value * 100.0f) / 100.0f;
}

void Mu45DelayAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &randomButton)
    {
        // Set random values to sliders
        timeSlider.setValue(randomGenerator(0, 1000));
        DryWetMixSlider.setValue(randomGenerator(0,100));
        fbGainSlider.setValue(randomGenerator(-1,-12));
        mHPFcSlider.setValue(randomGenerator(40,1000));
        mLPFcSlider.setValue(randomGenerator(1000,10000));
    }
}
Mu45DelayAudioProcessorEditor::~Mu45DelayAudioProcessorEditor()
{
}

//==============================================================================
void Mu45DelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void Mu45DelayAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
