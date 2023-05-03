#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================
class AudioPluginAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;


    //An object of APVTS is made to store the parameters of the plugin
    juce::AudioProcessorValueTreeState apvts;

private:

    // Defining function that makes parameters.
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    int currentNote, lastNoteValue;
    int time;
    float rate;
    juce::SortedSet<int> notes;
    int counter;
    float pointerToFloat (juce::String parameterID);
    int calculateOffSet(int time, int numSamples, int noteDuration);
    int timeUpdater(int time, int numSamples, int noteDuration);
    bool notesAreHeld (juce::SortedSet<int> notes);
    bool notesAreNotHeld(juce::SortedSet<int> notes);
    bool timeForNoteChange(int time, int numSamples, int noteDuration);
    bool timeBetweenFirstAndSecondNote (int time, int numSamples, int noteDuration);
    void noteOnSenderFromNextNote (int& currentNote, juce::SortedSet<int>& notes, int& lastNoteValue, juce::MidiBuffer& midiMessages, int offset,int mode);
    void lastNoteOffMessageSender (juce::MidiBuffer& midiMessages,int& lastNoteValue, int offset, int& time, int& currentNote);
    void upNoteChanger (int& time, juce::MidiBuffer& midiMessages, int offset, int numSamples, int noteDuration, int mode);
    void downNoteChanger (int& time, juce::MidiBuffer& midiMessages, int offset, int numSamples, int noteDuration, int mode);
    void upDownNoteChanger(int numSamples, int noteDuration, juce::MidiBuffer& midiMessages, int offset, int mode);
    void insertNoteOffMessage(juce::MidiBuffer& midiMessages, int offset);
    void moveUpOneInSortedSet();
    void downNoteEquation();
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)

};
