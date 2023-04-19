#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{

    setSize (400, 400); // Pixel Dimensions of plugin (400x400 pixels(

    // ----------------------------------- Tempo -----------------------------------
    tempo.setSliderStyle(juce::Slider::LinearVertical); // A vertical slider is selected
    tempo.setRange(20.0, 200.0, 1); // Range of tempo parameter
    tempo.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20); // Dimensions of text box showing value
    tempo.setPopupDisplayEnabled(true, false, this); //BPM shown as popup
    tempo.setTextValueSuffix(" BPM"); // Showing units alongside with value
    tempo.setValue(120); // Inital value set to "120 BPM"
    tempo.addListener(this); // Inital value set to "120 BPM"

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
    processorRef.newTempoVal = tempo.getValue();
}