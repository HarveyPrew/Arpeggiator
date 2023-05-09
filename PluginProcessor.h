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
    juce::AudioProcessorValueTreeState treeState;

private:

    // Defining function that makes parameters.
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    // Define variables used in arpeggiator
    int currentNote, lastNoteValue;
    int time;
    float rate;

    // Using sorted set to organise notes in arpeggiator.
    juce::SortedSet<int> notes;

    // Counter used in up-down mode to switch
    int upDownSwitch;

    // Get parameter value from Tree State.
    float getParameterValue (juce::String parameterID);

    // Calculate offset so note gets placed in right place.
    int calculateOffSet(int time, int numSamples, int noteDuration);

    // Update time after each processBlock called
    int timeUpdater(int time, int numSamples, int noteDuration);

    // Condition met when notes are held.
    bool notesAreHeld (juce::SortedSet<int> notes);

    // Condition met when notes are not held.
    bool notesAreNotHeld(juce::SortedSet<int> notes);

    // Condition met when it's time to change the note.
    bool timeForNoteChange(int time, int numSamples, int noteDuration);

    // Function for sending note on message.
    void noteOnSenderFromNextNote (int& currentNote, juce::SortedSet<int>& notes, int& lastNoteValue, juce::MidiBuffer& midiMessages, int offset,int mode);

    // Function for sending note off message for final note.
    void lastNoteOffMessageSender (juce::MidiBuffer& midiMessages,int& lastNoteValue, int offset, int& time, int& currentNote);

    // Function to change to the next highest note (or back to lowest note when highest is reached).
    void upNoteChanger (int& time, juce::MidiBuffer& midiMessages, int offset, int numSamples, int noteDuration, int mode);

    // Function to change to the next lowest note (or back to higest note when lowest is reached).
    void downNoteChanger (int& time, juce::MidiBuffer& midiMessages, int offset, int numSamples, int noteDuration, int mode);

    // Function used to alternate between up and down note changer.
    void upDownNoteChanger(int numSamples, int noteDuration, juce::MidiBuffer& midiMessages, int offset, int mode);

    // Function used to send a note off message.
    void insertNoteOffMessage(juce::MidiBuffer& midiMessages, int offset);

    // Functions below are used to move up and down in the sorted set. allowing one to change notes.
    void moveUpOneInSortedSet();
    void moveDownOneInSortedSet();
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)

};
