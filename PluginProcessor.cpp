/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Mu45DelayAudioProcessor::Mu45DelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    addParameter(delaytimeParam = new juce::AudioParameterFloat("delay time (ms)", // parameterID,
        "Delaytime", // parameterName,
        0.0f, // minValue,
        1000.0f, // maxValue,
        0.0f)); // defaultValue
    addParameter(DryWetMixParam = new juce::AudioParameterFloat("mix percentage (%)", // parameterID,
        "Mix", // parameterName,
        0.0f, // minValue,
        100.0f, // maxValue,
        50.0f)); // defaultValue
    addParameter(fbGainParam = new juce::AudioParameterFloat("feedback gain (db)", // parameterID,
        "feedback", // parameterName,
        -12.0f, // minValue,
        -1.0f, // maxValue,
        -1.0f)); // defaultValue
    addParameter(mHPFcParam = new juce::AudioParameterFloat("HP Filter Cutoff (Hz)", // parameterID,
        "HPFiltFc", // parameterName,
        40.0f, // minValue,
        20000.0f, // maxValue,
        40.0f)); // defaultValue
    addParameter(mLPFcParam = new juce::AudioParameterFloat("LP Filter Cutoff (Hz)", // parameterID,
        "LPFiltFc", // parameterName,
        40.0f, // minValue,
        20000.0f, // maxValue,
        20000.0f)); // defaultValue
}

Mu45DelayAudioProcessor::~Mu45DelayAudioProcessor()
{
}

//==============================================================================
const juce::String Mu45DelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Mu45DelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Mu45DelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Mu45DelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Mu45DelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Mu45DelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Mu45DelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Mu45DelayAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Mu45DelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void Mu45DelayAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

int Mu45DelayAudioProcessor::calcDelaySampsFromMsec(float msec)
{
    return static_cast<int>(msec * mFs / 1000.0f);
}

// set the delays from the parameter
void Mu45DelayAudioProcessor::calcDelays()
{
    float msec = delaytimeParam->get();
    int samps = calcDelaySampsFromMsec(msec);
    delay1L.setDelay(samps);
    delay1R.setDelay(samps);
}

//==============================================================================
void Mu45DelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    mFs = sampleRate;
    
    int samps = calcDelaySampsFromMsec(maxDelayMsec);
    delay1L.setMaximumDelay(samps);
    delay1R.setMaximumDelay(samps);
    calcDelays();
}

void Mu45DelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Mu45DelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif
void Mu45DelayAudioProcessor::calcAlgorithmParams()
{
    float gainDb = fbGainParam->get();
    if (gainDb < -24.0)
        fb = 0.0;
    else
        fb = powf(10.0, gainDb/20.0);
    
    float hpfc = mHPFcParam->get();
    float lpfc = mLPFcParam->get();
    
    float hpcoeffs[5];
    Mu45FilterCalc::calcCoeffsHPF(hpcoeffs, hpfc, 1.0, mFs);
    float lpcoeffs[5];
    Mu45FilterCalc::calcCoeffsLPF(lpcoeffs, lpfc, 1.0, mFs);
    
    mHPL.setCoefficients(hpcoeffs[0], hpcoeffs[1], hpcoeffs[2], hpcoeffs[3], hpcoeffs[4]);
    mHPR.setCoefficients(hpcoeffs[0], hpcoeffs[1], hpcoeffs[2], hpcoeffs[3], hpcoeffs[4]);
    
    mLPL.setCoefficients(lpcoeffs[0], lpcoeffs[1], lpcoeffs[2], lpcoeffs[3], lpcoeffs[4]);
    mLPR.setCoefficients(lpcoeffs[0], lpcoeffs[1], lpcoeffs[2], lpcoeffs[3], lpcoeffs[4]);
    
}
void Mu45DelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    calcAlgorithmParams();
    float dryGain = 1.0 - 1.0 *DryWetMixParam->get()/100.0;
    float wetGain = 1.0 * DryWetMixParam->get()/100.0;
    // This version assumes we always have 2 channels
    float* channelDataL = buffer.getWritePointer (0);
    float* channelDataR = buffer.getWritePointer (1);
    float tempL, tempR, temp2L, temp2R;
    //float filterL,filterR;
    // The "inner loop"
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        
        //temp1 =delay1.nextOut();
        //temp2 = temp1*fb+ inSample*wetGain;
        //delay1.tick(temp2);
        //outSample = inSample*dryGain +temp1
        
        
        tempL = delay1L.nextOut();
        tempR = delay1R.nextOut();
        
        tempL = mHPL.tick(tempL);
        tempR = mHPR.tick(tempR);
        tempL = mLPL.tick(tempL);
        tempR = mLPR.tick(tempR);
        
        temp2L =tempL *fb+ channelDataL[i]* wetGain;
        temp2R =tempR *fb+ channelDataR[i]* wetGain;
        //temp2L =tempL*fb + channelDataL[i]* wetGain;
        //temp2R =tempR*fb + channelDataR[i]* wetGain;
        
        delay1L.tick(temp2L);
        delay1R.tick(temp2R);
        
        
        channelDataL[i] = dryGain *channelDataL[i] + tempL;
        channelDataR[i] = dryGain *channelDataR[i] + tempR;
        
    }
}

//==============================================================================
bool Mu45DelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Mu45DelayAudioProcessor::createEditor()
{
    return new Mu45DelayAudioProcessorEditor (*this);
}

//==============================================================================
void Mu45DelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Mu45DelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Mu45DelayAudioProcessor();
}
