#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{

    setSize (400, 400);
    tempo.setSliderStyle(juce::Slider::LinearVertical);
    tempo.setRange(0.0, 1.0, 0.01);
    tempo.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    tempo.setPopupDisplayEnabled(true, false, this);
    tempo.setTextValueSuffix(" BPM");
    tempo.setValue(0.5);
    tempo.addListener(this);

    // add the child component tempo to the parent component (the Editor here)
    addAndMakeVisible (&tempo);
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
    tempo.setBounds (0, 0, 180, 180);
}


void AudioPluginAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
    processorRef.newSpeedVal = tempo.getValue();
}