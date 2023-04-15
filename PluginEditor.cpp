#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{

    setSize (200, 200);
    midiVolume.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    midiVolume.setRange(0.0, 127.0, 1.0);
    midiVolume.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    midiVolume.setPopupDisplayEnabled(true, false, this);
    midiVolume.setTextValueSuffix("MIDI Velocity");
    midiVolume.setValue(1.0);

    // add the child component midiVolume to the parent component (the Editor here)
    addAndMakeVisible (&midiVolume);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
   // Fill the whole window white
    g.fillAll (juce::Colours::white);

    // Set current drawing colour to black
    g.setColour(juce::Colours::black);

    // Set the font size and draw text to the screen
    g.setFont(15.0f);

    g.drawFittedText ("Midi Volume", 0, 0, getWidth(), 30, juce::Justification::centred, 1);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // Position and Size of the slider with arguments (x, y, width, height)
    midiVolume.setBounds (getLocalBounds());
}
