#pragma once

#include "PluginProcessor.h"

//==============================================================================
class AudioPluginAudioProcessorEditor  : public juce::AudioProcessorEditor, private
    juce::Slider::Listener // Add Listener class (1)
{
public:
    AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::Slider tempo;
    void sliderValueChanged (juce::Slider* slider) override; // Add the callback function
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> tempoSliderAttachment;
    AudioPluginAudioProcessor& processorRef;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
