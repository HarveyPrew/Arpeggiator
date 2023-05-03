#pragma once

#include "PluginProcessor.h"

//==============================================================================
class AudioPluginAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::Slider tempo;
    juce::ComboBox mode;
    juce::ToggleButton quaver;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> tempoSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> quaverAttachment;
    AudioPluginAudioProcessor& processorRef;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
