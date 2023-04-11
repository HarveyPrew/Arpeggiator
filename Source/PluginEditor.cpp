#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (200, 200);

    //parameter of button object
    midiVolume.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    midiVolume.setRange(0.0, 127.0, 1.0);
    midiVolume.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    midiVolume.setPopupDisplayEnabled(true, false, this);
    midiVolume.setTextValueSuffix("MIDI Velocity");
    midiVolume.setValue(1.0);

    // adds the child component midiVolume to the parent component (the editor here)
    addAndMakeVisible(&midiVolume);

}

PluginEditor::~PluginEditor()
{

}

//==============================================================================
void PluginEditor::paint (juce::Graphics& g)
{
    // Fill the whole window white
    g.fillAll (juce::Colours::white);

    // Set the current drawing colour to black
    g.setColour (juce::Colours::black);

    // Set the font size and draw text to screen
    g.setFont (15.0f);

    g.drawFittedText ("Midi Volume", 0, 0, getWidth(), 30, juce::Justification::centred, 1);
}

void PluginEditor::resized()
{
    // Position and size of the slider with arguments (x, y, width, height)
    midiVolume.setBounds(40, 30, 20, getHeight() - 60);
}

void PluginEditor::sliderValueChanged (juce::Slider* slider)
{
    processorRef.noteOnVel = midiVolume.getValue();
}
