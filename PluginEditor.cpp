#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{

    setSize (400, 400);
    speed.setSliderStyle(juce::Slider::LinearVertical);
    speed.setRange(0.0, 1.0, 0.1);
    speed.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    speed.setPopupDisplayEnabled(true, false, this);
    speed.setTextValueSuffix(" Speed");
    speed.setValue(1.0);
    speed.addListener(this);

    // add the child component speed to the parent component (the Editor here)
    addAndMakeVisible (&speed);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
   // Fill the whole window white
    g.fillAll (juce::Colours::green);

    // Set current drawing colour to black
    g.setColour(juce::Colours::black);

    // Set the font size and draw text to the screen
    g.setFont(15.0f);

    g.drawFittedText ("Arpeggiator", 0, 0, getWidth(), 30, juce::Justification::centred, 1);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // Position and Size of the slider with arguments (x, y, width, height)
    speed.setBounds (0, 0, 180, 180);
}


void AudioPluginAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
    processorRef.noteOnVel = speed.getValue();
}